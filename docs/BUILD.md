# Сборка

## Требования

| Что | Минимум | Откуда |
|---|---|---|
| CMake | 3.20 | cmake.org / пакетный менеджер |
| Компилятор C | C11 | GCC, Clang, MSVC 2019+ |
| SDL2 | 2.0.14 | libsdl.org |
| SDL2_ttf | 2.0.15 | libsdl.org |
| Шрифт | DejaVu Sans Mono Bold | лежит в `assets/fonts/` |

SDL2_ttf 2.0.15 — минимальная версия из-за `TTF_SizeUTF8` с корректной
обработкой суррогатов. Более старые версии работают, но могут обрезать
эмодзи.

## Windows: MSYS2 (UCRT64) — рекомендуемый путь

```bash
pacman -S --needed \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-ninja \
    mingw-w64-ucrt-x86_64-SDL2 \
    mingw-w64-ucrt-x86_64-SDL2_ttf

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Запуск:

```bash
./build/hello.exe
```

Запускать надо **из корня репозитория**, иначе не найдётся
`assets/fonts/DejaVuSansMono-Bold.ttf`. Если запускаете из другого места —
укажите путь явно:

```bash
./build/hello.exe --font assets/fonts/DejaVuSansMono-Bold.ttf
```

### Почему MSYS2, а не MSVC

MSVC тоже работает, но:

- `find_package(SDL2 CONFIG)` требует, чтобы SDL2 был собран с CMake
  и содержал `SDL2Config.cmake`. Официальный SDL2-devel для MSVC его
  содержит, но пути надо прописать вручную через `CMAKE_PREFIX_PATH`.
- Иконка приложения требует `windres`, которого в MSVC нет — там `.rc`
  обрабатывается по-другому.

Для MSVC:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" `
    -DCMAKE_PREFIX_PATH="C:/SDL2;C:/SDL2_ttf"
cmake --build build --config Release
```

## Linux

### Debian / Ubuntu

```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
./build/hello
```

### Fedora

```bash
sudo dnf install gcc cmake SDL2-devel SDL2_ttf-devel
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

### Arch

```bash
sudo pacman -S base-devel cmake sdl2 sdl2_ttf
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

## macOS

```bash
brew install cmake sdl2 sdl2_ttf
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(sysctl -n hw.ncpu)"
./build/hello
```

Если SDL2 не находится, укажите префикс:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix sdl2);$(brew --prefix sdl2_ttf)"
```

## Тесты

```bash
ctest --test-dir build --output-on-failure
```

Или напрямую — вывод подробнее:

```bash
./build/hello_tests
```

```
Codename Hello — тесты
  test_utf8                          ok
  test_chat                          ok
  test_input                         ok
  test_config                        ok
  test_metrics                       ok
  test_markov                        ok

Проверок: 522, провалов: 0
```

Тесты не требуют SDL и дисплея — только стандартную библиотеку C11.
Поэтому они собираются и в CI, и на headless-сервере.

## Опции сборки

| Опция | По умолчанию | Что делает |
|---|---|---|
| `HELLO_BUILD_TESTS` | `ON` | собирать `hello_tests` |
| `HELLO_WERROR` | `OFF` | считать предупреждения ошибками |
| `CMAKE_BUILD_TYPE` | `Release` | `Debug`, `Release`, `RelWithDebInfo` |

Пример строгой сборки (как в CI):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
    -DHELLO_WERROR=ON -DHELLO_BUILD_TESTS=ON
cmake --build build
```

## Установка

```bash
cmake --install build --prefix /usr/local
```

Раскладка:

```
/usr/local/bin/hello
/usr/local/share/hello/fonts/DejaVuSansMono-Bold.ttf
/usr/local/share/hello/fonts/LICENSE
/usr/local/share/hello/fonts/AUTHORS
/usr/local/share/doc/hello/README.md
/usr/local/share/doc/hello/LICENSE
```

## Портативная сборка для Windows

Чтобы `hello.exe` работал без установленного MSYS2, нужно положить рядом
DLL:

```bash
mkdir -p dist
cp build/hello.exe dist/
cp /ucrt64/bin/SDL2.dll dist/
cp /ucrt64/bin/SDL2_ttf.dll dist/
cp /ucrt64/bin/libfreetype-6.dll dist/
cp /ucrt64/bin/libbz2-1.dll dist/
cp /ucrt64/bin/libbrotlidec.dll dist/
cp /ucrt64/bin/libbrotlicommon.dll dist/
cp /ucrt64/bin/libpng16-16.dll dist/
cp /ucrt64/bin/zlib1.dll dist/
mkdir -p dist/assets/fonts
cp assets/fonts/*.ttf dist/assets/fonts/
```

Проверить зависимости конкретного бинаря:

```bash
objdump -p build/hello.exe | grep "DLL Name"
```

Если хотите один файл без DLL — нужен статический SDL2, которого
в MSYS2 нет. Варианты: собрать SDL2 из исходников с
`-DSDL_STATIC=ON -DSDL_SHARED=OFF`, либо использовать `zig cc`
с линковкой статических библиотек.

## Генерация иконки

Иконка закоммичена (`assets/icon.png`, `assets/icon.ico`,
`assets/hello.rc`, `src/icon_rgba.h`). Перегенерировать:

```bash
pip install Pillow
python tools/gen_icon.py
```

Скрипт рисует мастер-изображение 1024×1024, из него делает PNG 512×512,
многоразмерный ICO (16–256), `.rc` для `windres` и C-массив 64×64 для
оконной иконки SDL.

## Скриншоты для документации

```bash
for theme in green amber ice paper; do
    ./build/hello.exe --screenshot "theme-$theme.bmp" --theme "$theme"
done
python - <<'PY'
from PIL import Image
import glob, os
for path in glob.glob("theme-*.bmp"):
    Image.open(path).convert("RGB").save(
        os.path.join("docs/screenshots", path.replace(".bmp", ".png")),
        optimize=True)
    os.remove(path)
PY
```

`--screenshot` создаёт скрытое окно, рисует демо-диалог и сохраняет BMP.
Полезно и для проверки, что рендеринг не сломался, без ручного запуска.

## Отладка

### Окно не появляется

```bash
SDL_VIDEODRIVER=dummy ./build/hello
```

Если с `dummy` запускается, а без него нет — проблема в видео-драйвере,
а не в коде.

### Шрифт не найден

```
Шрифт не найден. Положи DejaVuSansMono-Bold.ttf в assets/fonts/
или укажи путь в hello.cfg (ключ font).
```

Проверить, что файл на месте:

```bash
ls -l assets/fonts/
```

### Кириллица в консоли Windows

`main.c` вызывает `AttachConsole(ATTACH_PARENT_PROCESS)` и переоткрывает
`stdout`/`stderr`. Если вывод всё равно кракозябрами — смените кодовую
страницу консоли:

```cmd
chcp 65001
```

### Утечки памяти

На Linux:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./build/hello
```

На Windows с MSYS2:

```bash
pacman -S mingw-w64-ucrt-x86_64-gdb
gdb ./build/hello.exe
```

### Санитайзеры (GCC/Clang)

```bash
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer"
cmake --build build-asan
./build-asan/hello_tests
```

## CI

`.github/workflows/build.yml` собирает проект на Linux, Windows и macOS
и запускает тесты. Кэш — `ccache`, артефакты — бинарь и `docs/screenshots`.

Проверить локально то же, что в CI:

```bash
cmake -S . -B build -DHELLO_WERROR=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## Частые проблемы

| Симптом | Причина | Решение |
|---|---|---|
| `SDL2 not found` | нет `SDL2Config.cmake` | `-DCMAKE_PREFIX_PATH=/путь/к/SDL2` |
| `undefined reference to WinMain` | не подключён `SDL2main` | проверить, что `SDL2::SDL2main` в `target_link_libraries` |
| `undefined reference to TTF_*` | не подключён `SDL2_ttf` | `find_package(SDL2_ttf CONFIG REQUIRED)` |
| Кракозябры в окне | шрифт без кириллицы | использовать DejaVu Sans Mono |
| Пустое окно | шрифт не найден | запускать из корня или указать `--font` |
| `windres: command not found` | нет MSYS2 binutils | `pacman -S mingw-w64-ucrt-x86_64-binutils` |
