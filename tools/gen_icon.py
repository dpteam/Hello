#!/usr/bin/env python3
"""Генератор иконок Codename Hello.

Рисует мастер-изображение, сохраняет PNG/ICO и C-массив RGBA
для оконной иконки SDL. Запуск:

    python tools/gen_icon.py

Требуется Pillow. Результаты коммитятся в репозиторий, поэтому
сборка проекта от Pillow не зависит.
"""
from __future__ import annotations

import struct
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parent.parent
ASSETS = ROOT / "assets"
SRC = ROOT / "src"
FONT = ASSETS / "fonts" / "DejaVuSansMono-Bold.ttf"

BG = (8, 14, 10, 255)
BORDER = (0, 200, 90, 255)
GLOW = (0, 255, 80, 90)
FG = (0, 255, 80, 255)
DIM = (0, 120, 50, 255)

MASTER = 1024
ICON_SIZES = [16, 24, 32, 48, 64, 128, 256]


def rounded_mask(size: int, radius: int) -> Image.Image:
    mask = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(mask)
    draw.rounded_rectangle((0, 0, size - 1, size - 1), radius=radius, fill=255)
    return mask


def build_master() -> Image.Image:
    size = MASTER
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Фон с лёгким вертикальным градиентом.
    for y in range(size):
        t = y / (size - 1)
        color = (
            int(6 + 10 * (1 - t)),
            int(12 + 16 * (1 - t)),
            int(9 + 8 * (1 - t)),
            255,
        )
        draw.line((0, y, size, y), fill=color)

    # Тонкая сетка «терминала», почти незаметная.
    for i in range(1, 12):
        y = size * i // 12
        draw.line((60, y, size - 60, y), fill=(0, 255, 80, 8))

    # Внутренняя рамка-свечение.
    draw.rounded_rectangle((18, 18, size - 19, size - 19), radius=170,
                           outline=GLOW, width=10)
    draw.rounded_rectangle((30, 30, size - 31, size - 31), radius=158,
                           outline=BORDER, width=8)

    font = ImageFont.truetype(str(FONT), 460)
    text = ">"
    bbox = draw.textbbox((0, 0), text, font=font)
    tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]

    # Курсор-блок той же высоты, что и «>», но поуже.
    cursor_w = 120
    gap = 46
    total_w = tw + gap + cursor_w
    x = (size - total_w) // 2 - bbox[0]
    y = (size - th) // 2 - bbox[1] - 20

    draw.text((x, y), text, font=font, fill=FG)

    cursor_x = x + bbox[0] + tw + gap
    cursor_y = y + bbox[1] + 40
    draw.rounded_rectangle(
        (cursor_x, cursor_y, cursor_x + cursor_w, cursor_y + th - 70),
        radius=18, fill=FG)

    # Скругление.
    img.putalpha(rounded_mask(size, 190))
    return img


def save_outputs(master: Image.Image) -> None:
    ASSETS.mkdir(parents=True, exist_ok=True)

    png = master.resize((512, 512), Image.LANCZOS)
    png.save(ASSETS / "icon.png", optimize=True)

    # ICO генерируется из 256x256: Pillow сам делает нужные размеры.
    ico_base = master.resize((256, 256), Image.LANCZOS)
    ico_base.save(
        ASSETS / "icon.ico",
        format="ICO",
        sizes=[(s, s) for s in ICON_SIZES],
    )

    # .rc для windres: иконка приложения в проводнике.
    (ASSETS / "hello.rc").write_text(
        "/* Сгенерировано tools/gen_icon.py */\n"
        "IDI_HELLO ICON \"icon.ico\"\n",
        encoding="utf-8",
        newline="\n",
    )

    small = master.resize((64, 64), Image.LANCZOS).convert("RGBA")
    pixels = list(small.getdata())
    lines = [
        "/* Сгенерировано tools/gen_icon.py — не редактировать вручную. */",
        "#ifndef HELLO_ICON_RGBA_H",
        "#define HELLO_ICON_RGBA_H",
        "",
        "/* 64x64, RGBA, построчно. */",
        "static const unsigned char kIconRgba64[64 * 64 * 4] = {",
    ]
    for i in range(0, len(pixels), 8):
        chunk = pixels[i:i + 8]
        row = ", ".join(f"{r},{g},{b},{a}" for (r, g, b, a) in chunk)
        lines.append("    " + row + ",")
    lines += ["};", "", "#endif /* HELLO_ICON_RGBA_H */", ""]
    (SRC / "icon_rgba.h").write_text("\n".join(lines), encoding="utf-8",
                                     newline="\n")


def main() -> None:
    if not FONT.exists():
        raise SystemExit(f"нет шрифта: {FONT}")
    master = build_master()
    save_outputs(master)
    print("icon.png, icon.ico, hello.rc, src/icon_rgba.h — готово")


if __name__ == "__main__":
    main()
