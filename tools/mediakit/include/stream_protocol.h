#ifndef STREAM_PROTOCOL_H
#define STREAM_PROTOCOL_H

#include <string>
#include <memory>

/**
 * @brief 流媒体协议接口基类
 *
 * 定义统一的流媒体服务接口，支持不同协议的实现
 */
class StreamProtocol {
public:
    virtual ~StreamProtocol() = default;

    /**
     * @brief 初始化协议服务
     * @param video_file 要播放的视频文件路径
     * @param url 推流URL（包含协议、地址、端口和路径）
     * @param loop 是否循环播放
     * @return 成功返回true，失败返回false
     */
    virtual bool initialize(const std::string& video_file,
                           const std::string& url,
                           bool loop = true) = 0;

    /**
     * @brief 启动流媒体服务
     * @return 成功返回true，失败返回false
     */
    virtual bool start() = 0;

    /**
     * @brief 停止流媒体服务
     */
    virtual void stop() = 0;

    /**
     * @brief 检查服务是否正在运行
     * @return 运行中返回true，否则返回false
     */
    virtual bool isRunning() const = 0;

    /**
     * @brief 获取协议名称
     * @return 协议名称字符串（如 "RTSP", "RTMP"）
     */
    virtual std::string getProtocolName() const = 0;

    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    virtual std::string getStats() const = 0;
};

/**
 * @brief 创建流媒体协议实例的工厂方法
 * @param protocol 协议类型 ("rtsp" 或 "rtmp")
 * @return 协议实例的智能指针
 */
std::unique_ptr<StreamProtocol> createStreamProtocol(const std::string& protocol);

#endif // STREAM_PROTOCOL_H
