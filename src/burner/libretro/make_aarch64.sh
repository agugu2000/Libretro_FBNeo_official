#!/bin/bash
# build_fbneo_rk3566.sh —— 放在 src/burner/libretro 目录下运行

set -e

LIBRETRO_DIR="$(pwd)"
JOBS="$(nproc)"

echo "==> 检查依赖"
command -v perl >/dev/null 2>&1 || { echo "缺 perl: sudo apt install perl"; exit 1; }
command -v aarch64-linux-gnu-gcc   >/dev/null 2>&1 || { echo "缺 aarch64-linux-gnu-gcc: sudo apt install gcc-aarch64-linux-gnu"; exit 1; }
command -v aarch64-linux-gnu-g++   >/dev/null 2>&1 || { echo "缺 aarch64-linux-gnu-g++: sudo apt install g++-aarch64-linux-gnu"; exit 1; }
command -v aarch64-linux-gnu-strip >/dev/null 2>&1 || { echo "缺 aarch64-linux-gnu-strip: sudo apt install binutils-aarch64-linux-gnu"; exit 1; }
[ -f "${LIBRETRO_DIR}/Makefile" ] || { echo "当前目录没有 Makefile，请在 src/burner/libretro 下运行"; exit 1; }

echo "==> 修改 Makefile (照抄 package.mk)"
sed -i "s|LDFLAGS += -static-libgcc -static-libstdc++|LDFLAGS += -static-libgcc|" Makefile

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