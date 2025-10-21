#!/bin/bash

# 测试视频生成脚本
# 使用 FFmpeg 生成多种测试视频文件

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUTPUT_DIR="$SCRIPT_DIR"

# 颜色输出
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}测试视频生成工具${NC}"
echo -e "${BLUE}========================================${NC}\n"

# 检查 ffmpeg 是否安装
if ! command -v ffmpeg &> /dev/null; then
    echo -e "${YELLOW}错误: 未找到 ffmpeg 命令${NC}"
    echo "请先安装 FFmpeg："
    echo "  macOS:   brew install ffmpeg"
    echo "  Ubuntu:  sudo apt-get install ffmpeg"
    echo "  Windows: 从 https://ffmpeg.org 下载"
    exit 1
fi

# 默认参数
DURATION=30
RESOLUTION="1280x720"
FRAMERATE=30
PATTERN="testsrc"

# 显示帮助信息
show_help() {
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  -d, --duration <秒>     视频时长（默认: 30秒）"
    echo "  -r, --resolution <WxH>  分辨率（默认: 1280x720）"
    echo "  -f, --framerate <fps>   帧率（默认: 30）"
    echo "  -p, --pattern <类型>    测试图案类型（默认: testsrc）"
    echo "                          可选: testsrc, testsrc2, smptebars, color"
    echo "  -a, --all              生成所有类型的测试视频"
    echo "  -h, --help             显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0                           # 生成默认测试视频"
    echo "  $0 -d 60 -r 1920x1080       # 生成60秒的1080p视频"
    echo "  $0 -p smptebars             # 生成彩条测试图案"
    echo "  $0 --all                     # 生成所有类型测试视频"
}

# 解析命令行参数
GENERATE_ALL=false
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--duration)
            DURATION="$2"
            shift 2
            ;;
        -r|--resolution)
            RESOLUTION="$2"
            shift 2
            ;;
        -f|--framerate)
            FRAMERATE="$2"
            shift 2
            ;;
        -p|--pattern)
            PATTERN="$2"
            shift 2
            ;;
        -a|--all)
            GENERATE_ALL=true
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 生成单个测试视频
generate_video() {
    local pattern=$1
    local output_file="${OUTPUT_DIR}/test_${pattern}_${RESOLUTION}_${FRAMERATE}fps.mp4"

    echo -e "${GREEN}正在生成: ${output_file}${NC}"
    echo "  图案: $pattern"
    echo "  分辨率: $RESOLUTION"
    echo "  帧率: ${FRAMERATE} fps"
    echo "  时长: ${DURATION} 秒"

    case $pattern in
        testsrc)
            # 经典测试图案（带移动方块和时间码）
            ffmpeg -f lavfi -i "testsrc=size=${RESOLUTION}:rate=${FRAMERATE}:duration=${DURATION}" \
                   -c:v libx264 -preset medium -crf 23 -pix_fmt yuv420p \
                   -y "$output_file" 2>&1 | grep -E "frame=|time=|speed=" || true
            ;;
        testsrc2)
            # 增强测试图案（更多颜色和图案）
            ffmpeg -f lavfi -i "testsrc2=size=${RESOLUTION}:rate=${FRAMERATE}:duration=${DURATION}" \
                   -c:v libx264 -preset medium -crf 23 -pix_fmt yuv420p \
                   -y "$output_file" 2>&1 | grep -E "frame=|time=|speed=" || true
            ;;
        smptebars)
            # SMPTE 彩条
            ffmpeg -f lavfi -i "smptebars=size=${RESOLUTION}:rate=${FRAMERATE}:duration=${DURATION}" \
                   -c:v libx264 -preset medium -crf 23 -pix_fmt yuv420p \
                   -y "$output_file" 2>&1 | grep -E "frame=|time=|speed=" || true
            ;;
        color)
            # 纯色（可用于测试）
            ffmpeg -f lavfi -i "color=c=blue:s=${RESOLUTION}:r=${FRAMERATE}:d=${DURATION}" \
                   -c:v libx264 -preset medium -crf 23 -pix_fmt yuv420p \
                   -y "$output_file" 2>&1 | grep -E "frame=|time=|speed=" || true
            ;;
        mandelbrot)
            # 分形图案（动态）
            ffmpeg -f lavfi -i "mandelbrot=size=${RESOLUTION}:rate=${FRAMERATE}" \
                   -t ${DURATION} -c:v libx264 -preset medium -crf 23 -pix_fmt yuv420p \
                   -y "$output_file" 2>&1 | grep -E "frame=|time=|speed=" || true
            ;;
        *)
            echo -e "${YELLOW}未知图案类型: $pattern${NC}"
            return 1
            ;;
    esac

    if [ -f "$output_file" ]; then
        local size=$(du -h "$output_file" | cut -f1)
        echo -e "${GREEN}✓ 完成: $output_file (大小: $size)${NC}\n"
    else
        echo -e "${YELLOW}✗ 生成失败${NC}\n"
    fi
}

# 生成视频
if [ "$GENERATE_ALL" = true ]; then
    echo -e "${BLUE}生成所有类型测试视频...${NC}\n"
    for pattern in testsrc testsrc2 smptebars color mandelbrot; do
        generate_video "$pattern"
    done
else
    generate_video "$PATTERN"
fi

echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}测试视频生成完成！${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "生成的文件位于: $OUTPUT_DIR"
echo ""
echo "使用示例:"
echo "  # RTSP 推流"
echo "  ../tools/mediakit/build/bin/mediakit -p rtsp -f $OUTPUT_DIR/test_testsrc_${RESOLUTION}_${FRAMERATE}fps.mp4 -u rtsp://localhost:8554/test"
echo ""
echo "  # 使用 ffplay 播放"
echo "  ffplay $OUTPUT_DIR/test_testsrc_${RESOLUTION}_${FRAMERATE}fps.mp4"
