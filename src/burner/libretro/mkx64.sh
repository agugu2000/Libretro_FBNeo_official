#!/bin/bash
# ============================================================
# mkx64.sh —— 放在 src/burner/libretro 目录下运行
#
# 用法:
#   ./mkx64.sh           # 编译当前平台 x86_64 桌面版
#   ./mkx64.sh clean     # 清理编译产物
#
# 目标:
#   Windows MSYS2 MINGW64 -> fbneo_advanced_libretro.dll
#   Linux x86_64          -> fbneo_advanced_libretro.so
#
# 全量编译，不传 SUBSET，产物为 fbneo_advanced_libretro.*
# 不跑 generate-files，不改 Makefile，编译完成后 strip
# ============================================================

set -e

LIBRETRO_DIR="$(pwd)"
JOBS="$(nproc)"

# ---------- 平台判断 ----------
UNAME_S="$(uname -s)"
case "${UNAME_S}" in
    MINGW*|MSYS*|CYGWIN*)
        PLATFORM="win"
        TARGET_NAME="fbneo_advanced_libretro.dll"
        ;;
    Linux*)
        PLATFORM="unix"
        TARGET_NAME="fbneo_advanced_libretro.so"
        ;;
    *)
        echo "不支持的系统: ${UNAME_S}"
        exit 1
        ;;
esac

echo "==> 平台: ${PLATFORM}"
echo "==> 产物: ${TARGET_NAME}"

# ---------- clean ----------
if [ "${1:-}" = "clean" ]; then
    echo "==> 清理..."
    make clean platform="${PLATFORM}" || true
    echo "==> 清理完成"
    exit 0
fi

# ---------- 依赖检查 ----------
echo "==> 检查依赖"
command -v gcc   >/dev/null 2>&1 || { echo "缺 gcc";   exit 1; }
command -v g++   >/dev/null 2>&1 || { echo "缺 g++";   exit 1; }
command -v make  >/dev/null 2>&1 || { echo "缺 make";  exit 1; }
command -v strip >/dev/null 2>&1 || { echo "缺 strip"; exit 1; }
[ -f "Makefile" ] || { echo "当前目录没有 Makefile，请在 src/burner/libretro 下运行"; exit 1; }

# ---------- 正式编译 ----------
echo "==> 正式编译"
make -j"${JOBS}" platform="${PLATFORM}"

# ---------- 找产物 ----------
TARGET_PATH="${LIBRETRO_DIR}/${TARGET_NAME}"
if [ ! -f "${TARGET_PATH}" ]; then
    echo "==> 编译失败，未找到 ${TARGET_PATH}"
    exit 1
fi

echo ""
echo "==> 编译成功"
file "${TARGET_PATH}"
ls -lh "${TARGET_PATH}"

# ---------- strip ----------
echo "==> strip 符号"
strip "${TARGET_PATH}"
echo "==> strip 后"
file "${TARGET_PATH}"
ls -lh "${TARGET_PATH}"

echo ""
echo "产物: ${TARGET_PATH}"