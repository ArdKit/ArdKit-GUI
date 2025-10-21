#include "rtmp_stream.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

RTMPStream::RTMPStream()
    : m_loop(true)
    , m_running(false)
    , m_inputFormatCtx(nullptr)
    , m_outputFormatCtx(nullptr)
    , m_codecCtx(nullptr)
    , m_videoStreamIndex(-1)
    , m_frameCount(0)
    , m_bytesSent(0)
{
}

RTMPStream::~RTMPStream() {
    stop();
    cleanup();
}

bool RTMPStream::initialize(const std::string& video_file,
                            const std::string& url,
                            bool loop) {
    m_videoFile = video_file;
    m_url = url;
    m_loop = loop;

    std::cout << "[RTMP] Initializing with video: " << m_videoFile << std::endl;
    std::cout << "[RTMP] Stream URL: " << m_url << std::endl;
    std::cout << "[RTMP] Loop mode: " << (m_loop ? "enabled" : "disabled") << std::endl;

    return true;
}

bool RTMPStream::start() {
    if (m_running) {
        std::cerr << "[RTMP] Already running" << std::endl;
        return false;
    }

    m_running = true;
    m_streamThread = std::thread(&RTMPStream::streamingLoop, this);

    std::cout << "[RTMP] Streaming started" << std::endl;
    return true;
}

void RTMPStream::stop() {
    if (!m_running) {
        return;
    }

    std::cout << "[RTMP] Stopping stream..." << std::endl;
    m_running = false;

    if (m_streamThread.joinable()) {
        m_streamThread.join();
    }

    cleanup();
    std::cout << "[RTMP] Stream stopped" << std::endl;
}

bool RTMPStream::isRunning() const {
    return m_running;
}

std::string RTMPStream::getProtocolName() const {
    return "RTMP";
}

std::string RTMPStream::getStats() const {
    std::ostringstream oss;
    oss << "Frames: " << m_frameCount
        << ", Bytes: " << m_bytesSent
        << " (" << (m_bytesSent / 1024.0 / 1024.0) << " MB)";
    return oss.str();
}

bool RTMPStream::openInputFile() {
    int ret = avformat_open_input(&m_inputFormatCtx, m_videoFile.c_str(), nullptr, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, sizeof(errbuf));
        std::cerr << "[RTMP] Failed to open input file: " << errbuf << std::endl;
        return false;
    }

    ret = avformat_find_stream_info(m_inputFormatCtx, nullptr);
    if (ret < 0) {
        std::cerr << "[RTMP] Failed to find stream info" << std::endl;
        return false;
    }

    // Find video stream
    m_videoStreamIndex = -1;
    for (unsigned int i = 0; i < m_inputFormatCtx->nb_streams; i++) {
        if (m_inputFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }

    if (m_videoStreamIndex == -1) {
        std::cerr << "[RTMP] No video stream found" << std::endl;
        return false;
    }

    std::cout << "[RTMP] Input file opened successfully" << std::endl;
    av_dump_format(m_inputFormatCtx, 0, m_videoFile.c_str(), 0);

    return true;
}

bool RTMPStream::openOutputStream() {
    int ret = avformat_alloc_output_context2(&m_outputFormatCtx, nullptr, "flv", m_url.c_str());
    if (ret < 0 || !m_outputFormatCtx) {
        std::cerr << "[RTMP] Failed to create output context" << std::endl;
        return false;
    }

    // Copy stream from input
    AVStream* in_stream = m_inputFormatCtx->streams[m_videoStreamIndex];
    AVStream* out_stream = avformat_new_stream(m_outputFormatCtx, nullptr);
    if (!out_stream) {
        std::cerr << "[RTMP] Failed to create output stream" << std::endl;
        return false;
    }

    ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
    if (ret < 0) {
        std::cerr << "[RTMP] Failed to copy codec parameters" << std::endl;
        return false;
    }

    out_stream->codecpar->codec_tag = 0;

    std::cout << "[RTMP] Output stream configured" << std::endl;
    av_dump_format(m_outputFormatCtx, 0, m_url.c_str(), 1);

    // Open output
    if (!(m_outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&m_outputFormatCtx->pb, m_url.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE];
            av_strerror(ret, errbuf, sizeof(errbuf));
            std::cerr << "[RTMP] Failed to open output: " << errbuf << std::endl;
            return false;
        }
    }

    ret = avformat_write_header(m_outputFormatCtx, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, sizeof(errbuf));
        std::cerr << "[RTMP] Failed to write header: " << errbuf << std::endl;
        return false;
    }

    std::cout << "[RTMP] Output opened successfully" << std::endl;
    return true;
}

void RTMPStream::streamingLoop() {
    do {
        // Reset state for new loop iteration
        cleanup();
        m_frameCount = 0;

        // Open input file
        if (!openInputFile()) {
            std::cerr << "[RTMP] Failed to open input file, stopping" << std::endl;
            m_running = false;
            break;
        }

        // Open output stream
        if (!openOutputStream()) {
            std::cerr << "[RTMP] Failed to open output stream, stopping" << std::endl;
            m_running = false;
            break;
        }

        AVPacket* packet = av_packet_alloc();
        if (!packet) {
            std::cerr << "[RTMP] Failed to allocate packet" << std::endl;
            m_running = false;
            break;
        }

        AVStream* in_stream = m_inputFormatCtx->streams[m_videoStreamIndex];
        AVStream* out_stream = m_outputFormatCtx->streams[0];

        int64_t start_time = av_gettime();
        int64_t pts_offset = 0;

        // Streaming loop
        while (m_running) {
            int ret = av_read_frame(m_inputFormatCtx, packet);
            if (ret < 0) {
                if (ret == AVERROR_EOF) {
                    std::cout << "[RTMP] End of file reached" << std::endl;
                    av_packet_unref(packet);
                    break;
                } else {
                    char errbuf[AV_ERROR_MAX_STRING_SIZE];
                    av_strerror(ret, errbuf, sizeof(errbuf));
                    std::cerr << "[RTMP] Read frame error: " << errbuf << std::endl;
                    av_packet_unref(packet);
                    break;
                }
            }

            if (packet->stream_index != m_videoStreamIndex) {
                av_packet_unref(packet);
                continue;
            }

            // Timing control
            AVRational time_base = in_stream->time_base;
            int64_t pts = packet->pts != AV_NOPTS_VALUE ? packet->pts : packet->dts;

            if (pts != AV_NOPTS_VALUE) {
                int64_t now = av_gettime() - start_time;
                int64_t dts = av_rescale_q(pts - pts_offset, time_base, AV_TIME_BASE_Q);

                if (dts > now) {
                    av_usleep(dts - now);
                }
            }

            // Rescale timestamps
            packet->pts = av_rescale_q_rnd(packet->pts, in_stream->time_base,
                                          out_stream->time_base,
                                          static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            packet->dts = av_rescale_q_rnd(packet->dts, in_stream->time_base,
                                          out_stream->time_base,
                                          static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            packet->duration = av_rescale_q(packet->duration, in_stream->time_base,
                                           out_stream->time_base);
            packet->pos = -1;
            packet->stream_index = 0;

            // Write packet
            ret = av_interleaved_write_frame(m_outputFormatCtx, packet);
            if (ret < 0) {
                char errbuf[AV_ERROR_MAX_STRING_SIZE];
                av_strerror(ret, errbuf, sizeof(errbuf));
                std::cerr << "[RTMP] Write frame error: " << errbuf << std::endl;
                av_packet_unref(packet);
                break;
            }

            m_frameCount++;
            m_bytesSent += packet->size;

            if (m_frameCount % 300 == 0) {  // Print every 10 seconds at 30fps
                std::cout << "[RTMP] " << getStats() << std::endl;
            }

            av_packet_unref(packet);
        }

        av_packet_free(&packet);

        if (m_outputFormatCtx && m_outputFormatCtx->pb) {
            av_write_trailer(m_outputFormatCtx);
        }

        std::cout << "[RTMP] Loop iteration completed" << std::endl;

    } while (m_loop && m_running);

    std::cout << "[RTMP] Streaming loop exited" << std::endl;
}

void RTMPStream::cleanup() {
    if (m_outputFormatCtx) {
        if (m_outputFormatCtx->pb && !(m_outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&m_outputFormatCtx->pb);
        }
        avformat_free_context(m_outputFormatCtx);
        m_outputFormatCtx = nullptr;
    }

    if (m_inputFormatCtx) {
        avformat_close_input(&m_inputFormatCtx);
        m_inputFormatCtx = nullptr;
    }

    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
    }

    m_videoStreamIndex = -1;
}
