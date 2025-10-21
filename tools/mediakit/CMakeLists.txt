cmake_minimum_required(VERSION 3.16)

# 获取版本号
execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version.sh get
    OUTPUT_VARIABLE MEDIAKIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

project(MediaKit VERSION ${MEDIAKIT_VERSION})

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找 FFmpeg 库
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(SWSCALE_LIBRARY swscale)

# 检查是否找到所有必需的库
if(NOT AVCODEC_LIBRARY OR NOT AVFORMAT_LIBRARY OR NOT AVUTIL_LIBRARY)
    message(FATAL_ERROR "FFmpeg libraries not found! Please install FFmpeg development packages.")
endif()

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${AVCODEC_INCLUDE_DIR}
    ${AVFORMAT_INCLUDE_DIR}
    ${AVUTIL_INCLUDE_DIR}
)

# 源文件
set(SOURCES
    src/main.cpp
    src/stream_protocol.cpp
    src/rtsp_stream.cpp
    src/rtmp_stream.cpp
)

# 生成版本头文件
string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S")

# 每次构建增加版本号
execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version.sh build
    OUTPUT_VARIABLE NEW_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
    @ONLY
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# 可执行文件
add_executable(mediakit ${SOURCES})

# 链接库
target_link_libraries(mediakit
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${SWSCALE_LIBRARY}
)

# 线程支持
find_package(Threads REQUIRED)
target_link_libraries(mediakit Threads::Threads)

# 安装规则
install(TARGETS mediakit
    RUNTIME DESTINATION bin
)

# 显示配置信息
message(STATUS "========================================")
message(STATUS "MediaKit Configuration")
message(STATUS "========================================")
message(STATUS "Version: ${NEW_VERSION}")
message(STATUS "Build date: ${BUILD_DATE}")
message(STATUS "FFmpeg libraries found:")
message(STATUS "  avcodec:  ${AVCODEC_LIBRARY}")
message(STATUS "  avformat: ${AVFORMAT_LIBRARY}")
message(STATUS "  avutil:   ${AVUTIL_LIBRARY}")
message(STATUS "  swscale:  ${SWSCALE_LIBRARY}")
message(STATUS "========================================")
