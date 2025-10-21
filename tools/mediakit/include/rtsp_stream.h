#ifndef RTSP_STREAM_H
#define RTSP_STREAM_H

#include "stream_protocol.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
}

#include <atomic>
#include <thread>
#include <string>

/**
 * @brief RTSP流媒体服务实现
 */
class RTSPStream : public StreamProtocol {
public:
    RTSPStream();
    ~RTSPStream() override;

    bool initialize(const std::string& video_file,
                   const std::string& url,
                   bool loop = true) override;

    bool start() override;
    void stop() override;
    bool isRunning() const override;
    std::string getProtocolName() const override;
    std::string getStats() const override;

private:
    void streamingLoop();
    void cleanup();
    bool openInputFile();
    bool openOutputStream();

    std::string m_videoFile;
    std::string m_url;
    bool m_loop;
    std::atomic<bool> m_running;
    std::thread m_streamThread;

    // FFmpeg context
    AVFormatContext* m_inputFormatCtx;
    AVFormatContext* m_outputFormatCtx;
    AVCodecContext* m_codecCtx;
    int m_videoStreamIndex;

    // Statistics
    std::atomic<uint64_t> m_frameCount;
    std::atomic<uint64_t> m_bytesSent;
};

#endif // RTSP_STREAM_H
