cmake_minimum_required(VERSION 3.16)

project(ArdKit-GUI VERSION 1.0.0 LANGUAGES CXX)

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

# 链接 Qt 库
if(Qt6_FOUND)
    target_link_libraries(ArdKit-GUI PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Multimedia
    )
else()
    target_link_libraries(ArdKit-GUI PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Quick
        Qt5::Multimedia
    )
endif()

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
