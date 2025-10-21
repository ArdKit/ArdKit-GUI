cmake_minimum_required(VERSION 3.16)

# 获取版本号
execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version.sh get
    OUTPUT_VARIABLE ARDKIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 解析版本号
string(REPLACE "." ";" VERSION_LIST ${ARDKIT_VERSION})
list(GET VERSION_LIST 0 ARDKIT_VERSION_MAJOR)
list(GET VERSION_LIST 1 ARDKIT_VERSION_MINOR)
list(GET VERSION_LIST 2 ARDKIT_VERSION_BUILD)

project(ArdKit-GUI VERSION ${ARDKIT_VERSION} LANGUAGES CXX)

# 防止在源码目录中构建
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "不允许在源码目录中构建！\n"
        "请创建单独的 build 目录：\n"
        "  mkdir build && cd build\n"
        "  cmake ..\n"
        "如果您已经在源码目录运行过 cmake，请先清理：\n"
        "  rm -rf CMakeCache.txt CMakeFiles/\n"
    )
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 确保所有自动生成的文件都在 build 目录中
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置 MOC、UIC、RCC 输出目录到 build 目录
set(CMAKE_AUTOMOC_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/moc")
set(CMAKE_AUTOUIC_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/uic")
set(CMAKE_AUTORCC_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rcc")

# 设置所有编译产物输出到 build 目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 查找 Qt 库
find_package(Qt6 COMPONENTS Core Gui Quick Multimedia QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Gui Quick Multimedia REQUIRED)
endif()

# 查找 FFmpeg 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED
    libavcodec
    libavformat
    libavutil
    libswscale
    libswresample
)

# 添加 FFmpeg 包含目录
include_directories(${FFMPEG_INCLUDE_DIRS})

# 生成版本头文件
string(TIMESTAMP ARDKIT_BUILD_DATE "%Y-%m-%d %H:%M:%S")

# 每次构建增加版本号
execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version.sh build
    OUTPUT_VARIABLE ARDKIT_VERSION_NEW
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 重新解析版本号
string(REPLACE "." ";" VERSION_LIST ${ARDKIT_VERSION_NEW})
list(GET VERSION_LIST 0 ARDKIT_VERSION_MAJOR)
list(GET VERSION_LIST 1 ARDKIT_VERSION_MINOR)
list(GET VERSION_LIST 2 ARDKIT_VERSION_BUILD)
set(ARDKIT_VERSION ${ARDKIT_VERSION_NEW})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
    @ONLY
)

# 添加生成的头文件目录
include_directories(${CMAKE_CURRENT_BINARY_DIR})

message(STATUS "========================================")
message(STATUS "ArdKit-GUI Configuration")
message(STATUS "========================================")
message(STATUS "Version: ${ARDKIT_VERSION}")
message(STATUS "Build date: ${ARDKIT_BUILD_DATE}")
message(STATUS "Qt version: ${Qt${QT_VERSION_MAJOR}_VERSION}")
message(STATUS "========================================")

# 源文件
set(PROJECT_SOURCES
    src/main.cpp
    src/videohandler.h
    src/videohandler.cpp
    src/connectionmanager.h
    src/connectionmanager.cpp
    src/configmanager.h
    src/configmanager.cpp
    src/messagelogger.h
    src/messagelogger.cpp
    src/videodecoder.h
    src/videodecoder.cpp
    src/videorenderer.h
    src/videorenderer.cpp
)

# 资源文件
set(PROJECT_RESOURCES
    resources.qrc
)

# 创建可执行文件
if(Qt6_FOUND)
    qt_add_executable(ArdKit-GUI
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
    )
    qt_add_qml_module(ArdKit-GUI
        URI ArdKitGUI
        VERSION 1.0
        QML_FILES
            qml/main.qml
            qml/components/CustomToolBar.qml
            qml/components/VideoDisplay.qml
            qml/components/MessageConsole.qml
            qml/components/ConnectionDialog.qml
    )
else()
    add_executable(ArdKit-GUI
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
    )
endif()

# 链接 Qt 库和 FFmpeg 库
if(Qt6_FOUND)
    target_link_libraries(ArdKit-GUI PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Multimedia
        ${FFMPEG_LIBRARIES}
    )
else()
    target_link_libraries(ArdKit-GUI PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Quick
        Qt5::Multimedia
        ${FFMPEG_LIBRARIES}
    )
endif()

# 添加 FFmpeg 库目录
target_link_directories(ArdKit-GUI PRIVATE ${FFMPEG_LIBRARY_DIRS})

# 平台特定设置
if(WIN32)
    set_target_properties(ArdKit-GUI PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set_target_properties(ArdKit-GUI PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

# 安装规则
install(TARGETS ArdKit-GUI
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
