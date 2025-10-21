# MediaKit

MediaKit 是一个基于 FFmpeg 的轻量级流媒体服务器工具，支持 RTSP 和 RTMP 协议的循环播放。

## 版本管理

MediaKit 使用 `x.y.z` 版本格式：
- `x` - 主架构版本
- `y` - 特性版本
- `z` - 构建次数（每次构建自动递增）

当前版本：**2.0.1**

## 特性

- ✅ 支持 RTSP 协议推流
- ✅ 支持 RTMP 协议推流
- ✅ 循环播放视频文件
- ✅ 可扩展的协议接口设计
- ✅ 自动版本管理
- ✅ 命令行界面

## 依赖

- CMake 3.16+
- C++17 编译器
- FFmpeg 库（libavcodec, libavformat, libavutil, libswscale）

## 编译

```bash
# 创建构建目录
mkdir build && cd build

# 配置
cmake ..

# 编译
make -j$(nproc)

# 可执行文件位于 build/bin/mediakit
```

## 使用方法

### RTSP 推流

```bash
./bin/mediakit -p rtsp -f /path/to/video.mp4 -u rtsp://localhost:8554/stream
```

### RTMP 推流

```bash
./bin/mediakit -p rtmp -f /path/to/video.mp4 -u rtmp://localhost:1935/live/stream
```

### 命令行参数

```
Usage: mediakit [options]

Options:
  -p, --protocol <rtsp|rtmp>  Protocol to use (required)
  -f, --file <path>           Video file to stream (required)
  -u, --url <url>             Stream URL (required)
  -l, --loop                  Enable loop playback (default: true)
  -n, --no-loop               Disable loop playback
  -v, --version               Show version information
  -h, --help                  Show this help message
```

## 架构设计

MediaKit 采用面向接口的设计模式：

```
StreamProtocol (接口)
├── RTSPStream (RTSP 实现)
└── RTMPStream (RTMP 实现)
```

这种设计便于后续添加自定义协议。

## 目录结构

```
tools/mediakit/
├── CMakeLists.txt          # 构建配置
├── README.md               # 本文件
├── include/                # 头文件
│   ├── stream_protocol.h  # 协议接口
│   ├── rtsp_stream.h      # RTSP 实现
│   ├── rtmp_stream.h      # RTMP 实现
│   └── version.h.in       # 版本模板
├── src/                    # 源文件
│   ├── main.cpp
│   ├── stream_protocol.cpp
│   ├── rtsp_stream.cpp
│   └── rtmp_stream.cpp
└── scripts/                # 脚本
    ├── version.sh         # 版本管理脚本
    └── version.txt        # 版本号存储
```

## 扩展自定义协议

要添加新协议（如私有协议），按以下步骤：

1. 创建新的协议头文件（如 `custom_stream.h`）继承 `StreamProtocol`
2. 实现所有虚函数
3. 在 `stream_protocol.cpp` 的工厂方法中注册新协议
4. 更新 CMakeLists.txt 添加新的源文件

## 许可证

Apache 2.0 License
