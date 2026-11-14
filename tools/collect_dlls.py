#!/usr/bin/env python3
"""Собирает портативный дистрибутив Windows: бинарник + все DLL.

Имена DLL не хардкодятся — зависимости вычисляются рекурсивно по
таблицам импорта PE. Так набор не разъедется, если SDL2 или FreeType
в следующей версии потянут что-то новое.

    python tools/collect_dlls.py build/hello.exe dist
    python tools/collect_dlls.py --check dist     # только проверить

Стандартная библиотека, без зависимостей. Работает и в CI, и локально.
"""
from __future__ import annotations

import os
import re
import shutil
import struct
import sys
from pathlib import Path

# DLL, которые всегда есть в Windows — копировать не нужно.
# Список намеренно по префиксам: api-ms-win-* и ext-ms-win-* покрывают
# сотни API-set'ов, перечислять их поимённо бессмысленно.
SYSTEM_PREFIXES = (
    "api-ms-win", "ext-ms-win",
    "kernel32", "user32", "gdi32", "gdiplus", "shell32", "shlwapi",
    "ole32", "oleaut32", "advapi32", "comdlg32", "comctl32", "winspool",
    "uuid", "ws2_32", "setupapi", "version", "imm32", "winmm",
    "bcrypt", "crypt32", "secur32", "rpcrt4", "msvcrt", "ucrtbase",
    "d3d9", "d3d11", "d3d12", "dxgi", "opengl32", "glu32", "msimg32",
    "dwmapi", "hid", "dinput8", "xinput", "ntdll", "powrprof", "userenv",
    "psapi", "iphlpapi", "dbghelp", "mswsock", "normaliz", "netapi32",
    "wldap32", "dwrite", "usp10", "uxtheme", "shcore", "winhttp",
    "wininet", "wintrust", "cfgmgr32", "dnsapi", "mpr", "ncrypt",
)


def pe_imports(path: Path) -> list[str]:
    """Имена DLL из таблицы импорта. Пустой список, если не PE."""
    try:
        data = path.read_bytes()
    except OSError:
        return []

    if data[:2] != b"MZ":
        return []

    try:
        pe = struct.unpack_from("<I", data, 0x3C)[0]
        if data[pe:pe + 4] != b"PE\0\0":
            return []

        nsec = struct.unpack_from("<H", data, pe + 6)[0]
        opt_size = struct.unpack_from("<H", data, pe + 20)[0]
        opt = pe + 24
        table = opt + opt_size

        # PE32 и PE32+ отличаются смещением таблицы данных:
        # 96 против 112 байт от начала опционального заголовка.
        magic = struct.unpack_from("<H", data, opt)[0]
        dd_base = 112 if magic == 0x20B else 96

        # Индекс 1 в таблице данных — импорт.
        dir_rva = struct.unpack_from("<I", data, opt + dd_base + 8)[0]
        if not dir_rva:
            return []

        def rva_to_off(rva: int) -> int | None:
            for i in range(nsec):
                off = table + i * 40
                vsize, vaddr, rawsize, rawptr = struct.unpack_from(
                    "<IIII", data, off + 8)
                if vaddr <= rva < vaddr + max(vsize, rawsize):
                    return rawptr + (rva - vaddr)
            return None

        off = rva_to_off(dir_rva)
        if off is None:
            return []

        names = []
        while True:
            entry = data[off:off + 20]
            if len(entry) < 20 or entry == b"\0" * 20:
                break
            name_rva = struct.unpack_from("<I", entry, 12)[0]
            name_off = rva_to_off(name_rva)
            if name_off is not None:
                end = data.index(b"\0", name_off)
                names.append(data[name_off:end].decode("latin-1"))
            off += 20
        return names
    except (struct.error, ValueError, IndexError):
        return []


def is_system_dll(name: str) -> bool:
    low = name.lower()
    return any(low.startswith(p) for p in SYSTEM_PREFIXES)


# Имя DLL внутри бинарника: строка вида "SDL3.dll" в таблице строк.
DLL_NAME_RE = re.compile(rb"[A-Za-z0-9_.+-]{1,60}\.dll", re.IGNORECASE)

# Библиотеки-прослойки: сами по себе почти пустые, всю работу отдают
# другой библиотеке, загружая её через LoadLibrary. В таблице импорта
# такой зависимости нет, поэтому ldd её не показывает, и проверка
# только по импортам пропускает нужный файл.
#
# Живой случай: MSYS2 отдаёт SDL2 как sdl2-compat. Он транслирует
# вызовы SDL2 в SDL3 и без SDL3.dll не работает вовсе — приложение
# падает с «Failed loading SDL3 library».
SHIMS: tuple[tuple[bytes, str], ...] = (
    (b"sdl2-compat", "SDL3.dll"),
)


def shim_requirements(path: Path) -> list[str]:
    """Библиотеки, которые нужны этому файлу как прослойке."""
    try:
        data = path.read_bytes()
    except OSError:
        return []

    needed = []
    for marker, dll in SHIMS:
        if marker in data and dll not in needed:
            needed.append(dll)
    return needed


def search_dirs(extra: list[Path]) -> list[Path]:
    """Каталоги, откуда берём библиотеки для копирования.

    System32 здесь намеренно нет: системные DLL копировать не нужно,
    они есть в любой Windows. Если добавить, сканер начнёт тащить
    сотни системных библиотек, которые упоминаются в строках.
    """
    dirs = list(extra)
    for var in ("MSYSTEM_PREFIX", "MINGW_PREFIX"):
        prefix = os.environ.get(var)
        if prefix:
            dirs.append(Path(prefix) / "bin")

    return [d for d in dirs if d.is_dir()]


def collect(exe: Path, out_dir: Path, verbose: bool = True) -> int:
    """Копирует exe и все недостающие DLL. Возвращает число проблем."""
    out_dir.mkdir(parents=True, exist_ok=True)

    target_exe = out_dir / exe.name
    if exe.resolve() != target_exe.resolve():
        shutil.copy2(exe, target_exe)

    roots = [p for p in out_dir.iterdir() if p.suffix.lower() == ".exe"]
    dirs = search_dirs([exe.parent])

    seen: set[str] = set()
    queue = list(roots)
    copied: list[str] = []
    problems = 0

    while queue:
        current = queue.pop()

        # 1. Статические зависимости — из таблицы импорта.
        candidates = [(name, "импорт") for name in pe_imports(current)]

        # 2. Прослойки: их библиотеки грузятся через LoadLibrary,
        #    в таблице импорта их нет.
        candidates += [(name, "прослойка") for name in shim_requirements(current)]

        for dll, origin in candidates:
            key = dll.lower()
            if key in seen:
                continue
            seen.add(key)

            if is_system_dll(dll):
                continue

            local = out_dir / dll
            if local.exists():
                queue.append(local)
                continue

            source = None
            for folder in dirs:
                candidate = folder / dll
                if candidate.exists():
                    source = candidate
                    break

            if source is None:
                print(f"ОШИБКА: не найдена {dll} (нужна для {current.name}, "
                      f"{origin})")
                problems += 1
                continue

            shutil.copy2(source, local)
            copied.append(dll)
            queue.append(local)
            if verbose:
                mark = "" if origin == "импорт" else f"  ({origin})"
                print(f"  + {dll}{mark}")

    if verbose:
        print(f"\nскопировано DLL: {len(copied)}")
    return problems


def check(out_dir: Path) -> int:
    """Проверяет, что все нужные DLL лежат рядом с бинарниками.

    Смотрит таблицу импорта и требования прослоек. Именно вторая
    проверка ловит sdl2-compat без SDL3.dll — случай, который
    не виден ни ldd, ни разбору импортов.
    """
    problems = 0
    binaries = sorted(out_dir.glob("*.exe")) + sorted(out_dir.glob("*.dll"))

    for binary in binaries:
        for dll in pe_imports(binary):
            if is_system_dll(dll):
                continue
            if not (out_dir / dll).exists():
                print(f"ОТСУТСТВУЕТ: {dll} (импортируется из {binary.name})")
                problems += 1

        for dll in shim_requirements(binary):
            if is_system_dll(dll):
                continue
            if not (out_dir / dll).exists():
                print(f"ОТСУТСТВУЕТ: {dll} "
                      f"(нужна прослойке {binary.name} в рантайме)")
                problems += 1

    return problems


def _make_output_utf8() -> None:
    """Консоль Windows может быть в cp1252/cp866 — русский текст в ней
    падает с UnicodeEncodeError. Переключаем вывод на UTF-8 с заменой
    непечатаемых символов: сообщения диагностики не должны ронять утилиту."""
    for stream in (sys.stdout, sys.stderr):
        encoding = getattr(stream, "encoding", None) or ""
        if encoding.lower().replace("-", "") == "utf8":
            continue
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except (AttributeError, ValueError, OSError):
            pass


def main() -> int:
    _make_output_utf8()

    args = [a for a in sys.argv[1:] if not a.startswith("--")]

    if "--check" in sys.argv:
        if not args:
            raise SystemExit(__doc__)
        problems = check(Path(args[0]))
        print("проблем: " + str(problems))
        return 1 if problems else 0

    if len(args) < 2:
        raise SystemExit(__doc__)

    exe = Path(args[0])
    out_dir = Path(args[1])

    if not exe.exists():
        raise SystemExit(f"нет файла: {exe}")

    problems = collect(exe, out_dir)
    problems += check(out_dir)
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
