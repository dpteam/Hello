# 2013: обвязка Automake

`configure`, `Makefile.in`, `aclocal.m4`, `depcomp`, `install-sh`, `missing`,
`config.status`, `config.h`, `stamp-h1`, `.whitesource`.

**Состояние: нерабочее.**

## Почему

`configure.ac`:

```
AC_INIT([amhello], [1.0], [bug-automake@gnu.org])
```

`amhello` — имя из учебного примера Automake. Дальше блок проверки SDL
вклеен **после** `AC_OUTPUT` и без переводов строк:

```
AC_OUTPUT
dnl Check for SDL SDL_VERSION=1.2.0 AM_PATH_SDL($SDL_VERSION,
:, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])

) CFLAGS="$CFLAGS $SDL_CFLAGS" LIBS="$LIBS $SDL_LIBS"
```

`AM_PATH_SDL` вызывается уже после генерации выходных файлов — работать
это не может. Плюс `-Werror` в `AM_INIT_AUTOMAKE` гарантирует падение
на любом предупреждении.

## Что ещё не так

- Сгенерированные файлы (`configure`, `Makefile.in`, `aclocal.m4`)
  закоммичены вместе с исходниками.
- `config.status` содержит абсолютные пути машины 2013 года.
- `src/Makefile.am` объявляет только `hello.c`, хотя реально нужны
  `keysim.c`, `acsman.c`, `helleventcontrol.c`.

В 2026-м всё заменено на [`CMakeLists.txt`](../../CMakeLists.txt).
