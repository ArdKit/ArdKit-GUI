#!/bin/bash

# MediaKit 版本管理脚本
# 版本格式: x.y.z
# x - 主架构版本
# y - 特性版本
# z - 构建次数（每次构建自动增加）

VERSION_FILE="$(dirname "$0")/version.txt"

# 如果版本文件不存在，创建初始版本
if [ ! -f "$VERSION_FILE" ]; then
    echo "2.0.1" > "$VERSION_FILE"
fi

# 读取当前版本
CURRENT_VERSION=$(cat "$VERSION_FILE")

# 解析版本号
IFS='.' read -r MAJOR MINOR BUILD <<< "$CURRENT_VERSION"

# 根据参数决定操作
case "${1:-build}" in
    major)
        MAJOR=$((MAJOR + 1))
        MINOR=0
        BUILD=0
        ;;
    minor)
        MINOR=$((MINOR + 1))
        BUILD=0
        ;;
    build)
        BUILD=$((BUILD + 1))
        ;;
    get)
        echo "$CURRENT_VERSION"
        exit 0
        ;;
    set)
        if [ -z "$2" ]; then
            echo "Usage: $0 set <version>"
            exit 1
        fi
        echo "$2" > "$VERSION_FILE"
        echo "Version set to $2"
        exit 0
        ;;
    *)
        echo "Usage: $0 {major|minor|build|get|set <version>}"
        exit 1
        ;;
esac

# 保存新版本
NEW_VERSION="$MAJOR.$MINOR.$BUILD"
echo "$NEW_VERSION" > "$VERSION_FILE"

echo "$NEW_VERSION"
