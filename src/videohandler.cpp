#include "videohandler.h"
#include <QDebug>
#include <QDateTime>

VideoHandler::VideoHandler(QObject *parent)
    : QObject(parent)
    , m_isPlaying(false)
    , m_isRecording(false)
    , m_isPaused(false)
    , m_videoSize(0, 0)
    , m_frameRate(0.0)
    , m_bitrate(0)
    , m_decoder(nullptr)
    , m_renderer(nullptr)
    , m_totalBytes(0)
    , m_lastBitrateTime(0)
{
    // 创建解码器
    m_decoder = new VideoDecoder(this);

    // 渲染器由 QML 创建并通过 setRenderer 设置

    // 连接信号
    connect(m_decoder, &VideoDecoder::frameReady, this, &VideoHandler::onFrameReady);
    connect(m_decoder, &VideoDecoder::errorOccurred, this, &VideoHandler::onDecoderError);
    connect(m_decoder, &VideoDecoder::streamOpened, this, &VideoHandler::onStreamOpened);
    connect(m_decoder, &VideoDecoder::streamClosed, this, &VideoHandler::onStreamClosed);
    connect(m_decoder, &VideoDecoder::packetReceived, this, &VideoHandler::onPacketReceived);

    qDebug() << "VideoHandler initialized";
}

VideoHandler::~VideoHandler()
{
    stopVideo();
    stopRecording();

    if (m_decoder) {
        delete m_decoder;
        m_decoder = nullptr;
    }

    // m_renderer由QML管理生命周期，不需要删除
}

void VideoHandler::setVideoSource(const QString &source)
{
    if (m_videoSource != source) {
        m_videoSource = source;
        emit videoSourceChanged();
        qDebug() << "Video source set to:" << source;
    }
}

void VideoHandler::setRenderer(VideoRenderer *renderer)
{
    if (m_renderer != renderer) {
        m_renderer = renderer;
        qDebug() << "VideoRenderer set to VideoHandler";
    }
}

void VideoHandler::startVideo()
{
    if (m_isPlaying) {
        qDebug() << "Video already playing";
        return;
    }

    if (m_videoSource.isEmpty()) {
        emit errorOccurred("No video source specified");
        return;
    }

    qDebug() << "Starting video from source:" << m_videoSource;

    // 打开视频流
    if (!m_decoder->openStream(m_videoSource)) {
        emit errorOccurred("Failed to open video stream");
        return;
    }

    // 启动解码
    m_decoder->startDecoding();

    m_isPlaying = true;
    if (m_renderer) {
        m_renderer->setPlaying(true);
    }
    emit isPlayingChanged();

    qDebug() << "Video started successfully";
}

void VideoHandler::stopVideo()
{
    if (!m_isPlaying) {
        return;
    }

    qDebug() << "Stopping video";

    // 停止录制（如果正在录制）
    if (m_isRecording) {
        stopRecording();
    }

    // 停止解码
    if (m_decoder) {
        m_decoder->stopDecoding();
        m_decoder->closeStream();
    }

    // 清除渲染器
    if (m_renderer) {
        m_renderer->setPlaying(false);
        m_renderer->clearFrame();
    }

    m_isPlaying = false;
    emit isPlayingChanged();

    qDebug() << "Video stopped";
}

void VideoHandler::pauseVideo()
{
    if (!m_isPlaying) {
        qWarning() << "Cannot pause: video not playing";
        return;
    }

    if (m_isPaused) {
        qDebug() << "Already paused";
        return;
    }

    qDebug() << "Pausing video";
    m_isPaused = true;
    emit isPausedChanged();

    if (m_decoder) {
        m_decoder->pauseDecoding();
    }
}

void VideoHandler::resumeVideo()
{
    if (!m_isPlaying) {
        qWarning() << "Cannot resume: video not playing";
        return;
    }

    if (!m_isPaused) {
        qDebug() << "Not paused";
        return;
    }

    qDebug() << "Resuming video";
    m_isPaused = false;
    emit isPausedChanged();

    if (m_decoder) {
        m_decoder->resumeDecoding();
    }
}

void VideoHandler::startRecording(const QString &filePath)
{
    if (m_isRecording) {
        qDebug() << "Already recording";
        return;
    }

    if (!m_isPlaying) {
        emit errorOccurred("Cannot record: video not playing");
        return;
    }

    // TODO: 实现录像功能
    // 这需要另一个FFmpeg编码器来编码和保存视频
    // 暂时先标记为recording状态

    m_isRecording = true;
    emit isRecordingChanged();
    qDebug() << "Recording started to:" << filePath;
    qDebug() << "Note: Recording feature not yet implemented";
}

void VideoHandler::stopRecording()
{
    if (!m_isRecording) {
        return;
    }

    // TODO: 停止录像并保存文件

    m_isRecording = false;
    emit isRecordingChanged();
    qDebug() << "Recording stopped";
}

void VideoHandler::takeScreenshot(const QString &filePath)
{
    if (!m_isPlaying) {
        emit errorOccurred("Cannot take screenshot: video not playing");
        return;
    }

    if (m_currentFrame.isNull()) {
        emit errorOccurred("No frame available for screenshot");
        return;
    }

    // 保存当前帧为图片
    QString finalPath = filePath;
    if (finalPath.isEmpty()) {
        finalPath = QString("screenshot_%1.png")
                        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    }

    if (m_currentFrame.save(finalPath)) {
        qDebug() << "Screenshot saved to:" << finalPath;
    } else {
        emit errorOccurred("Failed to save screenshot");
    }
}

void VideoHandler::onFrameReady()
{
    if (!m_decoder) {
        return;
    }

    // 获取最新帧
    m_currentFrame = m_decoder->getLatestFrame();

    if (m_currentFrame.isNull()) {
        return;
    }

    // 更新渲染器
    if (m_renderer) {
        m_renderer->updateFrame(m_currentFrame);
    }

    // 发送信号
    emit frameReady(m_currentFrame);

    // TODO: 如果正在录制，将帧写入视频文件
}

void VideoHandler::onPacketReceived(int packetSize)
{
    // 使用实际的packet大小计算码率
    m_totalBytes += packetSize;

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    // 初始化时间戳
    if (m_lastBitrateTime == 0) {
        m_lastBitrateTime = currentTime;
        return;
    }

    qint64 elapsedMs = currentTime - m_lastBitrateTime;

    // 每秒更新一次码率
    if (elapsedMs >= 1000) {
        // 计算比特率（bps = bytes * 8 / seconds）
        m_bitrate = (m_totalBytes * 8 * 1000) / elapsedMs;
        emit bitrateChanged();

        // 重置统计
        m_totalBytes = 0;
        m_lastBitrateTime = currentTime;
    }
}

void VideoHandler::onDecoderError(const QString &error)
{
    qWarning() << "Decoder error:" << error;
    emit errorOccurred(error);

    // 严重错误时停止视频
    stopVideo();
}

void VideoHandler::onStreamOpened(int width, int height, double fps)
{
    qDebug() << "Stream opened:" << width << "x" << height << "@" << fps << "fps";

    m_videoSize = QSize(width, height);
    emit videoSizeChanged();

    m_frameRate = fps;
    emit frameRateChanged();

    // 重置码率统计
    m_totalBytes = 0;
    m_bitrate = 0;
    m_lastBitrateTime = QDateTime::currentMSecsSinceEpoch();
    emit bitrateChanged();
}

void VideoHandler::onStreamClosed()
{
    qDebug() << "Stream closed";

    m_videoSize = QSize(0, 0);
    emit videoSizeChanged();

    m_frameRate = 0.0;
    emit frameRateChanged();

    m_bitrate = 0;
    emit bitrateChanged();
}
