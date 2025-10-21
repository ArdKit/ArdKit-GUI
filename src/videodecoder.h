#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QThread>
#include <QString>
#include <QQueue>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

class VideoDecoder : public QThread
{
    Q_OBJECT

public:
    explicit VideoDecoder(QObject *parent = nullptr);
    ~VideoDecoder() override;

    // 打开视频流
    bool openStream(const QString &url);

    // 关闭视频流
    void closeStream();

    // 启动/停止解码
    void startDecoding();
    void stopDecoding();
    void pauseDecoding();
    void resumeDecoding();

    // 获取视频信息
    int videoWidth() const { return m_videoWidth; }
    int videoHeight() const { return m_videoHeight; }
    double frameRate() const { return m_frameRate; }

    // 获取最新的帧（RGB格式）
    QImage getLatestFrame();

    bool isRunning() const { return m_running; }

signals:
    void frameReady();
    void errorOccurred(const QString &error);
    void streamOpened(int width, int height, double fps);
    void streamClosed();
    void packetReceived(int packetSize);  // 新增：接收到数据包时发送大小

protected:
    void run() override;

private:
    // FFmpeg组件
    AVFormatContext *m_formatContext;
    AVCodecContext *m_codecContext;
    const AVCodec *m_codec;
    AVFrame *m_frame;
    AVFrame *m_frameRGB;
    AVPacket *m_packet;
    SwsContext *m_swsContext;

    // 视频流信息
    int m_videoStreamIndex;
    int m_videoWidth;
    int m_videoHeight;
    double m_frameRate;

    // RGB buffer
    uint8_t *m_buffer;
    int m_bufferSize;

    // 控制标志
    bool m_running;
    bool m_streamOpened;
    bool m_paused;

    // 帧缓冲
    QMutex m_frameMutex;
    QImage m_latestFrame;

    // 内部方法
    bool initFFmpeg(const QString &url);
    void cleanupFFmpeg();
    bool decodePacket();
    void convertFrameToRGB();
};

#endif // VIDEODECODER_H
