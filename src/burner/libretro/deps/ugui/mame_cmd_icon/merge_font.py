#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
合并 SIMSUN2_13X13_raw.c + uicmd14.c -> SIMSUN2_13X13.c
合并后自动：
  - 校验字节数、升序、无重复
  - 逐项校验所有 glyph 的 metrics 和 bitmap 与源文件一致
  - 额外随机抽查 10 个汉字 + 10 个图标，打印对比结果
用法:
    python merge_and_verify.py
"""

import os
import re
import sys
import random
import argparse

HEADER_SIZE = 20
CP_SIZE     = 2
METRIC_SIZE = 10
OFFSET_SIZE = 4

# 图标段校准
ICON_Y_SHIFT = 0
ICON_ADV     = 0

# 目标变量名 / 宏
FONT_NAME   = "FONT_SIMSUN2_13X13"
IFDEF_MACRO = "UGUI_USE_FONT_SIMSUN2_13X13"

# 抽查数量
RANDOM_SAMPLES = 10
RANDOM_SEED    = 42


def script_dir():
    return os.path.dirname(os.path.abspath(__file__))


# ============================================================
# 解析
# ============================================================
def parse_font_c(path):
    with open(path, "r", encoding="utf-8") as f:
        txt = f.read()
    txt = re.sub(r"//[^\n]*", "", txt)
    txt = re.sub(r"/\*.*?\*/", "", txt, flags=re.S)

    m = re.search(r"UG_FONT\s+\w+\s*\[\s*\]\s*=\s*\{(.*?)\};", txt, flags=re.S)
    if not m:
        raise ValueError(f"no UG_FONT array in {path}")
    body = m.group(1)
    b = [int(x, 16) for x in re.findall(r"0x([0-9A-Fa-f]{2})", body)]

    if len(b) < HEADER_SIZE:
        raise ValueError(f"too few bytes in {path}")

    n          = (b[6] << 24) | (b[7] << 16) | (b[8] << 8) | b[9]
    total_size = (b[10] << 24) | (b[11] << 16) | (b[12] << 8) | b[13]
    max_w      = (b[2] << 8) | b[3]
    max_h      = (b[4] << 8) | b[5]
    notdef_adv = (b[14] << 8) | b[15]
    ascender   = (b[16] << 8) | b[17]
    descender  = (b[18] << 8) | b[19]
    if ascender >= 0x8000: ascender -= 0x10000
    if descender >= 0x8000: descender -= 0x10000

    if len(b) != total_size:
        raise ValueError(f"{path}: extracted {len(b)} bytes, header says {total_size}")

    expected_data_start = HEADER_SIZE + n * CP_SIZE + n * METRIC_SIZE + n * OFFSET_SIZE
    if total_size < expected_data_start:
        raise ValueError(f"bad total_size in {path}")

    p = HEADER_SIZE
    codepoints = []
    for _ in range(n):
        codepoints.append((b[p] << 8) | b[p+1]); p += CP_SIZE

    metrics = []
    for _ in range(n):
        w   = (b[p+0] << 8) | b[p+1]
        h   = (b[p+2] << 8) | b[p+3]
        xo  = (b[p+4] << 8) | b[p+5]
        yo  = (b[p+6] << 8) | b[p+7]
        adv = (b[p+8] << 8) | b[p+9]
        if xo >= 0x8000: xo -= 0x10000
        if yo >= 0x8000: yo -= 0x10000
        metrics.append([w, h, xo, yo, adv]); p += METRIC_SIZE

    offsets = []
    for _ in range(n):
        off = (b[p] << 24) | (b[p+1] << 16) | (b[p+2] << 8) | b[p+3]
        offsets.append(off); p += OFFSET_SIZE

    data = bytes(b[expected_data_start:total_size])

    return {
        "n": n, "max_w": max_w, "max_h": max_h,
        "notdef_adv": notdef_adv,
        "ascender": ascender, "descender": descender,
        "codepoints": codepoints,
        "metrics": metrics,
        "offsets": offsets,
        "data": data,
    }


def glyph_bytes(font, i):
    off = font["offsets"][i]
    nxt = font["offsets"][i+1] if i+1 < font["n"] else len(font["data"])
    return font["data"][off:nxt]


# ============================================================
# 合并
# ============================================================
def merge(simsun, icons):
    combined = []
    for i in range(simsun["n"]):
        combined.append((simsun["codepoints"][i], "s", i))
    for i in range(icons["n"]):
        combined.append((icons["codepoints"][i], "i", i))
    combined.sort(key=lambda x: x[0])

    n = len(combined)
    max_w = max(simsun["max_w"], icons["max_w"])
    max_h = max(simsun["max_h"], icons["max_h"])

    ascender   = simsun["ascender"]
    descender  = simsun["descender"]
    notdef_adv = simsun["notdef_adv"]

    new_data = bytearray()
    new_offsets = []
    new_metrics = []
    new_cps = []

    for cp, src, idx in combined:
        new_offsets.append(len(new_data))
        new_cps.append(cp)
        if src == "s":
            w, h, xo, yo, adv = simsun["metrics"][idx]
        else:
            w, h, xo, yo, adv = icons["metrics"][idx]
            yo += ICON_Y_SHIFT
            if ICON_ADV > 0:
                adv = ICON_ADV
        new_metrics.append([w, h, xo, yo, adv])
        new_data += glyph_bytes(simsun if src == "s" else icons, idx)

    total_size = HEADER_SIZE + n*CP_SIZE + n*METRIC_SIZE + n*OFFSET_SIZE + len(new_data)

    return {
        "n": n, "max_w": max_w, "max_h": max_h,
        "notdef_adv": notdef_adv,
        "ascender": ascender, "descender": descender,
        "codepoints": new_cps,
        "metrics": new_metrics,
        "offsets": new_offsets,
        "data": bytes(new_data),
        "total_size": total_size,
        "combined": combined,
    }


# ============================================================
# 写 C
# ============================================================
def emit_c(font, out_path):
    n = font["n"]
    max_w = font["max_w"]
    max_h = font["max_h"]
    ascender = font["ascender"]
    descender = font["descender"]
    notdef_adv = font["notdef_adv"]
    total_size = font["total_size"]

    with open(out_path, "w", encoding="utf-8") as o:
        o.write(f"// Merged: {FONT_NAME}\n")
        o.write(f"// glyphs={n} max={max_w}x{max_h} asc={ascender} desc={descender}\n")
        o.write(f'#include "ugui.h"\n')
        o.write(f"#ifdef {IFDEF_MACRO}\n\n")
        o.write(f"UG_FONT {FONT_NAME}[] = {{\n")

        o.write("  0x00,0x00,\n")
        o.write(f"  0x{max_w>>8:02X},0x{max_w&0xFF:02X},\n")
        o.write(f"  0x{max_h>>8:02X},0x{max_h&0xFF:02X},\n")
        o.write(f"  0x{(n>>24)&0xFF:02X},0x{(n>>16)&0xFF:02X},0x{(n>>8)&0xFF:02X},0x{n&0xFF:02X},\n")
        o.write(f"  0x{(total_size>>24)&0xFF:02X},0x{(total_size>>16)&0xFF:02X},0x{(total_size>>8)&0xFF:02X},0x{total_size&0xFF:02X},\n")
        o.write(f"  0x{notdef_adv>>8:02X},0x{notdef_adv&0xFF:02X},\n")
        o.write(f"  0x{(ascender>>8)&0xFF:02X},0x{ascender&0xFF:02X},\n")
        o.write(f"  0x{(descender>>8)&0xFF:02X},0x{descender&0xFF:02X},\n")

        o.write("\n  /* codepoints */\n")
        for cp in font["codepoints"]:
            o.write(f"  0x{cp>>8:02X},0x{cp&0xFF:02X},\n")

        o.write("\n  /* metrics: w h x_off y_off adv */\n")
        for w, h, xo, yo, adv in font["metrics"]:
            o.write(f"  0x{w>>8:02X},0x{w&0xFF:02X},"
                    f"0x{h>>8:02X},0x{h&0xFF:02X},"
                    f"0x{(xo>>8)&0xFF:02X},0x{xo&0xFF:02X},"
                    f"0x{(yo>>8)&0xFF:02X},0x{yo&0xFF:02X},"
                    f"0x{adv>>8:02X},0x{adv&0xFF:02X},\n")

        o.write("\n  /* data_offsets */\n")
        for off in font["offsets"]:
            o.write(f"  0x{(off>>24)&0xFF:02X},0x{(off>>16)&0xFF:02X},0x{(off>>8)&0xFF:02X},0x{off&0xFF:02X},\n")

        o.write("\n  /* data */\n")
        for cp, src, idx in font["combined"]:
            if src == "s":
                bm = glyph_bytes(simsun_global, idx)
            else:
                bm = glyph_bytes(icons_global, idx)
            o.write("  ")
            for byte in bm:
                o.write(f"0x{byte:02X},")
            o.write(f" /* U+{cp:04X} */\n")

        o.write("};\n#endif\n")


# ============================================================
# 验证
# ============================================================
def get_bitmap(font, idx):
    off = font["offsets"][idx]
    nxt = font["offsets"][idx+1] if idx+1 < font["n"] else len(font["data"])
    return font["data"][off:nxt]


def verify_full(simsun, icons, merged):
    # 1. 数量
    assert merged["n"] == simsun["n"] + icons["n"], "count mismatch"
    print(f"count: {simsun['n']} + {icons['n']} = {merged['n']}  OK")

    # 2. 升序、无重复
    for i in range(1, merged["n"]):
        if merged["codepoints"][i] <= merged["codepoints"][i-1]:
            raise AssertionError(f"order error at {i}")
    print("codepoints strictly ascending  OK")

    # 3. 建索引
    m_idx = {cp: i for i, cp in enumerate(merged["codepoints"])}

    # 4. 逐项校验 SIMSUN2 段
    for i, cp in enumerate(simsun["codepoints"]):
        ni = m_idx[cp]
        if simsun["metrics"][i] != merged["metrics"][ni]:
            raise AssertionError(f"simsun U+{cp:04X} metric mismatch")
        if get_bitmap(simsun, i) != get_bitmap(merged, ni):
            raise AssertionError(f"simsun U+{cp:04X} bitmap mismatch")
    print(f"all {simsun['n']} simsun glyphs identical  OK")

    # 5. 逐项校验 ICON 段
    for i, cp in enumerate(icons["codepoints"]):
        ni = m_idx[cp]
        # 注意：icons 段可能被 ICON_Y_SHIFT/ICON_ADV 改了，要按同样规则比
        iw, ih, ixo, iyo, iadv = icons["metrics"][i]
        iyo += ICON_Y_SHIFT
        if ICON_ADV > 0:
            iadv = ICON_ADV
        expect = [iw, ih, ixo, iyo, iadv]
        if expect != merged["metrics"][ni]:
            raise AssertionError(f"icon U+{cp:04X} metric mismatch: {expect} vs {merged['metrics'][ni]}")
        if get_bitmap(icons, i) != get_bitmap(merged, ni):
            raise AssertionError(f"icon U+{cp:04X} bitmap mismatch")
    print(f"all {icons['n']} icon glyphs identical  OK")

    # 6. data_offsets 逐项校验
    for i in range(merged["n"]):
        w, h = merged["metrics"][i][0], merged["metrics"][i][1]
        bpr = (w + 7) // 8
        need = bpr * h
        nxt = merged["offsets"][i+1] if i+1 < merged["n"] else len(merged["data"])
        if merged["offsets"][i] + need != nxt:
            raise AssertionError(f"offset mismatch at {i}")
    print("all data_offsets consistent  OK")


def random_check(simsun, icons, merged, n_simsun=10, n_icons=10):
    random.seed(RANDOM_SEED)
    m_idx = {cp: i for i, cp in enumerate(merged["codepoints"])}

    # ---- 抽查汉字 ----
    han_cps = [cp for cp in simsun["codepoints"] if 0x4E00 <= cp <= 0x9FFF]
    if not han_cps:
        print("warn: no han chars to sample")
    else:
        picks = random.sample(han_cps, min(n_simsun, len(han_cps)))
        print(f"\n--- random {len(picks)} han chars ---")
        for cp in picks:
            si = simsun["codepoints"].index(cp)
            mi = m_idx[cp]
            sm = simsun["metrics"][si]
            mm = merged["metrics"][mi]
            sb = get_bitmap(simsun, si)
            mb = get_bitmap(merged, mi)
            assert sm == mm, f"U+{cp:04X} metric"
            assert sb == mb, f"U+{cp:04X} bitmap"
            print(f"  U+{cp:04X}  w={mm[0]:2d} h={mm[1]:2d} "
                  f"x_off={mm[2]:3d} y_off={mm[3]:3d} adv={mm[4]:2d}  "
                  f"bytes={len(mb):3d}  OK")

    # ---- 抽查图标 ----
    icon_cps = [cp for cp in icons["codepoints"]]
    picks = random.sample(icon_cps, min(n_icons, len(icon_cps)))
    print(f"\n--- random {len(picks)} icon chars ---")
    for cp in picks:
        ii = icons["codepoints"].index(cp)
        mi = m_idx[cp]
        im = icons["metrics"][ii]
        # 应用校准
        iw, ih, ixo, iyo, iadv = im
        iyo += ICON_Y_SHIFT
        if ICON_ADV > 0:
            iadv = ICON_ADV
        mm = merged["metrics"][mi]
        ib = get_bitmap(icons, ii)
        mb = get_bitmap(merged, mi)
        assert [iw, ih, ixo, iyo, iadv] == mm, f"U+{cp:04X} metric"
        assert ib == mb, f"U+{cp:04X} bitmap"
        print(f"  U+{cp:04X}  w={mm[0]:2d} h={mm[1]:2d} "
              f"x_off={mm[2]:3d} y_off={mm[3]:3d} adv={mm[4]:2d}  "
              f"bytes={len(mb):3d}  OK")


# ============================================================
# main
# ============================================================
def main():
    global simsun_global, icons_global

    here = script_dir()
    ap = argparse.ArgumentParser()
    ap.add_argument("--simsun", default=os.path.join(here, "SIMSUN2_13X13_raw.c"))
    ap.add_argument("--icons",  default=os.path.join(here, "uicmd14.c"))
    ap.add_argument("-o", "--output", default=None)
    args = ap.parse_args()

    simsun_path = os.path.abspath(args.simsun)
    icons_path  = os.path.abspath(args.icons)
    out_path    = os.path.abspath(args.output) if args.output else os.path.join(here, "SIMSUN2_13X13.c")

    for p in (simsun_path, icons_path):
        if not os.path.isfile(p):
            print(f"error: not found: {p}")
            sys.exit(1)

    print(f"simsun: {simsun_path}")
    print(f"icons:  {icons_path}\n")

    simsun = parse_font_c(simsun_path)
    icons  = parse_font_c(icons_path)
    simsun_global = simsun
    icons_global  = icons
    print(f"parsed: simsun n={simsun['n']}, icons n={icons['n']}")
    print(f"  simsun: asc={simsun['ascender']} desc={simsun['descender']} "
          f"max={simsun['max_w']}x{simsun['max_h']}")
    print(f"  icons:  asc={icons['ascender']} desc={icons['descender']} "
          f"max={icons['max_w']}x{icons['max_h']}\n")

    merged = merge(simsun, icons)
    print(f"merged: total={merged['n']} "
          f"asc={merged['ascender']} desc={merged['descender']} "
          f"max={merged['max_w']}x{merged['max_h']}\n")

    # ========== 合并后自检 ==========
    print("========== verification ==========")
    verify_full(simsun, icons, merged)
    random_check(simsun, icons, merged, RANDOM_SAMPLES, RANDOM_SAMPLES)
    print("\n========== ALL CHECKS PASSED ==========\n")

    # ========== 写文件 ==========
    emit_c(merged, out_path)
    print(f"wrote: {out_path}")


if __name__ == "__main__":
    main()