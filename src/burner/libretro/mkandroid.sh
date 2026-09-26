#!/bin/bash
# ============================================================
# mkandroid.sh —— 放在 src/burner/libretro 目录下运行
#
# 用法:
#   ./mkandroid.sh                # 只编 arm64-v8a（默认）
#   ./mkandroid.sh armeabi-v7a    # 只编 armeabi-v7a
#   ./mkandroid.sh arm64-v8a      # 只编 arm64-v8a
#   ./mkandroid.sh x86            # 只编 x86
#   ./mkandroid.sh x86_64         # 只编 x86_64
#   ./mkandroid.sh all            # 编全部 4 个平台
#   ./mkandroid.sh clean          # 清理 obj/ 和 libs/
#
# 产物:
#   libs/<平台>/fbneo_advanced_libretro.so
# ============================================================

set -e

TARGET="${1:-arm64-v8a}"
all_platforms=(armeabi-v7a arm64-v8a x86 x86_64)

CURRENT_DIR="$(pwd)"
JNICALL_DIR="${CURRENT_DIR}/jni"
ANDROID_MK="${JNICALL_DIR}/Android.mk"
ANDROID_MK_BAK="${JNICALL_DIR}/Android.mk.backup"
LIBS_TARGET_PATH="${CURRENT_DIR}/libs"
OBJ_DIR="${CURRENT_DIR}/obj"
JOBS="$(nproc)"

# ---------- clean ----------
if [ "$TARGET" = "clean" ]; then
    echo "==> 清理编译文件..."
    (
        cd "${JNICALL_DIR}"
        ndk-build clean 2>/dev/null || true
    )
    [ -d "${OBJ_DIR}" ] && rm -rf "${OBJ_DIR}" && echo "已删除 ${OBJ_DIR}"
    [ -d "${LIBS_TARGET_PATH}" ] && rm -rf "${LIBS_TARGET_PATH}" && echo "已删除 ${LIBS_TARGET_PATH}"
    [ -f "${ANDROID_MK_BAK}" ] && mv -f "${ANDROID_MK_BAK}" "${ANDROID_MK}" && echo "已还原 Android.mk"
    echo "==> 清理完成"
    exit 0
fi

# ---------- 参数校验 ----------
if [ "$TARGET" = "all" ]; then
    BUILD_PLATFORMS=("${all_platforms[@]}")
    APP_ABI_ARG=""
else
    ok=0
    for p in "${all_platforms[@]}"; do
        [ "$p" = "$TARGET" ] && ok=1 && break
    done
    [ $ok -eq 1 ] || { echo "平台不被允许: $TARGET"; echo "只接受: armeabi-v7a / arm64-v8a / x86 / x86_64 / all / clean"; exit 1; }
    BUILD_PLATFORMS=("$TARGET")
    APP_ABI_ARG="APP_ABI=$TARGET"
fi

command -v ndk-build >/dev/null 2>&1 || { echo "缺 ndk-build，请把 Android NDK 加入 PATH"; exit 1; }
[ -f "${ANDROID_MK}" ] || { echo "找不到 ${ANDROID_MK}"; exit 1; }

# ---------- 找 strip ----------
NDK_ROOT="$(cd "$(dirname "$(command -v ndk-build)")" && pwd)"
LLVM_STRIP=""
for cand in \
    "${NDK_ROOT}/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip" \
    "${NDK_ROOT}/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-strip" \
    "${NDK_ROOT}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android-strip" \
    "${NDK_ROOT}/toolchains/llvm/prebuilt/linux-x86_64/bin/arm-linux-androideabi-strip" \
    "${NDK_ROOT}/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android-strip" \
    "${NDK_ROOT}/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android-strip"
do
    [ -x "$cand" ] && LLVM_STRIP="$cand" && break
done
[ -n "$LLVM_STRIP" ] && echo "==> strip: ${LLVM_STRIP}" || echo "==> 警告: 未找到 strip 工具，跳过 strip"

# ---------- 退出还原 Android.mk ----------
restore_android_mk() {
    if [ -f "${ANDROID_MK_BAK}" ]; then
        mv -f "${ANDROID_MK_BAK}" "${ANDROID_MK}"
        echo "==> Android.mk 已还原"
    fi
}
trap restore_android_mk EXIT

# ---------- 备份 + 插入 ----------
echo "==> 备份 Android.mk"
cp -f "${ANDROID_MK}" "${ANDROID_MK_BAK}"

echo "==> 插入 non_official_features.cpp 和 GIT_VERSION"

sed -i '0,/^LOCAL_SRC_FILES/s/^LOCAL_SRC_FILES.*/&\nLOCAL_SRC_FILES+=..\/non_official_features.cpp\nLOCAL_SRC_FILES+=..\/decode_command.cpp/' "${ANDROID_MK}"

sed -i '1s/^/override GIT_VERSION := " [Modified by aGuGu]"\nCXXFLAGS += -DGIT_VERSION="$(GIT_VERSION)"\n/' "${ANDROID_MK}"

# ---------- 编译 ----------
echo "==> 编译: ${BUILD_PLATFORMS[*]}，j=${JOBS}"
(
    cd "${JNICALL_DIR}"
    ndk-build -j${JOBS} ${APP_ABI_ARG} APP_PLATFORM=android-21
)

# ---------- 还原 ----------
mv -f "${ANDROID_MK_BAK}" "${ANDROID_MK}"
echo "==> Android.mk 已还原"

# ---------- 重命名 + strip ----------
echo "==> 重命名并 strip"
for p in "${BUILD_PLATFORMS[@]}"; do
    so="${LIBS_TARGET_PATH}/${p}/libretro.so"
    out="${LIBS_TARGET_PATH}/${p}/fbneo_advanced_libretro.so"
    if [ -f "$so" ]; then
        mv "$so" "$out"
        [ -n "$LLVM_STRIP" ] && "$LLVM_STRIP" --strip-unneeded "$out"
        echo -e "\033[33m==> 产物: ${out}\033[0m"
    else
        echo "==> ${p}: 未找到 ${so}"
    fi
done

echo "==> 完成"