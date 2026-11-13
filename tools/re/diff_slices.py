#!/usr/bin/env python3
"""Сравнение двух версий исходника: что именно изменилось между срезами.

    python tools/re/diff_slices.py старый.c новый.c

Полезно при разборе архива: показывает, какая правка сломала сборку.
"""
from __future__ import annotations

import difflib
import sys
from pathlib import Path


def read_text(path: Path) -> tuple[list[str], str]:
    raw = path.read_bytes()
    for encoding in ("utf-8-sig", "utf-8", "cp1251"):
        try:
            return raw.decode(encoding).splitlines(), encoding
        except UnicodeDecodeError:
            continue
    return raw.decode("latin-1").splitlines(), "latin-1"


def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)

    old_path, new_path = Path(sys.argv[1]), Path(sys.argv[2])
    old, old_enc = read_text(old_path)
    new, new_enc = read_text(new_path)

    print(f"старый: {old_path.name}  {len(old)} строк  кодировка={old_enc}")
    print(f"новый:  {new_path.name}  {len(new)} строк  кодировка={new_enc}")

    if old_enc != new_enc:
        print(f"ВНИМАНИЕ: кодировки различаются ({old_enc} → {new_enc})")

    diff = list(
        difflib.unified_diff(
            old, new,
            fromfile=f"{old_path.name} [{old_enc}]",
            tofile=f"{new_path.name} [{new_enc}]",
            lineterm="",
            n=2,
        )
    )

    if not diff:
        print("\nсодержимое идентично")
        return

    added = sum(1 for line in diff if line.startswith("+") and not line.startswith("+++"))
    removed = sum(1 for line in diff if line.startswith("-") and not line.startswith("---"))
    print(f"\nизменений: +{added} / -{removed}\n")
    print("\n".join(diff))


if __name__ == "__main__":
    main()
