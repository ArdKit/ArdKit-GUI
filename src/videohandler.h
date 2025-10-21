#ifndef VIDEOHANDLER_H
#define VIDEOHANDLER_H

#include <QObject>
#include <QString>
#include <QImage>
#include "videodecoder.h"
#include "videorenderer.h"

/**
 * @brief 视频处理类
 * 负责视频流的接收、解码和渲染
 */
class VideoHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY isRecordingChanged)
    Q_PROPERTY(bool isPaused READ isPaused NOTIFY isPausedChanged)
    Q_PROPERTY(QString videoSource READ videoSource WRITE setVideoSource NOTIFY videoSourceChanged)
    Q_PROPERTY(QSize videoSize READ videoSize NOTIFY videoSizeChanged)
    Q_PROPERTY(double frameRate READ frameRate NOTIFY frameRateChanged)
    Q_PROPERTY(qint64 bitrate READ bitrate NOTIFY bitrateChanged)
    Q_PROPERTY(VideoRenderer* renderer READ renderer CONSTANT)

public:
    explicit VideoHandler(QObject *parent = nullptr);
    ~VideoHandler();

    bool isPlaying() const { return m_isPlaying; }
    bool isRecording() const { return m_isRecording; }
    bool isPaused() const { return m_isPaused; }
    QString videoSource() const { return m_videoSource; }
    QSize videoSize() const { return m_videoSize; }
    double frameRate() const { return m_frameRate; }
    qint64 bitrate() const { return m_bitrate; }
    VideoRenderer* renderer() const { return m_renderer; }

    void setVideoSource(const QString &source);

public slots:
    void setRenderer(VideoRenderer *renderer);
    void startVideo();
    void stopVideo();
    void pauseVideo();
    void resumeVideo();
    void startRecording(const QString &filePath);
    void stopRecording();
    void takeScreenshot(const QString &filePath);

signals:
    void isPlayingChanged();
    void isRecordingChanged();
    void isPausedChanged();
    void videoSourceChanged();
    void videoSizeChanged();
    void frameRateChanged();
    void bitrateChanged();
    void frameReady(const QImage &frame);
    void errorOccurred(const QString &error);

private slots:
    void onFrameReady();
    void onDecoderError(const QString &error);
    void onStreamOpened(int width, int height, double fps);
    void onStreamClosed();
    void onPacketReceived(int packetSize);

private:
    bool m_isPlaying;
    bool m_isRecording;
    bool m_isPaused;
    QString m_videoSource;
    QSize m_videoSize;
    double m_frameRate;
    qint64 m_bitrate;

    // 视频解码器
    VideoDecoder *m_decoder;

    // 视频渲染器
    VideoRenderer *m_renderer;

    // 当前帧（用于截图和录制）
    QImage m_currentFrame;

    // 码率统计
    qint64 m_totalBytes;
    qint64 m_lastBitrateTime;
};

#endif // VIDEOHANDLER_H
