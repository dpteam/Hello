#include "markov.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "text_metrics.h"
#include "utf8.h"

#define MK_MAX_WORD_BYTES 48
#define MK_MAX_WORDS_PER_SENTENCE 64
#define MK_MAX_OUTPUT_WORDS 40
#define MK_START_TOKEN 0u
#define MK_END_TOKEN 1u
#define MK_FIRST_WORD 2u

/* ------------------------------------------------------------------ */
/* Хеш-таблица «ключ → индекс»                                        */
/* ------------------------------------------------------------------ */

typedef struct {
    uint64_t key; /* 0 — пусто */
    uint32_t value;
} MkSlot;

typedef struct {
    MkSlot *slots;
    size_t cap;
    size_t count;
} MkMap;

static uint64_t mk_hash_bytes(const void *data, size_t len)
{
    const unsigned char *p = (const unsigned char *)data;
    uint64_t h = 1469598103934665603ull; /* FNV-1a offset basis */
    for (size_t i = 0; i < len; ++i) {
        h ^= (uint64_t)p[i];
        h *= 1099511628211ull;
    }
    return h == 0 ? 1 : h;
}

static void mk_map_free(MkMap *map)
{
    free(map->slots);
    map->slots = NULL;
    map->cap = 0;
    map->count = 0;
}

static bool mk_map_reserve(MkMap *map, size_t want)
{
    if (map->cap >= want)
        return true;

    size_t cap = map->cap ? map->cap : 64;
    while (cap < want)
        cap *= 2;

    MkSlot *slots = (MkSlot *)calloc(cap, sizeof(MkSlot));
    if (slots == NULL)
        return false;

    for (size_t i = 0; i < map->cap; ++i) {
        if (map->slots[i].key == 0)
            continue;
        size_t idx = (size_t)(map->slots[i].key & (uint64_t)(cap - 1));
        while (slots[idx].key != 0)
            idx = (idx + 1) & (cap - 1);
        slots[idx] = map->slots[i];
    }

    free(map->slots);
    map->slots = slots;
    map->cap = cap;
    return true;
}

static uint32_t mk_map_get(const MkMap *map, uint64_t key)
{
    if (map->cap == 0 || key == 0)
        return UINT32_MAX;

    size_t idx = (size_t)(key & (uint64_t)(map->cap - 1));
    size_t guard = 0;
    while (map->slots[idx].key != 0 && guard++ <= map->cap) {
        if (map->slots[idx].key == key)
            return map->slots[idx].value;
        idx = (idx + 1) & (map->cap - 1);
    }
    return UINT32_MAX;
}

/* Возвращает true, если ключ новый. */
static bool mk_map_put(MkMap *map, uint64_t key, uint32_t value)
{
    if (key == 0)
        return false;
    if (map->count * 4 >= map->cap * 3) {
        if (!mk_map_reserve(map, map->cap * 2 + 8))
            return false;
    }

    size_t idx = (size_t)(key & (uint64_t)(map->cap - 1));
    while (map->slots[idx].key != 0) {
        if (map->slots[idx].key == key)
            return false;
        idx = (idx + 1) & (map->cap - 1);
    }

    map->slots[idx].key = key;
    map->slots[idx].value = value;
    ++map->count;
    return true;
}

/* ------------------------------------------------------------------ */
/* Переходы                                                           */
/* ------------------------------------------------------------------ */

typedef struct {
    uint32_t token;
    uint32_t count;
} MkTransition;

typedef struct {
    MkTransition *items;
    size_t count;
    size_t cap;
} MkList;

static void mk_list_free(MkList *list)
{
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->cap = 0;
}

static bool mk_list_add(MkList *list, uint32_t token)
{
    for (size_t i = 0; i < list->count; ++i) {
        if (list->items[i].token == token) {
            if (list->items[i].count < UINT32_MAX)
                ++list->items[i].count;
            return true;
        }
    }

    if (list->count == list->cap) {
        size_t cap = list->cap ? list->cap * 2 : 4;
        MkTransition *grown =
            (MkTransition *)realloc(list->items, cap * sizeof(MkTransition));
        if (grown == NULL)
            return false;
        list->items = grown;
        list->cap = cap;
    }

    list->items[list->count].token = token;
    list->items[list->count].count = 1;
    ++list->count;
    return true;
}

/* ------------------------------------------------------------------ */
/* Модель                                                             */
/* ------------------------------------------------------------------ */

struct Markov {
    unsigned order;
    unsigned seed;
    unsigned rng;

    char **words;
    size_t word_count;
    size_t word_cap;
    MkMap word_index;

    MkList *unigram; /* переходы по одному слову */
    size_t unigram_cap;

    MkMap bigram_index;
    MkList *bigrams;
    size_t bigram_count;
    size_t bigram_cap;

    size_t tokens_seen;
};

static uint32_t mk_rng_next(Markov *model)
{
    uint32_t x = model->rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    if (x == 0)
        x = 0x9E3779B9u;
    model->rng = x;
    return x;
}

static double mk_rng_unit(Markov *model)
{
    return (double)(mk_rng_next(model) >> 8) / 16777216.0;
}

static uint32_t mk_intern(Markov *model, const char *word, size_t len)
{
    uint64_t key = mk_hash_bytes(word, len);
    uint32_t existing = mk_map_get(&model->word_index, key);
    if (existing != UINT32_MAX)
        return existing;

    if (model->word_count == model->word_cap) {
        size_t cap = model->word_cap ? model->word_cap * 2 : 64;
        char **grown = (char **)realloc(model->words, cap * sizeof(char *));
        if (grown == NULL)
            return UINT32_MAX;
        model->words = grown;
        model->word_cap = cap;
    }

    char *copy = utf8_dup_range(word, len);
    if (copy == NULL)
        return UINT32_MAX;

    uint32_t id = (uint32_t)model->word_count;
    model->words[model->word_count++] = copy;

    if (!mk_map_put(&model->word_index, key, id)) {
        /* Коллизия хеша: слово всё равно добавлено, но не найдётся.
         * Вероятность исчезающе мала на реальных словарях. */
        (void)0;
    }
    return id;
}

static const char *mk_word(const Markov *model, uint32_t token)
{
    if (token >= model->word_count)
        return "";
    return model->words[token];
}

static MkList *mk_bigram_list(Markov *model, uint32_t a, uint32_t b)
{
    uint64_t key = ((uint64_t)a << 32) | (uint64_t)b;
    uint32_t idx = mk_map_get(&model->bigram_index, key);
    if (idx != UINT32_MAX)
        return &model->bigrams[idx];

    if (model->bigram_count == model->bigram_cap) {
        size_t cap = model->bigram_cap ? model->bigram_cap * 2 : 64;
        MkList *grown =
            (MkList *)realloc(model->bigrams, cap * sizeof(MkList));
        if (grown == NULL)
            return NULL;
        model->bigrams = grown;
        model->bigram_cap = cap;
    }

    MkList *list = &model->bigrams[model->bigram_count];
    memset(list, 0, sizeof(*list));

    if (!mk_map_put(&model->bigram_index, key, (uint32_t)model->bigram_count))
        return NULL;

    ++model->bigram_count;
    return list;
}

Markov *markov_create(unsigned order, unsigned seed)
{
    if (order < 1)
        order = 1;
    if (order > MARKOV_MAX_ORDER)
        order = MARKOV_MAX_ORDER;

    Markov *model = (Markov *)calloc(1, sizeof(*model));
    if (model == NULL)
        return NULL;

    model->order = order;
    model->seed = seed ? seed : 1u;
    model->rng = model->seed;

    /* Токены 0 и 1 зарезервированы под границы предложения. */
    model->word_cap = 64;
    model->words = (char **)calloc(model->word_cap, sizeof(char *));
    if (model->words == NULL) {
        free(model);
        return NULL;
    }

    model->words[0] = utf8_dup("<s>");
    model->words[1] = utf8_dup("</s>");
    if (model->words[0] == NULL || model->words[1] == NULL) {
        markov_destroy(model);
        return NULL;
    }
    model->word_count = MK_FIRST_WORD;

    if (!mk_map_reserve(&model->word_index, 128)) {
        markov_destroy(model);
        return NULL;
    }
    mk_map_put(&model->word_index, mk_hash_bytes("<s>", 3), MK_START_TOKEN);
    mk_map_put(&model->word_index, mk_hash_bytes("</s>", 4), MK_END_TOKEN);

    model->unigram = (MkList *)calloc(128, sizeof(MkList));
    if (model->unigram == NULL) {
        markov_destroy(model);
        return NULL;
    }
    model->unigram_cap = 128;
    return model;
}

void markov_reset(Markov *model)
{
    if (model == NULL)
        return;

    for (size_t i = MK_FIRST_WORD; i < model->word_count; ++i)
        free(model->words[i]);
    model->word_count = MK_FIRST_WORD;

    for (size_t i = 0; i < model->bigram_count; ++i)
        mk_list_free(&model->bigrams[i]);
    model->bigram_count = 0;

    mk_map_free(&model->word_index);
    mk_map_free(&model->bigram_index);

    /* unigram пересоздаём под текущий размер словаря. */
    for (size_t i = 0; i < model->unigram_cap; ++i)
        mk_list_free(&model->unigram[i]);
    free(model->unigram);
    model->unigram = (MkList *)calloc(128, sizeof(MkList));
    model->unigram_cap = model->unigram != NULL ? 128 : 0;

    mk_map_reserve(&model->word_index, 128);
    mk_map_put(&model->word_index, mk_hash_bytes("<s>", 3), MK_START_TOKEN);
    mk_map_put(&model->word_index, mk_hash_bytes("</s>", 4), MK_END_TOKEN);

    model->tokens_seen = 0;
    model->rng = model->seed;
}

void markov_destroy(Markov *model)
{
    if (model == NULL)
        return;

    for (size_t i = 0; i < model->word_count; ++i)
        free(model->words[i]);
    free(model->words);

    for (size_t i = 0; i < model->bigram_count; ++i)
        mk_list_free(&model->bigrams[i]);
    free(model->bigrams);

    if (model->unigram != NULL) {
        for (size_t i = 0; i < model->unigram_cap; ++i)
            mk_list_free(&model->unigram[i]);
        free(model->unigram);
    }

    mk_map_free(&model->word_index);
    mk_map_free(&model->bigram_index);
    free(model);
}

/* Расширяет unigram-массив под новый токен. */
static bool mk_grow_unigram(Markov *model, size_t need)
{
    if (model->unigram == NULL)
        return false;

    if (need <= model->unigram_cap)
        return true;

    size_t cap = model->unigram_cap ? model->unigram_cap : 128;
    while (cap < need)
        cap *= 2;

    MkList *grown = (MkList *)realloc(model->unigram, cap * sizeof(MkList));
    if (grown == NULL)
        return false;

    memset(grown + model->unigram_cap, 0,
           (cap - model->unigram_cap) * sizeof(MkList));
    model->unigram = grown;
    model->unigram_cap = cap;
    return true;
}

static void mk_train_sentence(Markov *model, const char *text, size_t len)
{
    uint32_t ids[MK_MAX_WORDS_PER_SENTENCE];
    size_t count = 0;

    ids[count++] = MK_START_TOKEN;

    size_t pos = 0;
    while (pos < len && count + 1 < MK_MAX_WORDS_PER_SENTENCE) {
        /* Пропускаем шум. */
        uint32_t cp = 0;
        int step = utf8_decode(text + pos, len - pos, &cp);
        if (step <= 0) {
            ++pos;
            continue;
        }
        if (utf8_is_noise(cp)) {
            pos += (size_t)step;
            continue;
        }

        size_t start = pos;
        while (pos < len) {
            step = utf8_decode(text + pos, len - pos, &cp);
            if (step <= 0)
                break;
            if (utf8_is_noise(cp))
                break;
            pos += (size_t)step;
        }

        size_t word_len = pos - start;
        if (word_len == 0 || word_len > MK_MAX_WORD_BYTES)
            continue;

        uint32_t id = mk_intern(model, text + start, word_len);
        if (id == UINT32_MAX)
            return;
        ids[count++] = id;
    }

    if (count <= 1)
        return;

    ids[count++] = MK_END_TOKEN;

    if (!mk_grow_unigram(model, model->word_count + 8))
        return;

    for (size_t i = 0; i + 1 < count; ++i) {
        uint32_t from = ids[i];
        uint32_t to = ids[i + 1];

        if (from < model->word_count)
            mk_list_add(&model->unigram[from], to);

        if (model->order >= 2 && i > 0) {
            MkList *list = mk_bigram_list(model, ids[i - 1], from);
            if (list != NULL)
                mk_list_add(list, to);
        }

        ++model->tokens_seen;
    }
}

void markov_train(Markov *model, const char *utf8_text)
{
    if (model == NULL || utf8_text == NULL)
        return;

    size_t len = strlen(utf8_text);
    size_t start = 0;

    /* Режем по переводам строк и точке/восклицательному/вопросу. */
    for (size_t i = 0; i <= len; ++i) {
        bool boundary = (i == len);
        if (!boundary) {
            unsigned char c = (unsigned char)utf8_text[i];
            if (c == '\n' || c == '\r' || c == '.' || c == '!' || c == '?')
                boundary = true;
        }
        if (!boundary)
            continue;

        if (i > start)
            mk_train_sentence(model, utf8_text + start, i - start);
        start = i + 1;
    }
}

/* Выбирает переход из списка с учётом температуры. */
static bool mk_pick(Markov *model, const MkList *list, double temperature,
                    uint32_t *out)
{
    if (list == NULL || list->count == 0)
        return false;

    double weights[MK_MAX_OUTPUT_WORDS * 8];
    double probs[MK_MAX_OUTPUT_WORDS * 8];
    size_t n = list->count;
    if (n > sizeof(weights) / sizeof(weights[0]))
        n = sizeof(weights) / sizeof(weights[0]);

    for (size_t i = 0; i < n; ++i)
        weights[i] = (double)list->items[i].count;

    tm_power_dist(weights, n, temperature, probs);

    double roll = mk_rng_unit(model);
    for (size_t i = 0; i < n; ++i) {
        roll -= probs[i];
        if (roll <= 0.0) {
            *out = list->items[i].token;
            return true;
        }
    }

    *out = list->items[n - 1].token;
    return true;
}

bool markov_generate(Markov *model, const char *prompt, double temperature,
                     char *out, size_t cap)
{
    if (model == NULL || out == NULL || cap == 0)
        return false;

    out[0] = '\0';
    if (model->tokens_seen == 0)
        return false;

    if (temperature <= 0.05)
        temperature = 0.05;

    /* Контекст из подсказки. */
    uint32_t context[MARKOV_MAX_ORDER];
    size_t ctx_len = 0;

    if (prompt != NULL && prompt[0] != '\0') {
        uint32_t ids[MK_MAX_WORDS_PER_SENTENCE];
        size_t count = 0;
        size_t len = strlen(prompt);
        size_t pos = 0;

        while (pos < len && count < MK_MAX_WORDS_PER_SENTENCE) {
            uint32_t cp = 0;
            int step = utf8_decode(prompt + pos, len - pos, &cp);
            if (step <= 0) {
                ++pos;
                continue;
            }
            if (utf8_is_noise(cp)) {
                pos += (size_t)step;
                continue;
            }

            size_t start = pos;
            while (pos < len) {
                step = utf8_decode(prompt + pos, len - pos, &cp);
                if (step <= 0 || utf8_is_noise(cp))
                    break;
                pos += (size_t)step;
            }

            uint64_t key = mk_hash_bytes(prompt + start, pos - start);
            uint32_t id = mk_map_get(&model->word_index, key);
            if (id == UINT32_MAX)
                break;
            ids[count++] = id;
        }

        size_t take = model->order - 1;
        if (take > count)
            take = count;
        for (size_t i = 0; i < take; ++i)
            context[i] = ids[count - take + i];
        ctx_len = take;
    }

    /* Пустой контекст — начинаем как новое предложение. */
    if (ctx_len == 0) {
        context[0] = MK_START_TOKEN;
        ctx_len = 1;
        if (model->order >= 2) {
            context[1] = MK_START_TOKEN;
            ctx_len = 2;
        }
    }

    size_t written = 0;
    size_t words = 0;
    bool any = false;

    while (words < MK_MAX_OUTPUT_WORDS) {
        uint32_t next = 0;
        bool picked = false;

        /* Бэкофф: сначала полный контекст, потом короче. */
        if (model->order >= 2 && ctx_len >= 2) {
            uint64_t key = ((uint64_t)context[ctx_len - 2] << 32) |
                           (uint64_t)context[ctx_len - 1];
            uint32_t idx = mk_map_get(&model->bigram_index, key);
            if (idx != UINT32_MAX)
                picked = mk_pick(model, &model->bigrams[idx], temperature, &next);
        }

        if (!picked) {
            uint32_t last = context[ctx_len - 1];
            if (last < model->word_count)
                picked = mk_pick(model, &model->unigram[last], temperature, &next);
        }

        if (!picked) {
            /* Совсем тупик: начинаем с начала предложения. */
            picked = mk_pick(model, &model->unigram[MK_START_TOKEN], temperature,
                             &next);
        }

        if (!picked || next == MK_END_TOKEN)
            break;

        const char *word = mk_word(model, next);
        if (word[0] == '\0')
            break;

        size_t wlen = strlen(word);
        if (written + wlen + 2 >= cap)
            break;

        if (written > 0)
            out[written++] = ' ';
        memcpy(out + written, word, wlen);
        written += wlen;
        out[written] = '\0';
        any = true;

        /* Сдвигаем контекст. */
        if (ctx_len < model->order) {
            context[ctx_len++] = next;
        } else {
            for (size_t i = 1; i < ctx_len; ++i)
                context[i - 1] = context[i];
            context[ctx_len - 1] = next;
        }

        ++words;
    }

    return any;
}

double markov_perplexity(const Markov *model, const char *utf8_text)
{
    if (model == NULL || utf8_text == NULL || model->tokens_seen == 0)
        return 0.0;

    /* Разбираем на токены так же, как при обучении.
     * Незнакомые слова не обрывают разбор: они становятся
     * «сюрпризом» с фиксированной вероятностью. */
    uint32_t ids[MK_MAX_WORDS_PER_SENTENCE];
    size_t count = 0;
    ids[count++] = MK_START_TOKEN;

    size_t len = strlen(utf8_text);
    size_t pos = 0;
    size_t unknown = 0;

    while (pos < len && count + 1 < MK_MAX_WORDS_PER_SENTENCE) {
        uint32_t cp = 0;
        int step = utf8_decode(utf8_text + pos, len - pos, &cp);
        if (step <= 0) {
            ++pos;
            continue;
        }
        if (utf8_is_noise(cp)) {
            pos += (size_t)step;
            continue;
        }

        size_t start = pos;
        while (pos < len) {
            step = utf8_decode(utf8_text + pos, len - pos, &cp);
            if (step <= 0 || utf8_is_noise(cp))
                break;
            pos += (size_t)step;
        }

        uint64_t key = mk_hash_bytes(utf8_text + start, pos - start);
        uint32_t id = mk_map_get(&model->word_index, key);
        if (id == UINT32_MAX) {
            ++unknown;
            continue;
        }
        ids[count++] = id;
    }

    if (count <= 1 && unknown == 0)
        return 0.0;

    ids[count++] = MK_END_TOKEN;

    /* Вероятность незнакомого слова: чем больше модель видела,
     * тем меньше шанс встретить что-то новое. */
    double oov_prob = 1.0 / ((double)model->tokens_seen + 1.0);
    if (oov_prob > 0.5)
        oov_prob = 0.5;
    if (oov_prob < 1e-9)
        oov_prob = 1e-9;

    double log_sum = 0.0;
    size_t steps = 0;

    for (size_t i = 1; i < count; ++i) {
        uint32_t from = ids[i - 1];
        uint32_t to = ids[i];
        double prob = 0.0;

        /* Биграммный контекст. */
        if (i >= 2) {
            uint64_t key = ((uint64_t)ids[i - 2] << 32) | (uint64_t)from;
            uint32_t idx = mk_map_get(&model->bigram_index, key);
            if (idx != UINT32_MAX) {
                const MkList *list = &model->bigrams[idx];
                uint32_t total = 0;
                for (size_t k = 0; k < list->count; ++k)
                    total += list->items[k].count;
                for (size_t k = 0; k < list->count; ++k) {
                    if (list->items[k].token == to && total > 0) {
                        prob = (double)list->items[k].count / (double)total;
                        break;
                    }
                }
            }
        }

        /* Бэкофф на униграммы. */
        if (prob == 0.0 && from < model->word_count) {
            const MkList *list = &model->unigram[from];
            uint32_t total = 0;
            for (size_t k = 0; k < list->count; ++k)
                total += list->items[k].count;
            for (size_t k = 0; k < list->count; ++k) {
                if (list->items[k].token == to && total > 0) {
                    prob = (double)list->items[k].count / (double)total;
                    break;
                }
            }
        }

        if (prob <= 0.0)
            prob = oov_prob;

        log_sum -= log(prob) / log(2.0);
        ++steps;
    }

    for (size_t i = 0; i < unknown; ++i) {
        log_sum -= log(oov_prob) / log(2.0);
        ++steps;
    }

    if (steps == 0)
        return 0.0;

    return pow(2.0, log_sum / (double)steps);
}

size_t markov_vocab_size(const Markov *model)
{
    if (model == NULL || model->word_count <= MK_FIRST_WORD)
        return 0;
    return model->word_count - MK_FIRST_WORD;
}

size_t markov_transition_count(const Markov *model)
{
    if (model == NULL)
        return 0;
    size_t total = 0;
    for (size_t i = 0; i < model->word_count; ++i)
        total += model->unigram[i].count;
    return total;
}

size_t markov_tokens_seen(const Markov *model)
{
    return model ? model->tokens_seen : 0;
}

unsigned markov_order(const Markov *model)
{
    return model ? model->order : 0;
}
