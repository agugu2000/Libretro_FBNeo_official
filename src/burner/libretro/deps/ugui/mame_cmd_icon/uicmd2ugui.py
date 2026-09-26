#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
uicmd14.png -> UGUI C 数组。
默认找脚本同目录的 uicmd14.png，输出 uicmd14.c。
用法:
    python uicmd2ugui.py
    python uicmd2ugui.py -o out.c -n FONT_X
"""

import os
import sys
import argparse
from PIL import Image


def script_dir():
    return os.path.dirname(os.path.abspath(__file__))


def pixel_is_set(px, x, y):
    r, g, b, a = px[x, y]
    return r == 0 and g == 0 and b == 0


class Glyph:
    __slots__ = ("cp", "width", "x_offs", "y_offs", "bm_w", "bm_h", "bitmap")


def parse_png(path):
    img = Image.open(path).convert("RGBA")
    W, H = img.size
    px = img.load()

    font_height = 0
    font_yoffs = 0
    glyphs = {}

    row_start = 0
    while row_start < H:
        while row_start < H and not pixel_is_set(px, 0, row_start):
            row_start += 1
        if row_start >= H:
            break

        row_end = row_start + 1
        while row_end < H and pixel_is_set(px, 0, row_end):
            row_end += 1
        row_end -= 1

        baseline = row_start
        while baseline <= row_end and not pixel_is_set(px, 1, baseline):
            baseline += 1
        if baseline > row_end:
            sys.stderr.write(f"[warn] no baseline in rows {row_start}-{row_end}\n")
            break

        h_this = row_end - row_start + 1
        # 修正：y_off = 基线相对 cell 顶边的距离（UGUI 语义）
        y_this = baseline - row_start

        if font_height == 0:
            font_height, font_yoffs = h_this, y_this
        else:
            if font_height != h_this or font_yoffs != y_this:
                sys.stderr.write(f"[warn] inconsistent at rows {row_start}-{row_end}\n")
                break

        ch_start = 0
        for x in range(4):
            for y in range(4):
                ch_start = (ch_start << 1) | (1 if pixel_is_set(px, 2 + x, row_start + y) else 0)

        sep_row = row_end + 2
        col_start = 0
        while col_start < W:
            while col_start < W and not pixel_is_set(px, col_start, sep_row):
                col_start += 1
            if col_start >= W:
                break
            col_end = col_start + 1
            while col_end < W and pixel_is_set(px, col_end, sep_row):
                col_end += 1
            col_end -= 1

            g = Glyph()
            g.cp = ch_start
            g.bitmap = []
            for y in range(row_start, row_end + 1):
                row = []
                for x in range(col_start, col_end + 1):
                    row.append(1 if pixel_is_set(px, x, y) else 0)
                g.bitmap.append(row)
            g.width  = col_end - col_start + 1
            g.x_offs = 0
            g.y_offs = font_yoffs
            g.bm_w   = len(g.bitmap[0])
            g.bm_h   = len(g.bitmap)

            glyphs[ch_start] = g
            ch_start += 1
            col_start = col_end + 1

        row_start = row_end + 1

    return glyphs, font_height, font_yoffs


def bits_to_ugui(bitmap):
    h = len(bitmap)
    w = len(bitmap[0]) if h else 0
    bpr = (w + 7) // 8
    out = bytearray()
    for y in range(h):
        for bx in range(bpr):
            byte = 0
            for k in range(8):
                x = bx * 8 + k
                if x >= w:
                    break
                if bitmap[y][x]:
                    byte |= (1 << k)
            out.append(byte)
    return bytes(out)


def emit_c(glyphs, font_height, font_yoffs, name, out_path):
    valid = sorted([g for g in glyphs.values() if g.bm_w > 0 and g.bm_h > 0],
                   key=lambda g: g.cp)
    n = len(valid)
    print(f"glyphs:   {n}")
    if n == 0:
        print("no glyphs")
        return
    cps = [g.cp for g in valid]
    print(f"cp range: U+{min(cps):04X} ~ U+{max(cps):04X}")

    max_w = max(g.bm_w for g in valid)
    max_h = max(g.bm_h for g in valid)

    # 过渡值（合并时会被 SIMSUN2 覆盖）
    ascender  = font_yoffs
    descender = font_yoffs - font_height

    notdef_adv = valid[0].width

    total = 20 + n * 2 + n * 10 + n * 4
    for g in valid:
        total += ((g.bm_w + 7) // 8) * g.bm_h

    with open(out_path, "w") as o:
        o.write(f"// Auto-generated\n")
        o.write(f"// glyphs={n} max={max_w}x{max_h} asc={ascender} desc={descender}\n")
        o.write('#include "ugui.h"\n')
        o.write(f"#ifdef UGUI_USE_FONT_{name}\n\n")
        o.write(f"UG_FONT {name}[] = {{\n")

        o.write("  0x00,0x00,\n")
        o.write(f"  0x{max_w>>8:02X},0x{max_w&0xFF:02X},\n")
        o.write(f"  0x{max_h>>8:02X},0x{max_h&0xFF:02X},\n")
        o.write(f"  0x{(n>>24)&0xFF:02X},0x{(n>>16)&0xFF:02X},0x{(n>>8)&0xFF:02X},0x{n&0xFF:02X},\n")
        o.write(f"  0x{(total>>24)&0xFF:02X},0x{(total>>16)&0xFF:02X},0x{(total>>8)&0xFF:02X},0x{total&0xFF:02X},\n")
        o.write(f"  0x{notdef_adv>>8:02X},0x{notdef_adv&0xFF:02X},\n")
        o.write(f"  0x{(ascender>>8)&0xFF:02X},0x{ascender&0xFF:02X},\n")
        o.write(f"  0x{(descender>>8)&0xFF:02X},0x{descender&0xFF:02X},\n")

        o.write("\n  /* codepoints */\n")
        for g in valid:
            o.write(f"  0x{g.cp>>8:02X},0x{g.cp&0xFF:02X}, /* U+{g.cp:04X} */\n")

        o.write("\n  /* metrics: w h x_off y_off adv */\n")
        for g in valid:
            o.write(f"  0x{g.bm_w>>8:02X},0x{g.bm_w&0xFF:02X},"
                    f"0x{g.bm_h>>8:02X},0x{g.bm_h&0xFF:02X},"
                    f"0x{(g.x_offs>>8)&0xFF:02X},0x{g.x_offs&0xFF:02X},"
                    f"0x{(g.y_offs>>8)&0xFF:02X},0x{g.y_offs&0xFF:02X},"
                    f"0x{g.width>>8:02X},0x{g.width&0xFF:02X},\n")

        o.write("\n  /* data_offsets */\n")
        off = 0
        for g in valid:
            o.write(f"  0x{(off>>24)&0xFF:02X},0x{(off>>16)&0xFF:02X},0x{(off>>8)&0xFF:02X},0x{off&0xFF:02X},\n")
            off += ((g.bm_w + 7) // 8) * g.bm_h

        o.write("\n  /* data */\n")
        for g in valid:
            bm = bits_to_ugui(g.bitmap)
            o.write("  ")
            for b in bm:
                o.write(f"0x{b:02X},")
            o.write(f" /* U+{g.cp:04X} */\n")

        o.write("};\n#endif\n")

    print(f"wrote:    {out_path}")


def main():
    here = script_dir()
    ap = argparse.ArgumentParser()
    ap.add_argument("input", nargs="?", default=os.path.join(here, "uicmd14.png"))
    ap.add_argument("-o", "--output", default=None)
    ap.add_argument("-n", "--name", default="FONT_UICMD14")
    args = ap.parse_args()

    in_path = os.path.abspath(args.input)
    if not os.path.isfile(in_path):
        print(f"error: not found: {in_path}")
        sys.exit(1)

    out_path = args.output or os.path.join(here, "uicmd14.c")
    out_path = os.path.abspath(out_path)

    print(f"in:       {in_path}")
    glyphs, fh, fy = parse_png(in_path)
    emit_c(glyphs, fh, fy, args.name, out_path)


if __name__ == "__main__":
    main()