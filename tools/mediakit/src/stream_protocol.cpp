#include "stream_protocol.h"
#include "rtsp_stream.h"
#include "rtmp_stream.h"

#include <algorithm>
#include <cctype>

std::unique_ptr<StreamProtocol> createStreamProtocol(const std::string& protocol) {
    // 转换为小写进行比较
    std::string lowerProtocol = protocol;
    std::transform(lowerProtocol.begin(), lowerProtocol.end(), lowerProtocol.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if (lowerProtocol == "rtsp") {
        return std::make_unique<RTSPStream>();
    } else if (lowerProtocol == "rtmp") {
        return std::make_unique<RTMPStream>();
    }

    return nullptr;
}
