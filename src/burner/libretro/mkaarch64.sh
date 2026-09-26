#!/bin/bash
# ============================================================
# mkaarch64.sh —— 放在 src/burner/libretro 目录下运行
#
# 用法:
#   ./mkaarch64.sh           # 编译 Linux aarch64 (RK3566 等)
#   ./mkaarch64.sh clean     # 清理编译产物
#
# 目标: Linux aarch64，用 aarch64-linux-gnu-gcc/g++
# ============================================================

set -e

LIBRETRO_DIR="$(pwd)"
JOBS="$(nproc)"
MAKEFILE="${LIBRETRO_DIR}/Makefile"
BACKUP="${MAKEFILE}.orig"

# ---------- clean ----------
if [ "${1:-}" = "clean" ]; then
    echo "==> 清理..."
    if [ -f "${MAKEFILE}" ]; then
        make clean platform=aarch64 || true
    fi
    [ -f "${BACKUP}" ] && rm -f "${BACKUP}" && echo "已删除 ${BACKUP}"
    echo "==> 清理完成"
    exit 0
fi

# ---------- 依赖检查 ----------
echo "==> 检查依赖"
command -v perl >/dev/null 2>&1 || { echo "缺 perl: sudo apt install perl"; exit 1; }
command -v aarch64-linux-gnu-gcc   >/dev/null 2>&1 || { echo "缺 aarch64-linux-gnu-gcc: sudo apt install gcc-aarch64-linux-gnu"; exit 1; }
command -v aarch64-linux-gnu-g++   >/dev/null 2>&1 || { echo "缺 aarch64-linux-gnu-g++: sudo apt install g++-aarch64-linux-gnu"; exit 1; }
command -v aarch64-linux-gnu-strip >/dev/null 2>&1 || { echo "缺 aarch64-linux-gnu-strip: sudo apt install binutils-aarch64-linux-gnu"; exit 1; }
[ -f "${MAKEFILE}" ] || { echo "当前目录没有 Makefile，请在 src/burner/libretro 下运行"; exit 1; }

# ---------- 退出时还原 Makefile ----------
restore_makefile() {
  if [ -f "${BACKUP}" ]; then
    mv -f "${BACKUP}" "${MAKEFILE}"
    echo "==> Makefile 已还原"
  fi
}
trap restore_makefile EXIT

echo "==> 备份 Makefile 到 ${BACKUP}"
cp -f "${MAKEFILE}" "${BACKUP}"

echo "==> 修改 Makefile (rocknix package.mk)"
sed -i "s|LDFLAGS += -static-libgcc -static-libstdc++|LDFLAGS += -static-libgcc|" "${MAKEFILE}"

echo "==> 生成文件"
make -j"${JOBS}" platform=aarch64 generate-files

echo "==> 正式编译"
make -j"${JOBS}" platform=aarch64

SO="${LIBRETRO_DIR}/fbneo_advanced_libretro.so"
if [ -f "${SO}" ]; then
  echo ""
  echo "==> 编译成功"
  file "${SO}"
  echo "==> strip 符号"
  aarch64-linux-gnu-strip "${SO}"
  echo "==> strip 后"
  file "${SO}"
  ls -lh "${SO}"
  echo "产物: ${SO}"
else
  echo "==> 编译失败，未找到 ${SO}"
  exit 1
fi