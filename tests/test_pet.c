#include "test.h"

#include <stdio.h>
#include <string.h>

#include "pet.h"

static void test_init(void)
{
    Pet pet;
    pet_init(&pet);

    CHECK(pet.mood > 50.0f);
    CHECK(pet.energy > 50.0f);
    CHECK_INT_EQ(pet.messages, 0);
    CHECK_INT_EQ(pet_mood(&pet), PET_CONTENT);
}

static void test_decay(void)
{
    Pet pet;
    pet_init(&pet);

    float mood_before = pet.mood;
    pet.last_update_ms = 1000;

    /* Час без общения. */
    pet_update(&pet, 1000 + 60 * 60 * 1000);

    CHECK(pet.mood < mood_before);
    CHECK(pet.mood > 0.0f); /* за час не умирает */
    CHECK(pet.energy < 80.0f);
}

static void test_decay_capped(void)
{
    Pet pet;
    pet_init(&pet);
    pet.last_update_ms = 1000;

    /* Сутки простоя — шаг ограничен, но настроение всё равно просядет. */
    pet_update(&pet, 1000 + 24 * 60 * 60 * 1000);
    CHECK(pet.mood >= 0.0f);
    CHECK(pet.energy >= 0.0f);
    CHECK(pet.mood < 50.0f);
}

static void test_feed(void)
{
    Pet pet;
    pet_init(&pet);
    pet.mood = 30.0f;
    pet.energy = 20.0f;

    pet_feed(&pet, 5000);

    CHECK(pet.mood > 30.0f);
    CHECK(pet.energy > 20.0f);
    CHECK_INT_EQ(pet.last_feed_ms, 5000);

    /* Не переваливает за 100. */
    for (int i = 0; i < 20; ++i)
        pet_feed(&pet, 5000);
    CHECK(pet.mood <= 100.0f);
    CHECK(pet.energy <= 100.0f);
}

static void test_mood_thresholds(void)
{
    Pet pet;
    pet_init(&pet);

    pet.mood = 90.0f;
    pet.energy = 90.0f;
    CHECK_INT_EQ(pet_mood(&pet), PET_HAPPY);

    pet.mood = 60.0f;
    pet.energy = 60.0f;
    CHECK_INT_EQ(pet_mood(&pet), PET_CONTENT);

    pet.mood = 40.0f;
    pet.energy = 40.0f;
    CHECK_INT_EQ(pet_mood(&pet), PET_BORED);

    pet.mood = 20.0f;
    pet.energy = 20.0f;
    CHECK_INT_EQ(pet_mood(&pet), PET_HUNGRY);

    pet.mood = 5.0f;
    pet.energy = 5.0f;
    CHECK_INT_EQ(pet_mood(&pet), PET_SLEEPING);

    /* Берётся минимум из двух: измотанный не может быть счастливым. */
    pet.mood = 95.0f;
    pet.energy = 10.0f;
    CHECK_INT_EQ(pet_mood(&pet), PET_SLEEPING);
}

static void test_mood_names(void)
{
    for (int m = PET_HAPPY; m <= PET_SLEEPING; ++m) {
        const char *name = pet_mood_name((PetMood)m);
        CHECK(name != NULL);
        CHECK(name[0] != '\0');
    }
}

static void test_vitality(void)
{
    Pet pet;
    pet_init(&pet);

    pet.mood = 100.0f;
    pet.energy = 100.0f;
    CHECK_NEAR(pet_vitality(&pet), 1.0f, 0.001);

    pet.mood = 0.0f;
    pet.energy = 0.0f;
    CHECK_NEAR(pet_vitality(&pet), 0.0f, 0.001);

    pet.mood = 50.0f;
    pet.energy = 90.0f;
    CHECK_NEAR(pet_vitality(&pet), 0.5f, 0.001);

    CHECK_NEAR(pet_vitality(NULL), 0.0f, 0.001);
}

static void test_message(void)
{
    Pet pet;
    pet_init(&pet);
    pet.mood = 50.0f;
    pet.energy = 50.0f;

    pet_on_message(&pet, 1000);
    CHECK_INT_EQ(pet.messages, 1);
    CHECK(pet.mood > 50.0f);
}

static void test_save_load(void)
{
    const char *path = "test_pet.tmp";

    Pet pet;
    pet_init(&pet);
    pet.mood = 42.5f;
    pet.energy = 66.25f;
    pet.messages = 17;

    CHECK(pet_save(&pet, path));

    Pet loaded;
    pet_init(&loaded);
    CHECK(pet_load(&loaded, path));

    CHECK_NEAR(loaded.mood, 42.5, 0.01);
    CHECK_NEAR(loaded.energy, 66.25, 0.01);
    CHECK_INT_EQ(loaded.messages, 17);
    CHECK(loaded.loaded_from_disk);

    remove(path);

    /* Отсутствующий файл — false, состояние не портится. */
    Pet fresh;
    pet_init(&fresh);
    CHECK(!pet_load(&fresh, "нет_такого_файла_12345"));
    CHECK(!fresh.loaded_from_disk);

    CHECK(!pet_save(NULL, path));
    CHECK(!pet_save(&pet, NULL));
}

void test_pet(void)
{
    test_init();
    test_decay();
    test_decay_capped();
    test_feed();
    test_mood_thresholds();
    test_mood_names();
    test_vitality();
    test_message();
    test_save_load();
}
