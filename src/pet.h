/*
 * pet.h — состояние «тамагочи»: настроение и энергия бота.
 *
 * Идея из макета 2015 года (tamag.png): у собеседника есть состояние,
 * и оно влияет на то, как он отвечает. Редко пишешь — грустит и
 * отвечает суше. Много общаешься — оживает и охотнее сочиняет.
 */
#ifndef HELLO_PET_H
#define HELLO_PET_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PET_HAPPY,    /* 80..100 */
    PET_CONTENT,  /* 55..80  */
    PET_BORED,    /* 30..55  */
    PET_HUNGRY,   /* 12..30  */
    PET_SLEEPING  /* 0..12   */
} PetMood;

typedef struct {
    float mood;   /* 0..100 */
    float energy; /* 0..100 */

    uint32_t last_update_ms;
    uint32_t last_feed_ms;
    uint32_t messages;

    bool loaded_from_disk;
} Pet;

/* Начальное состояние: бодрый, но не при смерти. */
void pet_init(Pet *pet);

/* Продвинуть время. Вызывается каждый кадр.
 * Голод растёт, энергия падает; скорость зависит от того,
 * сколько прошло с прошлого обновления. */
void pet_update(Pet *pet, uint32_t now_ms);

/* Покормить: +энергия, +настроение. */
void pet_feed(Pet *pet, uint32_t now_ms);

/* Пришло сообщение от пользователя: бот радуется. */
void pet_on_message(Pet *pet, uint32_t now_ms);

PetMood pet_mood(const Pet *pet);
const char *pet_mood_name(PetMood mood);

/* 0..1 — насколько бот «в форме». Используется как множитель
 * к порогу импровизации: голодный бот реже сочиняет. */
float pet_vitality(const Pet *pet);

/* Формат: key = value, по строке на поле. */
bool pet_save(const Pet *pet, const char *path);
bool pet_load(Pet *pet, const char *path);

#endif /* HELLO_PET_H */
