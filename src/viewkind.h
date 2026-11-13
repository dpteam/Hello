/*
 * viewkind.h — тип строки интерфейса, без зависимостей от SDL.
 *
 * Вынесено отдельно, чтобы экспорт диалога и тесты могли работать
 * с типами строк, не подтягивая SDL.
 */
#ifndef HELLO_VIEWKIND_H
#define HELLO_VIEWKIND_H

typedef enum {
    VIEW_SYSTEM,
    VIEW_USER,
    VIEW_BOT,
    VIEW_WARNING,
    VIEW_ERROR,
    VIEW_HINT
} ViewKind;

/* Машинное имя типа: "system", "user", "bot"… */
const char *view_kind_name(ViewKind kind);

/* Метка автора для диалога: "Вы", "Hello".
 * Для системных строк возвращает NULL — у них нет автора. */
const char *view_kind_author(ViewKind kind);

/* Является ли строка репликой участника (а не системным сообщением). */
int view_kind_is_dialogue(ViewKind kind);

#endif /* HELLO_VIEWKIND_H */
