#!/usr/bin/env python3
"""Разбор PE-файлов: секции, дата сборки, импорты, символы.

Только стандартная библиотека. Запуск:

    python tools/re/pe_info.py archive/2013-01-03-sdl-console/hello.exe
"""
from __future__ import annotations

import struct
import sys
from datetime import datetime, timezone
from pathlib import Path


def read_sections(data: bytes) -> tuple[int, int, list[tuple[str, int, int, int, int]]]:
    if data[:2] != b"MZ":
        raise SystemExit("не PE: нет сигнатуры MZ")

    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe : pe + 4] != b"PE\0\0":
        raise SystemExit("не PE: нет сигнатуры PE")

    timestamp = struct.unpack_from("<I", data, pe + 8)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    opt = pe + 24
    image_base = struct.unpack_from("<I", data, opt + 28)[0]
    subsystem = struct.unpack_from("<H", data, opt + 68)[0]

    table = opt + opt_size
    sections = []
    for i in range(nsec):
        off = table + i * 40
        name = data[off : off + 8].rstrip(b"\0").decode("latin-1")
        vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", data, off + 8)
        sections.append((name, vaddr, vsize, rawptr, rawsize))

    return timestamp, image_base, subsystem, sections


def imports(data: bytes) -> list[str]:
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    opt = pe + 24
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    table = opt + opt_size

    dir_rva = struct.unpack_from("<I", data, opt + 96 + 8)[0]

    def rva_to_off(rva: int) -> int | None:
        for i in range(nsec):
            off = table + i * 40
            vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", data, off + 8)
            if vaddr <= rva < vaddr + max(vsize, rawsize):
                return rawptr + (rva - vaddr)
        return None

    result = []
    off = rva_to_off(dir_rva) if dir_rva else None
    if off is None:
        return result

    while True:
        entry = data[off : off + 20]
        if len(entry) < 20 or entry == b"\0" * 20:
            break
        name_rva = struct.unpack_from("<I", entry, 12)[0]
        name_off = rva_to_off(name_rva)
        if name_off:
            end = data.index(b"\0", name_off)
            result.append(data[name_off:end].decode("latin-1"))
        off += 20
    return result


def symbols(data: bytes) -> list[tuple[int, str]]:
    """Имена из таблицы символов (если не стрипнута)."""
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    coff = pe + 4
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    ptr_sym = struct.unpack_from("<I", data, coff + 8)[0]
    num_sym = struct.unpack_from("<I", data, coff + 12)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]

    if ptr_sym == 0 or num_sym == 0:
        return []

    strtab = ptr_sym + num_sym * 18
    out = []
    for i in range(num_sym):
        off = ptr_sym + i * 18
        name_raw = data[off : off + 8]
        value = struct.unpack_from("<I", data, off + 8)[0]
        if name_raw[:4] == b"\0\0\0\0":
            name_off = struct.unpack_from("<I", data, off + 4)[0]
            start = strtab + name_off
            end = data.index(b"\0", start)
            name = data[start:end].decode("latin-1", "replace")
        else:
            name = name_raw.rstrip(b"\0").decode("latin-1", "replace")
        if name:
            out.append((value, name))
    return out


SUBSYSTEMS = {1: "NATIVE", 2: "WINDOWS/GUI", 3: "CONSOLE", 9: "WINDOWS/CE"}


def main() -> None:
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)

    for arg in sys.argv[1:]:
        path = Path(arg)
        data = path.read_bytes()
        ts, base, subsystem, sections = read_sections(data)

        print(f"== {path}")
        print(f"   размер:    {len(data):,} байт")
        print(f"   imagebase: 0x{base:08X}")
        print(f"   собран:    {datetime.fromtimestamp(ts, tz=timezone.utc):%Y-%m-%d %H:%M:%S} UTC")
        print(f"   подсистема:{SUBSYSTEMS.get(subsystem, subsystem)}")

        print("   секции:")
        for name, vaddr, vsize, rawptr, rawsize in sections:
            print(f"     {name:10} va=0x{vaddr:08X} vsize=0x{vsize:X} raw=0x{rawptr:X}+0x{rawsize:X}")

        dlls = imports(data)
        if dlls:
            print("   импорты: " + ", ".join(dlls))

        syms = symbols(data)
        if syms:
            print(f"   символов: {len(syms)}")
            for value, name in syms:
                if name.startswith(("_", "?")) and any(
                    key in name
                    for key in ("main", "process", "analyze", "SDL_", "TTF_", "Rnd")
                ):
                    print(f"     0x{value:08X} {name}")
        else:
            print("   символов: нет (стрипнуто)")
        print()


if __name__ == "__main__":
    main()
