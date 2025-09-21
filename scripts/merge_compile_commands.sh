#!/bin/bash
set -e

# 需要合并的编译数据库
APP=build/application/compile_commands.json
# BOOT=build/bootloader/compile_commands.json
OUT=build/compile_commands.json

if [ ! -f "$APP" ]; then
  echo "❌ 没有找到 $APP"
  exit 1
fi

# if [ ! -f "$BOOT" ]; then
#   echo "❌ 没有找到 $BOOT"
#   exit 1
# fi

# 用 jq 合并两个数组，写到项目根目录
# jq -s '.[0] + .[1]' "$APP" "$BOOT" > "$OUT"
jq -s '.[0] + .[1]' "$APP" > "$OUT"

echo "✅ 已生成合并后的 $OUT"