#include "videodecoder.h"
#include <QDebug>

VideoDecoder::VideoDecoder(QObject *parent)
    : QThread(parent)
    , m_formatContext(nullptr)
    , m_codecContext(nullptr)
    , m_codec(nullptr)
    , m_frame(nullptr)
    , m_frameRGB(nullptr)
    , m_packet(nullptr)
    , m_swsContext(nullptr)
    , m_videoStreamIndex(-1)
    , m_videoWidth(0)
    , m_videoHeight(0)
    , m_frameRate(0.0)
    , m_buffer(nullptr)
    , m_bufferSize(0)
    , m_running(false)
    , m_streamOpened(false)
    , m_paused(false)
{
    qDebug() << "VideoDecoder created";
}

VideoDecoder::~VideoDecoder()
{
    stopDecoding();
    closeStream();
    qDebug() << "VideoDecoder destroyed";
}

bool VideoDecoder::openStream(const QString &url)
{
    if (m_streamOpened) {
        qWarning() << "Stream already opened";
        return false;
    }

    qDebug() << "Opening stream:" << url;

    if (!initFFmpeg(url)) {
        emit errorOccurred("Failed to initialize FFmpeg");
        return false;
    }

    m_streamOpened = true;
    emit streamOpened(m_videoWidth, m_videoHeight, m_frameRate);

    qDebug() << "Stream opened successfully:"
             << m_videoWidth << "x" << m_videoHeight
             << "@" << m_frameRate << "fps";

    return true;
}

void VideoDecoder::closeStream()
{
    if (!m_streamOpened) {
        return;
    }

    qDebug() << "Closing stream";

    stopDecoding();
    cleanupFFmpeg();

    m_streamOpened = false;
    emit streamClosed();
}

void VideoDecoder::startDecoding()
{
    if (!m_streamOpened) {
        qWarning() << "Cannot start decoding: stream not opened";
        return;
    }

    if (m_running) {
        qWarning() << "Decoding already running";
        return;
    }

    qDebug() << "Starting decoding thread";
    m_running = true;
    start();
}

void VideoDecoder::stopDecoding()
{
    if (!m_running) {
        return;
    }

    qDebug() << "Stopping decoding thread";
    m_running = false;
    m_paused = false;

    // 等待线程结束
    if (!wait(3000)) {
        qWarning() << "Decoding thread did not stop in time, terminating";
        terminate();
        wait();
    }
}

void VideoDecoder::pauseDecoding()
{
    if (!m_running) {
        qWarning() << "Cannot pause: decoding not running";
        return;
    }

    qDebug() << "Pausing decoding";
    m_paused = true;
}

void VideoDecoder::resumeDecoding()
{
    if (!m_running) {
        qWarning() << "Cannot resume: decoding not running";
        return;
    }

    qDebug() << "Resuming decoding";
    m_paused = false;
}

QImage VideoDecoder::getLatestFrame()
{
    QMutexLocker locker(&m_frameMutex);
    return m_latestFrame;
}

void VideoDecoder::run()
{
    qDebug() << "Decoding thread started";

    int errorCount = 0;
    const int maxConsecutiveErrors = 10;

    while (m_running) {
        // 如果暂停，等待并继续循环
        if (m_paused) {
            msleep(100);
            continue;
        }

        // 读取packet
        int ret = av_read_frame(m_formatContext, m_packet);

        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                // 对于实时流，EOF 可能是暂时的，等待后重试
                qDebug() << "Temporary end of stream, retrying...";
                msleep(100);  // 等待 100ms
                errorCount++;
                if (errorCount > maxConsecutiveErrors) {
                    qWarning() << "Too many consecutive EOF errors, stopping";
                    emit errorOccurred("Stream ended or connection lost");
                    break;
                }
                continue;
            } else {
                char errbuf[AV_ERROR_MAX_STRING_SIZE];
                av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
                qWarning() << "Error reading frame:" << errbuf;
                emit errorOccurred(QString("Read frame error: %1").arg(errbuf));

                // 对于网络错误，尝试重试
                msleep(100);
                errorCount++;
                if (errorCount > maxConsecutiveErrors) {
                    qWarning() << "Too many consecutive errors, stopping";
                    break;
                }
                continue;
            }
        }

        // 成功读取，重置错误计数
        errorCount = 0;

        // 只处理视频流的packet
        if (m_packet->stream_index == m_videoStreamIndex) {
            // 发送packet大小用于码率计算
            emit packetReceived(m_packet->size);

            if (decodePacket()) {
                // 转换为RGB并发送信号
                convertFrameToRGB();
                emit frameReady();
            }
        }

        av_packet_unref(m_packet);

        // 控制帧率，避免解码太快
        msleep(1000 / (m_frameRate > 0 ? m_frameRate : 30));
    }

    qDebug() << "Decoding thread stopped";
}

bool VideoDecoder::initFFmpeg(const QString &url)
{
    qDebug() << "=== VideoDecoder::initFFmpeg ===";
    qDebug() << "URL:" << url;

    // 分配format context
    m_formatContext = avformat_alloc_context();
    if (!m_formatContext) {
        qCritical() << "Failed to allocate format context";
        return false;
    }

    qDebug() << "Format context allocated successfully";

    // 打开输入流
    AVDictionary *options = nullptr;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);  // RTSP使用TCP传输
    av_dict_set(&options, "max_delay", "500000", 0);    // 最大延迟500ms
    av_dict_set(&options, "timeout", "5000000", 0);     // 超时5秒

    qDebug() << "Opening input with options: rtsp_transport=tcp, max_delay=500000, timeout=5000000";

    int ret = avformat_open_input(&m_formatContext, url.toUtf8().constData(), nullptr, &options);
    av_dict_free(&options);

    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
        qCritical() << "Failed to open input:" << errbuf << "(error code:" << ret << ")";
        qCritical() << "URL was:" << url;
        avformat_free_context(m_formatContext);
        m_formatContext = nullptr;
        return false;
    }

    qDebug() << "Input opened successfully";

    // 获取流信息
    qDebug() << "Finding stream info...";
    ret = avformat_find_stream_info(m_formatContext, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
        qCritical() << "Failed to find stream info:" << errbuf;
        cleanupFFmpeg();
        return false;
    }

    qDebug() << "Found" << m_formatContext->nb_streams << "streams";

    // 查找视频流
    m_videoStreamIndex = -1;
    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        AVMediaType type = m_formatContext->streams[i]->codecpar->codec_type;
        qDebug() << "Stream" << i << "type:" << av_get_media_type_string(type);
        if (type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }

    if (m_videoStreamIndex == -1) {
        qCritical() << "No video stream found in" << m_formatContext->nb_streams << "streams";
        cleanupFFmpeg();
        return false;
    }

    qDebug() << "Video stream found at index:" << m_videoStreamIndex;

    // 获取codec parameters
    AVCodecParameters *codecParams = m_formatContext->streams[m_videoStreamIndex]->codecpar;

    // 查找decoder
    m_codec = avcodec_find_decoder(codecParams->codec_id);
    if (!m_codec) {
        qCritical() << "Codec not found";
        cleanupFFmpeg();
        return false;
    }

    // 分配codec context
    m_codecContext = avcodec_alloc_context3(m_codec);
    if (!m_codecContext) {
        qCritical() << "Failed to allocate codec context";
        cleanupFFmpeg();
        return false;
    }

    // 复制codec parameters到context
    ret = avcodec_parameters_to_context(m_codecContext, codecParams);
    if (ret < 0) {
        qCritical() << "Failed to copy codec parameters";
        cleanupFFmpeg();
        return false;
    }

    // 打开codec
    ret = avcodec_open2(m_codecContext, m_codec, nullptr);
    if (ret < 0) {
        qCritical() << "Failed to open codec";
        cleanupFFmpeg();
        return false;
    }

    // 获取视频信息
    m_videoWidth = m_codecContext->width;
    m_videoHeight = m_codecContext->height;

    AVRational fps = m_formatContext->streams[m_videoStreamIndex]->avg_frame_rate;
    if (fps.num && fps.den) {
        m_frameRate = static_cast<double>(fps.num) / fps.den;
    } else {
        m_frameRate = 30.0;  // 默认30fps
    }

    // 分配frame
    m_frame = av_frame_alloc();
    m_frameRGB = av_frame_alloc();
    if (!m_frame || !m_frameRGB) {
        qCritical() << "Failed to allocate frames";
        cleanupFFmpeg();
        return false;
    }

    // 分配RGB buffer
    m_bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_videoWidth, m_videoHeight, 1);
    m_buffer = (uint8_t *)av_malloc(m_bufferSize);
    if (!m_buffer) {
        qCritical() << "Failed to allocate buffer";
        cleanupFFmpeg();
        return false;
    }

    // 填充frameRGB
    av_image_fill_arrays(m_frameRGB->data, m_frameRGB->linesize,
                         m_buffer, AV_PIX_FMT_RGB24,
                         m_videoWidth, m_videoHeight, 1);

    // 创建sws context用于格式转换
    m_swsContext = sws_getContext(
        m_videoWidth, m_videoHeight, m_codecContext->pix_fmt,
        m_videoWidth, m_videoHeight, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    if (!m_swsContext) {
        qCritical() << "Failed to create sws context";
        cleanupFFmpeg();
        return false;
    }

    // 分配packet
    m_packet = av_packet_alloc();
    if (!m_packet) {
        qCritical() << "Failed to allocate packet";
        cleanupFFmpeg();
        return false;
    }

    qDebug() << "FFmpeg initialized successfully";
    return true;
}

void VideoDecoder::cleanupFFmpeg()
{
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }

    if (m_swsContext) {
        sws_freeContext(m_swsContext);
        m_swsContext = nullptr;
    }

    if (m_buffer) {
        av_free(m_buffer);
        m_buffer = nullptr;
    }

    if (m_frameRGB) {
        av_frame_free(&m_frameRGB);
        m_frameRGB = nullptr;
    }

    if (m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    if (m_codecContext) {
        avcodec_free_context(&m_codecContext);
        m_codecContext = nullptr;
    }

    if (m_formatContext) {
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
    }

    m_videoStreamIndex = -1;
    m_videoWidth = 0;
    m_videoHeight = 0;
    m_frameRate = 0.0;
}

bool VideoDecoder::decodePacket()
{
    // 发送packet到decoder
    int ret = avcodec_send_packet(m_codecContext, m_packet);
    if (ret < 0) {
        qWarning() << "Error sending packet to decoder";
        return false;
    }

    // 从decoder接收frame
    ret = avcodec_receive_frame(m_codecContext, m_frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return false;
    } else if (ret < 0) {
        qWarning() << "Error receiving frame from decoder";
        return false;
    }

    return true;
}

void VideoDecoder::convertFrameToRGB()
{
    // 转换颜色空间从YUV到RGB
    sws_scale(m_swsContext,
              m_frame->data, m_frame->linesize,
              0, m_videoHeight,
              m_frameRGB->data, m_frameRGB->linesize);

    // 创建QImage
    QImage image(m_frameRGB->data[0],
                 m_videoWidth, m_videoHeight,
                 m_frameRGB->linesize[0],
                 QImage::Format_RGB888);

    // 复制到成员变量（因为m_frameRGB的数据会被重用）
    QMutexLocker locker(&m_frameMutex);
    m_latestFrame = image.copy();
}
