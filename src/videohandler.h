#ifndef VIDEOHANDLER_H
#define VIDEOHANDLER_H

#include <QObject>
#include <QString>
#include <QImage>

/**
 * @brief 视频处理类
 * 负责视频流的接收、解码和渲染
 */
class VideoHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY isRecordingChanged)
    Q_PROPERTY(QString videoSource READ videoSource WRITE setVideoSource NOTIFY videoSourceChanged)
    Q_PROPERTY(QSize videoSize READ videoSize NOTIFY videoSizeChanged)

public:
    explicit VideoHandler(QObject *parent = nullptr);
    ~VideoHandler();

    bool isPlaying() const { return m_isPlaying; }
    bool isRecording() const { return m_isRecording; }
    QString videoSource() const { return m_videoSource; }
    QSize videoSize() const { return m_videoSize; }

    void setVideoSource(const QString &source);

public slots:
    void startVideo();
    void stopVideo();
    void startRecording(const QString &filePath);
    void stopRecording();
    void takeScreenshot(const QString &filePath);

signals:
    void isPlayingChanged();
    void isRecordingChanged();
    void videoSourceChanged();
    void videoSizeChanged();
    void frameReady(const QImage &frame);
    void errorOccurred(const QString &error);

private:
    bool m_isPlaying;
    bool m_isRecording;
    QString m_videoSource;
    QSize m_videoSize;

    // 视频解码和录制相关的私有成员
    void processVideoFrame();
};

#endif // VIDEOHANDLER_H
