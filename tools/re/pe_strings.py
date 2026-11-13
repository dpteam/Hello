#!/usr/bin/env python3
"""Строки в PE-секциях: ASCII и UTF-8 (кириллица).

    python tools/re/pe_strings.py файл.exe            # ASCII в .rdata/.data
    python tools/re/pe_strings.py файл.exe --utf8     # UTF-8 в .data
    python tools/re/pe_strings.py файл.exe --all      # все секции
"""
from __future__ import annotations

import re
import struct
import sys
from pathlib import Path

ASCII_RE = re.compile(rb"[\x20-\x7e]{4,}")
# Ведущий байт + продолжения; допускаем смешанные строки.
UTF8_RE = re.compile(rb"[\x20-\x7e\xc2-\xf4][\x20-\x7e\x80-\xbf\xc2-\xf4]{2,}")


def sections(data: bytes):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    opt = pe + 24
    image_base = struct.unpack_from("<I", data, opt + 28)[0]

    table = opt + opt_size
    out = []
    for i in range(nsec):
        off = table + i * 40
        name = data[off : off + 8].rstrip(b"\0").decode("latin-1")
        vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", data, off + 8)
        out.append((name, vaddr, rawptr, rawsize))
    return image_base, out


def printable_ratio(text: str) -> float:
    if not text:
        return 0.0
    good = sum(ch.isprintable() or ch in " \t" for ch in text)
    return good / len(text)


def main() -> None:
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    flags = {a for a in sys.argv[1:] if a.startswith("--")}
    if not args:
        raise SystemExit(__doc__)

    path = Path(args[0])
    data = path.read_bytes()
    image_base, secs = sections(data)

    wanted = {"--utf8": {".data", ".rdata"}, "--all": None}
    if "--all" in flags:
        targets = None
    elif "--utf8" in flags:
        targets = wanted["--utf8"]
    else:
        targets = {".rdata", ".data"}

    use_utf8 = "--utf8" in flags
    pattern = UTF8_RE if use_utf8 else ASCII_RE

    print(f"== {path} ({len(data):,} байт)")
    for name, vaddr, rawptr, rawsize in secs:
        if rawsize == 0:
            continue
        if targets is not None and name not in targets:
            continue

        body = data[rawptr : rawptr + rawsize]
        found = 0
        print(f"\n--- {name} ---")
        for match in pattern.finditer(body):
            raw = match.group()
            try:
                text = raw.decode("utf-8")
            except UnicodeDecodeError:
                continue
            if printable_ratio(text) < 0.85:
                continue
            if use_utf8 and not any(ord(ch) > 127 for ch in text):
                continue
            print(f"0x{image_base + vaddr + match.start():08X}  {text}")
            found += 1
        if found == 0:
            print("(ничего)")


if __name__ == "__main__":
    main()
