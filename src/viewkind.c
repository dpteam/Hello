#include "viewkind.h"

const char *view_kind_name(ViewKind kind)
{
    switch (kind) {
    case VIEW_USER:
        return "user";
    case VIEW_BOT:
        return "bot";
    case VIEW_WARNING:
        return "warning";
    case VIEW_ERROR:
        return "error";
    case VIEW_HINT:
        return "hint";
    case VIEW_SYSTEM:
    default:
        return "system";
    }
}

const char *view_kind_author(ViewKind kind)
{
    switch (kind) {
    case VIEW_USER:
        return "Вы";
    case VIEW_BOT:
        return "Hello";
    default:
        return 0;
    }
}

int view_kind_is_dialogue(ViewKind kind)
{
    return (kind == VIEW_USER || kind == VIEW_BOT) ? 1 : 0;
}
