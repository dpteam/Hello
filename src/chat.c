#include "chat.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "markov.h"
#include "text_metrics.h"
#include "utf8.h"

#define CHAT_MAX_PHRASES 4096
#define CHAT_MAX_INPUT_BYTES 512
#define CHAT_MAX_CODEPOINTS 256
#define CHAT_TOP_CANDIDATES 6
#define CHAT_MAX_QUERY_WORDS 16

/* Ниже этого порога считаем, что бот ничего не понял. */
#define CHAT_GENERIC_THRESHOLD 0.28f

/* Кандидаты слабее этой доли от лучшего не рассматриваются. */
#define CHAT_RELATIVE_FLOOR 0.72f

/* Вес BM25 в итоговой оценке (остальное — метрики похожести). */
#define CHAT_BM25_WEIGHT 0.30

/* BM25: насыщение частоты и нормализация длины. */
#define CHAT_BM25_K1 1.4
#define CHAT_BM25_B 0.72

/* Вероятность «сочинить» ответ вместо того, чтобы вспомнить,
 * когда лучшее совпадение не идеально. */
#define CHAT_IMPROV_STRENGTH 0.45

/* Штраф за повтор и его нижняя граница: даже самая заезженная
 * фраза сохраняет половину веса. */
#define CHAT_REPEAT_PENALTY 0.35
#define CHAT_REPEAT_FLOOR 0.5

static const char *const kSeedPhrases[] = {
    "Привет",
    "Не знаю",
    "Да",
    "Нет",
    "Ну-ну...",
    "Может быть",
    "Серьезно?",
    "Когда?",
    "Как?",
    "Где?",
    "O_o",
    "Ээээ...",
    "Да пошел ты!",
    "Да?",
    "Хватит",
    "???",
    "Не надо.",
    "Хорошо",
    "Зачем?",
    "Не понимаю...",
    "В смысле?",
    "Да ты что!",
    "Я думаю...",
};

#define SEED_COUNT (sizeof(kSeedPhrases) / sizeof(kSeedPhrases[0]))

struct Chat {
    char **items;
    size_t count;
    size_t cap;
    size_t learned;
    size_t seen;          /* всего предложено фраз (для резервуара) */

    unsigned *uses;       /* сколько раз фраза шла в ответ */
    size_t last_reply;

    Markov *markov;

    char generated[512];

    float vitality; /* 0..1, влияет на охоту сочинять */

    unsigned rng;
};

/* ---------------------------------------------------------------- */
/* Случайность                                                       */
/* ---------------------------------------------------------------- */

static uint32_t rng_next(Chat *chat)
{
    uint32_t x = chat->rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    if (x == 0)
        x = 0x9E3779B9u;
    chat->rng = x;
    return x;
}

static double rng_unit(Chat *chat)
{
    return (double)(rng_next(chat) >> 8) / 16777216.0;
}

/* ---------------------------------------------------------------- */
/* Слова                                                             */
/* ---------------------------------------------------------------- */

typedef struct {
    const char *ptr;
    size_t len;
} Word;

/* Режет нормализованный текст на слова (кодпоинты без шума). */
static size_t split_words(const char *text, Word *words, size_t max)
{
    if (text == NULL || words == NULL || max == 0)
        return 0;

    size_t len = strlen(text);
    size_t pos = 0;
    size_t n = 0;

    while (pos < len && n < max) {
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
            if (step <= 0 || utf8_is_noise(cp))
                break;
            pos += (size_t)step;
        }

        words[n].ptr = text + start;
        words[n].len = pos - start;
        ++n;
    }
    return n;
}

static bool word_equal_ci(Word a, Word b)
{
    size_t ia = 0, ib = 0;
    while (ia < a.len && ib < b.len) {
        uint32_t ca = 0, cb = 0;
        int na = utf8_decode(a.ptr + ia, a.len - ia, &ca);
        int nb = utf8_decode(b.ptr + ib, b.len - ib, &cb);
        if (na <= 0)
            na = 1;
        if (nb <= 0)
            nb = 1;
        if (utf8_casefold(ca) != utf8_casefold(cb))
            return false;
        ia += (size_t)na;
        ib += (size_t)nb;
    }
    return ia >= a.len && ib >= b.len;
}

static bool contains_run(const uint32_t *hay, size_t nh,
                         const uint32_t *needle, size_t nn)
{
    if (nn == 0 || nn > nh)
        return false;
    for (size_t i = 0; i + nn <= nh; ++i) {
        size_t j = 0;
        while (j < nn && hay[i + j] == needle[j])
            ++j;
        if (j == nn)
            return true;
    }
    return false;
}

/* ---------------------------------------------------------------- */
/* Память                                                            */
/* ---------------------------------------------------------------- */

static char *trim_copy(const char *text)
{
    if (text == NULL)
        return NULL;

    const char *start = text;
    while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n')
        ++start;

    const char *end = start + strlen(start);
    while (end > start) {
        char c = end[-1];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
        --end;
    }
    return utf8_dup_range(start, (size_t)(end - start));
}

static bool phrase_equal_ci(const char *a, const char *b)
{
    Word wa[CHAT_MAX_QUERY_WORDS];
    Word wb[CHAT_MAX_QUERY_WORDS];
    size_t na = split_words(a, wa, CHAT_MAX_QUERY_WORDS);
    size_t nb = split_words(b, wb, CHAT_MAX_QUERY_WORDS);

    if (na != nb)
        return false;
    for (size_t i = 0; i < na; ++i) {
        if (!word_equal_ci(wa[i], wb[i]))
            return false;
    }
    return true;
}

static void retrain_markov(Chat *chat)
{
    if (chat->markov == NULL)
        return;

    markov_reset(chat->markov);
    for (size_t i = 0; i < chat->count; ++i)
        markov_train(chat->markov, chat->items[i]);
}

Chat *chat_create(unsigned seed)
{
    Chat *chat = (Chat *)calloc(1, sizeof(*chat));
    if (chat == NULL)
        return NULL;

    chat->rng = seed ? seed : 1u;
    chat->last_reply = (size_t)-1;
    chat->vitality = 1.0f;
    chat->cap = SEED_COUNT + 32;
    chat->items = (char **)calloc(chat->cap, sizeof(char *));
    chat->uses = (unsigned *)calloc(chat->cap, sizeof(unsigned));
    if (chat->items == NULL || chat->uses == NULL) {
        free(chat->items);
        free(chat->uses);
        free(chat);
        return NULL;
    }

    chat->markov = markov_create(MARKOV_DEFAULT_ORDER, seed ? seed : 1u);
    if (chat->markov == NULL) {
        free(chat->items);
        free(chat->uses);
        free(chat);
        return NULL;
    }

    for (size_t i = 0; i < SEED_COUNT; ++i) {
        chat->items[i] = utf8_dup(kSeedPhrases[i]);
        if (chat->items[i] == NULL) {
            chat_destroy(chat);
            return NULL;
        }
        ++chat->count;
    }

    retrain_markov(chat);
    return chat;
}

void chat_destroy(Chat *chat)
{
    if (chat == NULL)
        return;
    for (size_t i = 0; i < chat->count; ++i)
        free(chat->items[i]);
    free(chat->items);
    free(chat->uses);
    markov_destroy(chat->markov);
    free(chat);
}

size_t chat_count(const Chat *chat)
{
    return chat ? chat->count : 0;
}

size_t chat_learned_count(const Chat *chat)
{
    return chat ? chat->learned : 0;
}

const char *chat_phrase(const Chat *chat, size_t index)
{
    if (chat == NULL || index >= chat->count)
        return NULL;
    return chat->items[index];
}

static bool grow_capacity(Chat *chat)
{
    size_t cap = chat->cap * 2;
    char **items = (char **)realloc(chat->items, cap * sizeof(char *));
    if (items == NULL)
        return false;
    chat->items = items;

    unsigned *uses = (unsigned *)realloc(chat->uses, cap * sizeof(unsigned));
    if (uses == NULL)
        return false;
    memset(uses + chat->cap, 0, (cap - chat->cap) * sizeof(unsigned));
    chat->uses = uses;

    chat->cap = cap;
    return true;
}

/* Резервуарная выборка: при переполнении каждая новая фраза с
 * вероятностью max/seen вытесняет случайную выученную. Так память
 * остаётся репрезентативной, а не «последние N». */
static bool reservoir_replace(Chat *chat, char *clean)
{
    size_t learned_slots = chat->count - SEED_COUNT;
    if (learned_slots == 0) {
        free(clean);
        return false;
    }

    size_t pick = (size_t)(rng_next(chat) % (uint32_t)learned_slots) + SEED_COUNT;
    free(chat->items[pick]);
    chat->items[pick] = clean;
    chat->uses[pick] = 0;
    return true;
}

bool chat_learn(Chat *chat, const char *text)
{
    if (chat == NULL || text == NULL)
        return false;

    char *clean = trim_copy(text);
    if (clean == NULL)
        return false;

    size_t bytes = strlen(clean);
    if (bytes == 0 || bytes > CHAT_MAX_INPUT_BYTES) {
        free(clean);
        return false;
    }

    for (size_t i = 0; i < chat->count; ++i) {
        if (phrase_equal_ci(chat->items[i], clean)) {
            free(clean);
            return false;
        }
    }

    ++chat->seen;

    if (chat->count >= CHAT_MAX_PHRASES) {
        bool replaced = reservoir_replace(chat, clean);
        if (replaced)
            retrain_markov(chat);
        return replaced;
    }

    if (chat->count == chat->cap && !grow_capacity(chat)) {
        free(clean);
        return false;
    }

    chat->items[chat->count] = clean;
    chat->uses[chat->count] = 0;
    ++chat->count;
    ++chat->learned;

    markov_train(chat->markov, clean);
    return true;
}

void chat_forget(Chat *chat)
{
    if (chat == NULL)
        return;

    for (size_t i = SEED_COUNT; i < chat->count; ++i) {
        free(chat->items[i]);
        chat->items[i] = NULL;
    }

    chat->count = SEED_COUNT;
    chat->learned = 0;
    chat->seen = 0;
    chat->last_reply = (size_t)-1;

    memset(chat->uses, 0, chat->cap * sizeof(unsigned));

    retrain_markov(chat);
}

/* ---------------------------------------------------------------- */
/* Похожесть                                                         */
/* ---------------------------------------------------------------- */

float chat_similarity(const char *a, const char *b)
{
    if (a == NULL || b == NULL)
        return 0.0f;

    uint32_t ca[CHAT_MAX_CODEPOINTS];
    uint32_t cb[CHAT_MAX_CODEPOINTS];
    size_t na = tm_normalize(a, ca, CHAT_MAX_CODEPOINTS);
    size_t nb = tm_normalize(b, cb, CHAT_MAX_CODEPOINTS);

    if (na == 0 || nb == 0)
        return 0.0f;

    float dice = tm_dice_bigrams(ca, na, cb, nb);
    float jw = tm_jaro_winkler(ca, na, cb, nb);

    size_t max_len = na > nb ? na : nb;
    size_t dist = tm_levenshtein(ca, na, cb, nb, max_len);
    float lev = max_len > 0 ? 1.0f - (float)dist / (float)max_len : 0.0f;
    if (lev < 0.0f)
        lev = 0.0f;

    /* Композит: Дайс ловит структуру, Джаро–Уинклер — начало слова,
     * Левенштейн — опечатки. Веса подобраны на глаз, но осмысленно. */
    float score = 0.50f * dice + 0.25f * jw + 0.25f * lev;

    /* Вложенность: «дела» внутри «как дела» — сильный сигнал. */
    if (na < nb && contains_run(cb, nb, ca, na))
        score += 0.16f;
    else if (nb < na && contains_run(ca, na, cb, nb))
        score += 0.16f;

    if (score > 1.0f)
        score = 1.0f;
    return score;
}

/* BM25 для одного документа. */
static double bm25_score(const Chat *chat, const Word *query, size_t nq,
                         const char *doc, double avg_len)
{
    Word doc_words[CHAT_MAX_QUERY_WORDS * 4];
    size_t nd = split_words(doc, doc_words, sizeof(doc_words) / sizeof(doc_words[0]));
    if (nd == 0)
        return 0.0;

    double score = 0.0;
    size_t docs = chat->count > 0 ? chat->count : 1;

    for (size_t i = 0; i < nq; ++i) {
        int freq = 0;
        for (size_t j = 0; j < nd; ++j) {
            if (word_equal_ci(query[i], doc_words[j]))
                ++freq;
        }
        if (freq == 0)
            continue;

        /* Document frequency: в скольких фразах встречается слово. */
        size_t df = 0;
        for (size_t k = 0; k < chat->count; ++k) {
            Word other[CHAT_MAX_QUERY_WORDS * 4];
            size_t no = split_words(chat->items[k], other,
                                    sizeof(other) / sizeof(other[0]));
            for (size_t j = 0; j < no; ++j) {
                if (word_equal_ci(query[i], other[j])) {
                    ++df;
                    break;
                }
            }
        }
        if (df == 0)
            df = 1;

        double idf = log(1.0 + ((double)docs - (double)df + 0.5) /
                                   ((double)df + 0.5));
        double tf = (double)freq;
        double dl = (double)nd;
        double denom = tf + CHAT_BM25_K1 *
                                (1.0 - CHAT_BM25_B +
                                 CHAT_BM25_B * (avg_len > 0.0 ? dl / avg_len : 1.0));
        if (denom <= 0.0)
            continue;

        score += idf * (tf * (CHAT_BM25_K1 + 1.0)) / denom;
    }

    return score;
}

static double average_length(const Chat *chat)
{
    if (chat->count == 0)
        return 0.0;

    size_t total = 0;
    for (size_t i = 0; i < chat->count; ++i) {
        Word words[CHAT_MAX_QUERY_WORDS * 4];
        total += split_words(chat->items[i], words,
                             sizeof(words) / sizeof(words[0]));
    }
    return (double)total / (double)chat->count;
}

/* ---------------------------------------------------------------- */
/* Ответ                                                             */
/* ---------------------------------------------------------------- */

const char *chat_random(Chat *chat)
{
    if (chat == NULL || chat->count == 0)
        return "…";
    return chat->items[rng_next(chat) % (uint32_t)chat->count];
}

void chat_set_vitality(Chat *chat, float vitality)
{
    if (chat == NULL)
        return;
    if (vitality < 0.0f)
        vitality = 0.0f;
    if (vitality > 1.0f)
        vitality = 1.0f;
    chat->vitality = vitality;
}

const char *chat_generate(Chat *chat, const char *prompt, double temperature)
{
    if (chat == NULL)
        return "…";

    if (markov_generate(chat->markov, prompt, temperature, chat->generated,
                        sizeof(chat->generated))) {
        return chat->generated;
    }
    return chat_random(chat);
}

static const char *generic_reply(Chat *chat)
{
    size_t pool = SEED_COUNT < chat->count ? SEED_COUNT : chat->count;
    size_t chosen = (size_t)-1;

    for (int attempt = 0; attempt < 8; ++attempt) {
        size_t pick = (size_t)(rng_next(chat) % (uint32_t)pool);
        if (pick != chat->last_reply || pool == 1) {
            chosen = pick;
            break;
        }
    }
    if (chosen == (size_t)-1)
        chosen = 0;

    chat->last_reply = chosen;
    if (chosen < chat->cap)
        ++chat->uses[chosen];
    return chat->items[chosen];
}

const char *chat_reply(Chat *chat, const char *input)
{
    if (chat == NULL || chat->count == 0)
        return "…";

    size_t best_idx[CHAT_TOP_CANDIDATES];
    double best_rank[CHAT_TOP_CANDIDATES];
    size_t best_n = 0;
    double top_base = 0.0;
    double top_rank = 0.0;

    Word query[CHAT_MAX_QUERY_WORDS];
    size_t nq = split_words(input, query, CHAT_MAX_QUERY_WORDS);
    double avg_len = average_length(chat);

    /* Уставший бот хуже узнаёт знакомое: порог «не понял» растёт. */
    double threshold =
        (double)CHAT_GENERIC_THRESHOLD * (1.25 - 0.25 * (double)chat->vitality);

    for (size_t i = 0; i < chat->count; ++i) {
        float sim = chat_similarity(input, chat->items[i]);
        if (sim <= 0.0f)
            continue;

        /* BM25 насыщается: переводим в 0..1 через x/(x+3). */
        double raw = bm25_score(chat, query, nq, chat->items[i], avg_len);
        double norm = raw / (raw + 3.0);

        double base = (1.0 - CHAT_BM25_WEIGHT) * (double)sim +
                      CHAT_BM25_WEIGHT * norm;

        /* Урновая модель «не повторяться»: использованные фразы теряют
         * вес, но не более чем вдвое — точное совпадение должно
         * оставаться точным совпадением. */
        double penalty = 1.0 / (1.0 + CHAT_REPEAT_PENALTY * (double)chat->uses[i]);
        if (penalty < CHAT_REPEAT_FLOOR)
            penalty = CHAT_REPEAT_FLOOR;

        double rank = base * penalty;

        if (base > top_base)
            top_base = base;

        if (best_n == CHAT_TOP_CANDIDATES &&
            rank <= best_rank[CHAT_TOP_CANDIDATES - 1])
            continue;

        size_t pos;
        if (best_n < CHAT_TOP_CANDIDATES) {
            pos = best_n;
            ++best_n;
        } else {
            pos = CHAT_TOP_CANDIDATES - 1;
        }

        while (pos > 0 && best_rank[pos - 1] < rank) {
            best_rank[pos] = best_rank[pos - 1];
            best_idx[pos] = best_idx[pos - 1];
            --pos;
        }
        best_rank[pos] = rank;
        best_idx[pos] = i;

        if (rank > top_rank)
            top_rank = rank;
    }

    if (best_n == 0 || top_base < threshold) {
        /* Не понял — либо сочиняем, либо отделываемся вежливостью.
         * Чем больше выучено, тем охотнее сочиняем. Уставший бот
         * сочиняет неохотно. */
        double improv = CHAT_IMPROV_STRENGTH *
                        ((double)chat->learned /
                         ((double)chat->learned + 12.0)) *
                        (0.3 + 0.7 * (double)chat->vitality);
        if (chat->learned >= 3 && rng_unit(chat) < improv) {
            const char *line = chat_generate(chat, input, 1.05);
            if (line != NULL && line[0] != '\0') {
                chat->last_reply = (size_t)-1;
                return line;
            }
        }
        return generic_reply(chat);
    }

    /* Оставляем только тех, кто близок к лучшему. */
    double floor_score = top_rank * (double)CHAT_RELATIVE_FLOOR;
    size_t keep = 0;
    for (size_t i = 0; i < best_n; ++i) {
        if (best_rank[i] >= floor_score)
            ++keep;
    }
    if (keep == 0)
        keep = 1;

    double weights[CHAT_TOP_CANDIDATES];
    double probs[CHAT_TOP_CANDIDATES];

    for (size_t i = 0; i < keep; ++i) {
        double base = best_rank[i] > 0.0 ? best_rank[i] : 0.001;
        weights[i] = base * base * base;
        if (best_idx[i] == chat->last_reply && keep > 1)
            weights[i] *= 0.1;
    }

    tm_power_dist(weights, keep, 1.0, probs);

    double roll = rng_unit(chat);
    size_t chosen = best_idx[0];
    for (size_t i = 0; i < keep; ++i) {
        roll -= probs[i];
        if (roll <= 0.0) {
            chosen = best_idx[i];
            break;
        }
    }

    chat->last_reply = chosen;
    if (chosen < chat->cap)
        ++chat->uses[chosen];
    return chat->items[chosen];
}

size_t chat_markov_states(const Chat *chat)
{
    if (chat == NULL)
        return 0;
    return markov_transition_count(chat->markov);
}

size_t chat_markov_vocab(const Chat *chat)
{
    if (chat == NULL)
        return 0;
    return markov_vocab_size(chat->markov);
}

double chat_perplexity(const Chat *chat, const char *text)
{
    if (chat == NULL)
        return 0.0;
    return markov_perplexity(chat->markov, text);
}

/* ---------------------------------------------------------------- */
/* Файлы                                                             */
/* ---------------------------------------------------------------- */

static bool write_escaped(FILE *out, const char *text)
{
    for (const char *p = text; *p; ++p) {
        switch (*p) {
        case '\\':
            fputs("\\\\", out);
            break;
        case '\n':
            fputs("\\n", out);
            break;
        case '\r':
            fputs("\\r", out);
            break;
        default:
            fputc((unsigned char)*p, out);
            break;
        }
    }
    return !ferror(out);
}

bool chat_save(const Chat *chat, const char *path)
{
    if (chat == NULL || path == NULL)
        return false;

    FILE *out = fopen(path, "wb");
    if (out == NULL)
        return false;

    fputs("# Codename Hello — память бота (v2)\n", out);
    fputs("# Одна фраза на строку. Строки с # игнорируются.\n", out);

    bool ok = true;
    for (size_t i = SEED_COUNT; i < chat->count; ++i) {
        if (!write_escaped(out, chat->items[i]) || fputc('\n', out) == EOF) {
            ok = false;
            break;
        }
    }

    if (fclose(out) != 0)
        ok = false;
    return ok;
}

static void unescape_in_place(char *text)
{
    char *src = text;
    char *dst = text;

    while (*src) {
        if (*src == '\\' && src[1] != '\0') {
            switch (src[1]) {
            case 'n':
                *dst++ = '\n';
                src += 2;
                continue;
            case 'r':
                *dst++ = '\r';
                src += 2;
                continue;
            case '\\':
                *dst++ = '\\';
                src += 2;
                continue;
            default:
                break;
            }
        }
        *dst++ = *src++;
    }
    *dst = '\0';
}

size_t chat_load(Chat *chat, const char *path)
{
    if (chat == NULL || path == NULL)
        return 0;

    FILE *in = fopen(path, "rb");
    if (in == NULL)
        return 0;

    char line[1024];
    size_t loaded = 0;

    while (fgets(line, (int)sizeof(line), in) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        if (len == 0 || line[0] == '#')
            continue;

        unescape_in_place(line);
        if (chat_learn(chat, line))
            ++loaded;
    }

    fclose(in);
    return loaded;
}
