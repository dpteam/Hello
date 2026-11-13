# 2012-12: дерево HELLO_TEST18122012

Среда сборки проекта, снятая 11.03.2015 тремя архивами.

| Архив | Размер | Содержимое |
|---|---|---|
| `Full.Pre-Old.Source.7z` | 29 МБ | полное дерево: **9289 файлов, 838 папок**, 162 МБ распакованных |
| `Hello-0.1.zip` | 1.1 МБ | упаковка состояния на 28.02.2014, 62 файла |
| `_DEV_old.7z` | 19 КБ | ранние консольные версии → см. [`../2012-07-console-bot/`](../2012-07-console-bot/) |

## Что внутри полного дерева

- `src/` — `hello.c`, `keysim.c`, `acsman.c`, `resize.c` (единственная
  копия `keysim.c` в исходном виде)
- `Library/Frameworks/SDL-1.2.15/` — вендоренный SDL целиком
- `Library/Frameworks/SDL_mingw32.Framework/` — сборка SDL под MinGW
- `Library/Frameworks/Irrlicht-1.8/` — 3D-движок, не пригодился
- `Library/Compilers/ack-6.0pre4/` — Amsterdam Compiler Kit
- `oglconsole/`, `DejaVuSansMono-Bold/`, `autom4te.cache/`

162 МБ на проект, где реально используется 300 КБ. Классическое
«скачал всё, что может понадобиться».

## Деталь про переносы строк

`Hello-0.1.zip` содержит `hello.c` на 23 785 байт, версия в git —
24 434 байта. Разница ровно 649 байт = один байт на строку:
в zip **LF**, в git **CRLF**. Содержимое идентично (проверено построчным
диффом через `tools/re/diff_slices.py`).

Подробности: [`../../docs/ARCHIVE.md`](../../docs/ARCHIVE.md#2012-12-hello-test-tree--дерево-hello_test18122012)
