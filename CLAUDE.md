# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

ArdKit-GUI 是一个基于 Qt Quick 的 C++ 跨平台桌面应用程序，主要用于：
- 图传显示（视频流显示）
- 配置管理
- 控制机器人与无人机

支持平台：macOS, Windows, Linux

## 构建和开发命令

### 环境要求
- Qt 5.x 或 Qt 6.x + Qt Quick 3.x（不使用 Widgets）
- C++ 编译器（GCC/Clang/MSVC）
- CMake 3.16 或更高版本

### 重要：构建目录管理

**所有构建文件必须在 build 目录中，禁止在源码目录构建！**

项目的 CMakeLists.txt 已配置为：
- **强制外部构建**：尝试在源码目录构建会报错
- **所有生成文件都在 build 目录**：包括 MOC、UIC、RCC、编译产物等
- **保持源码目录清洁**：不会污染源码目录

构建目录结构：
```
build/
├── bin/              # 可执行文件输出目录
├── lib/              # 库文件输出目录
├── moc/              # Qt MOC 生成文件
├── uic/              # Qt UIC 生成文件
├── rcc/              # Qt RCC 生成文件
├── CMakeFiles/       # CMake 内部文件
└── CMakeCache.txt    # CMake 缓存
```

### 构建项目

```bash
# 1. 创建并进入构建目录（必须在项目根目录外）
mkdir build && cd build

# 2. 配置项目（Debug）
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 或配置项目（Release）
cmake -DCMAKE_BUILD_TYPE=Release ..

# 3. 编译
cmake --build .

# 或使用多核编译（推荐）
cmake --build . -j$(nproc)  # Linux/macOS
cmake --build . -j%NUMBER_OF_PROCESSORS%  # Windows
```

### 运行应用

```bash
# 从 build 目录运行
./bin/ArdKit-GUI  # Linux/macOS
.\bin\ArdKit-GUI.exe  # Windows

# 或从项目根目录运行
./build/bin/ArdKit-GUI  # Linux/macOS
.\build\bin\ArdKit-GUI.exe  # Windows
```

### 清理构建

```bash
# 完全清理：删除整个 build 目录
rm -rf build

# 部分清理：只清理编译产物
cd build
make clean  # 或 cmake --build . --target clean
```

### 如果误在源码目录运行了 cmake

如果不小心在源码目录运行了 cmake，会看到错误提示。清理方法：

```bash
# 删除 CMake 生成的文件
rm -rf CMakeCache.txt CMakeFiles/

# 然后按正确方式构建
mkdir build && cd build
cmake ..
```

## 架构设计

### 整体架构

本项目采用 Qt Quick (QML) + C++ 混合架构：
- **前端界面**：使用 QML 构建，利用 Qt Quick 的声明式 UI 和动画能力
- **后端逻辑**：C++ 实现核心业务逻辑
- **桥接层**：通过 Qt 的 Meta-Object 系统（信号槽、Q_PROPERTY）连接 QML 和 C++

### 主窗口布局

主窗口包含以下核心区域：

1. **菜单栏**：标准应用菜单（文件、编辑、帮助等）
2. **工具栏**：快速操作按钮
   - 连接/断开
   - 录像/停止
   - 截图
   - 配置
3. **视频显示区域**
   - 尺寸：显示器的 1/2
   - 比例：16:9 或 4:3（根据视频源自适应）
4. **信息显示区**
   - 类似终端的文本输出
   - 显示机器人/无人机的实时信息
   - 默认保留最近 1000 条记录（可配置）
   - 支持滚动查看历史信息

### 关键组件

#### QML 前端
- 使用 Qt Quick Controls 2 构建 UI 组件
- 利用 Qt Quick 的布局系统（RowLayout, ColumnLayout 等）
- 使用 Qt Multimedia 或 Qt Quick 3D 处理视频显示

#### C++ 后端
- **视频处理模块**：处理视频流解码和渲染
- **通信模块**：与机器人/无人机建立连接和数据交换
- **配置管理**：读取和保存应用配置
- **录像和截图**：媒体文件保存功能

#### QML 与 C++ 交互
- 使用 `Q_PROPERTY` 暴露 C++ 属性到 QML
- 使用 `Q_INVOKABLE` 或 `public slots` 让 QML 调用 C++ 方法
- 使用 `signals` 从 C++ 通知 QML 更新 UI
- 使用 `qmlRegisterType` 注册自定义 C++ 类型到 QML

### 视频流处理

视频显示需要考虑：
- 使用 `QQuickItem` 或 `QQuickPaintedItem` 自定义视频渲染组件
- 或使用 Qt Multimedia 的 `VideoOutput` QML 组件
- 支持硬件加速解码
- 处理不同的视频编码格式

## 开发指南

### 项目结构

```
ArdKit-GUI/
├── build/            # 构建目录（.gitignore 已忽略）
│   ├── bin/          # 可执行文件
│   ├── lib/          # 库文件
│   ├── moc/          # Qt MOC 文件
│   ├── uic/          # Qt UIC 文件
│   └── rcc/          # Qt RCC 文件
├── src/              # C++ 源代码
│   ├── main.cpp      # 应用入口
│   ├── videohandler.h/cpp      # 视频处理
│   ├── connectionmanager.h/cpp # 连接管理
│   ├── configmanager.h/cpp     # 配置管理
│   └── messagelogger.h/cpp     # 日志管理
├── qml/              # QML 界面文件
│   ├── main.qml      # 主窗口
│   └── components/   # 可复用组件
│       ├── ToolBar.qml
│       ├── VideoDisplay.qml
│       └── MessageConsole.qml
├── resources/        # 资源文件
│   ├── icons/        # 图标
│   └── images/       # 图片
├── CMakeLists.txt    # 构建配置
├── resources.qrc     # Qt 资源文件
├── CLAUDE.md         # 开发指南
└── README.md         # 项目文档
```

**注意**：`build/` 目录由 CMake 自动创建，包含所有构建产物，已在 `.gitignore` 中排除。

### Qt Quick 开发注意事项

#### QML 最佳实践
- 保持 QML 文件简洁，复杂逻辑放在 C++ 中
- 使用 `Loader` 延迟加载不常用的组件
- 避免在 QML 中进行复杂计算
- 使用 `anchors` 或 Layout 进行响应式布局

#### C++ 与 QML 集成
- 所有需要在 QML 中使用的 C++ 类必须继承 `QObject`
- 使用 `Q_OBJECT` 宏启用元对象系统
- 属性更改时使用 `emit` 发送信号通知 QML
- 在 `main.cpp` 中使用 `QQmlApplicationEngine` 加载 QML

#### 性能优化
- 视频渲染使用独立线程，避免阻塞 UI 线程
- 大量数据使用 C++ 模型（`QAbstractListModel`）而不是 JavaScript 数组
- 使用 `Qt Quick Compiler` 提前编译 QML 以提升性能

#### 跨平台注意事项
- macOS：处理菜单栏的特殊行为（全局菜单栏）
- Windows：注意 DLL 依赖和部署
- Linux：测试不同桌面环境（GNOME、KDE 等）的兼容性

### 调试技巧

```bash
# 启用 QML 调试输出
export QML_IMPORT_TRACE=1
export QT_LOGGING_RULES="qt.qml.binding=true"

# 查看 QML 性能分析
export QSG_RENDER_TIMING=1
```

### 常见问题

- **QML 找不到 C++ 类型**：检查是否使用 `qmlRegisterType` 注册
- **信号槽连接失败**：确保使用了 `Q_OBJECT` 宏并重新运行 `cmake`
- **视频显示黑屏**：检查视频解码器和渲染路径是否正确配置
