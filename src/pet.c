#include "pet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* За час без общения настроение падает примерно на 12 пунктов,
 * энергия — на 8. Это медленно: бот не должен «умирать» за вечер. */
#define PET_MOOD_DECAY_PER_MIN 0.2f
#define PET_ENERGY_DECAY_PER_MIN 0.13f

/* Ограничение на один шаг обновления: если приложение свернули
 * на сутки, не вычитаем всё разом — иначе бот мгновенно умрёт. */
#define PET_MAX_STEP_MIN 180.0f

#define PET_FEED_MOOD 18.0f
#define PET_FEED_ENERGY 25.0f
#define PET_MESSAGE_MOOD 1.5f
#define PET_MESSAGE_ENERGY 0.4f

static float clamp01_100(float v)
{
    if (v < 0.0f)
        return 0.0f;
    if (v > 100.0f)
        return 100.0f;
    return v;
}

void pet_init(Pet *pet)
{
    if (pet == NULL)
        return;

    memset(pet, 0, sizeof(*pet));
    pet->mood = 72.0f;
    pet->energy = 80.0f;
}

void pet_update(Pet *pet, uint32_t now_ms)
{
    if (pet == NULL)
        return;

    if (pet->last_update_ms == 0) {
        pet->last_update_ms = now_ms;
        return;
    }

    uint32_t delta = now_ms - pet->last_update_ms;
    if (delta == 0)
        return;

    pet->last_update_ms = now_ms;

    float minutes = (float)delta / 60000.0f;
    if (minutes > PET_MAX_STEP_MIN)
        minutes = PET_MAX_STEP_MIN;

    pet->mood = clamp01_100(pet->mood - minutes * PET_MOOD_DECAY_PER_MIN);
    pet->energy =
        clamp01_100(pet->energy - minutes * PET_ENERGY_DECAY_PER_MIN);

    /* Голодный бот теряет силы быстрее. */
    if (pet->mood < 25.0f)
        pet->energy = clamp01_100(pet->energy - minutes * 0.1f);
}

void pet_feed(Pet *pet, uint32_t now_ms)
{
    if (pet == NULL)
        return;

    pet->mood = clamp01_100(pet->mood + PET_FEED_MOOD);
    pet->energy = clamp01_100(pet->energy + PET_FEED_ENERGY);
    pet->last_feed_ms = now_ms;
}

void pet_on_message(Pet *pet, uint32_t now_ms)
{
    if (pet == NULL)
        return;

    (void)now_ms;
    ++pet->messages;
    pet->mood = clamp01_100(pet->mood + PET_MESSAGE_MOOD);
    pet->energy = clamp01_100(pet->energy + PET_MESSAGE_ENERGY);
}

PetMood pet_mood(const Pet *pet)
{
    if (pet == NULL)
        return PET_SLEEPING;

    /* Берём минимум из настроения и энергии: измотанный бот не может
     * быть счастливым, даже если формально всё хорошо. */
    float value = pet->mood < pet->energy ? pet->mood : pet->energy;

    if (value >= 80.0f)
        return PET_HAPPY;
    if (value >= 55.0f)
        return PET_CONTENT;
    if (value >= 30.0f)
        return PET_BORED;
    if (value >= 12.0f)
        return PET_HUNGRY;
    return PET_SLEEPING;
}

const char *pet_mood_name(PetMood mood)
{
    switch (mood) {
    case PET_HAPPY:
        return "рад";
    case PET_CONTENT:
        return "норм";
    case PET_BORED:
        return "скучно";
    case PET_HUNGRY:
        return "голоден";
    case PET_SLEEPING:
    default:
        return "спит";
    }
}

float pet_vitality(const Pet *pet)
{
    if (pet == NULL)
        return 0.0f;

    float value = pet->mood < pet->energy ? pet->mood : pet->energy;
    value /= 100.0f;
    if (value < 0.0f)
        value = 0.0f;
    if (value > 1.0f)
        value = 1.0f;
    return value;
}

bool pet_save(const Pet *pet, const char *path)
{
    if (pet == NULL || path == NULL)
        return false;

    FILE *out = fopen(path, "wb");
    if (out == NULL)
        return false;

    fprintf(out, "# Codename Hello — состояние бота\n");
    fprintf(out, "mood = %.2f\n", (double)pet->mood);
    fprintf(out, "energy = %.2f\n", (double)pet->energy);
    fprintf(out, "messages = %u\n", pet->messages);

    bool ok = !ferror(out);
    if (fclose(out) != 0)
        ok = false;
    return ok;
}

bool pet_load(Pet *pet, const char *path)
{
    if (pet == NULL || path == NULL)
        return false;

    FILE *in = fopen(path, "rb");
    if (in == NULL)
        return false;

    char line[256];
    bool any = false;

    while (fgets(line, (int)sizeof(line), in) != NULL) {
        char *text = line;
        while (*text == ' ' || *text == '\t')
            ++text;
        if (*text == '#' || *text == '\0')
            continue;

        char *eq = strchr(text, '=');
        if (eq == NULL)
            continue;
        *eq = '\0';

        char *key = text;
        char *value = eq + 1;

        /* Обрезаем ключ и значение. */
        size_t klen = strlen(key);
        while (klen > 0 && (key[klen - 1] == ' ' || key[klen - 1] == '\t'))
            key[--klen] = '\0';
        while (*value == ' ' || *value == '\t')
            ++value;

        if (strcmp(key, "mood") == 0) {
            pet->mood = clamp01_100((float)atof(value));
            any = true;
        } else if (strcmp(key, "energy") == 0) {
            pet->energy = clamp01_100((float)atof(value));
            any = true;
        } else if (strcmp(key, "messages") == 0) {
            long v = strtol(value, NULL, 10);
            if (v > 0)
                pet->messages = (uint32_t)v;
            any = true;
        }
    }

    fclose(in);
    pet->loaded_from_disk = any;
    return any;
}
