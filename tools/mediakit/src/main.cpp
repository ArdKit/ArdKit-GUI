#include "stream_protocol.h"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

extern "C" {
#include <libavutil/ffversion.h>
}

// Generated version header
#include "version.h"

std::atomic<bool> g_running(true);

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
        g_running = false;
    }
}

void printUsage(const char* program_name) {
    std::cout << "MediaKit v" << MEDIAKIT_VERSION << " - Stream Media Server\n";
    std::cout << "Usage: " << program_name << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -p, --protocol <rtsp|rtmp>  Protocol to use (required)\n";
    std::cout << "  -f, --file <path>           Video file to stream (required)\n";
    std::cout << "  -u, --url <url>             Stream URL (required)\n";
    std::cout << "  -l, --loop                  Enable loop playback (default: true)\n";
    std::cout << "  -n, --no-loop               Disable loop playback\n";
    std::cout << "  -v, --version               Show version information\n";
    std::cout << "  -h, --help                  Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " -p rtsp -f video.mp4 -u rtsp://localhost:8554/stream\n";
    std::cout << "  " << program_name << " -p rtmp -f video.mp4 -u rtmp://localhost:1935/live/stream\n";
    std::cout << "  " << program_name << " --protocol rtsp --file test.mp4 --url rtsp://0.0.0.0:8554/test --no-loop\n";
}

void printVersion() {
    std::cout << "MediaKit version " << MEDIAKIT_VERSION << "\n";
    std::cout << "Build date: " << MEDIAKIT_BUILD_DATE << "\n";
    std::cout << "FFmpeg version: " << FFMPEG_VERSION << "\n";
}

int main(int argc, char* argv[]) {
    std::string protocol;
    std::string video_file;
    std::string url;
    bool loop = true;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-p" || arg == "--protocol") {
            if (i + 1 < argc) {
                protocol = argv[++i];
            } else {
                std::cerr << "Error: --protocol requires an argument\n";
                return 1;
            }
        } else if (arg == "-f" || arg == "--file") {
            if (i + 1 < argc) {
                video_file = argv[++i];
            } else {
                std::cerr << "Error: --file requires an argument\n";
                return 1;
            }
        } else if (arg == "-u" || arg == "--url") {
            if (i + 1 < argc) {
                url = argv[++i];
            } else {
                std::cerr << "Error: --url requires an argument\n";
                return 1;
            }
        } else if (arg == "-l" || arg == "--loop") {
            loop = true;
        } else if (arg == "-n" || arg == "--no-loop") {
            loop = false;
        } else {
            std::cerr << "Error: Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // Validate required arguments
    if (protocol.empty()) {
        std::cerr << "Error: Protocol is required (use -p or --protocol)\n";
        printUsage(argv[0]);
        return 1;
    }

    if (video_file.empty()) {
        std::cerr << "Error: Video file is required (use -f or --file)\n";
        printUsage(argv[0]);
        return 1;
    }

    if (url.empty()) {
        std::cerr << "Error: Stream URL is required (use -u or --url)\n";
        printUsage(argv[0]);
        return 1;
    }

    // Print banner
    std::cout << "========================================\n";
    std::cout << "MediaKit v" << MEDIAKIT_VERSION << "\n";
    std::cout << "========================================\n\n";

    // Create stream protocol instance
    auto stream = createStreamProtocol(protocol);
    if (!stream) {
        std::cerr << "Error: Unsupported protocol: " << protocol << "\n";
        std::cerr << "Supported protocols: rtsp, rtmp\n";
        return 1;
    }

    std::cout << "Protocol: " << stream->getProtocolName() << "\n";
    std::cout << "Video file: " << video_file << "\n";
    std::cout << "Stream URL: " << url << "\n";
    std::cout << "Loop mode: " << (loop ? "enabled" : "disabled") << "\n\n";

    // Initialize stream
    if (!stream->initialize(video_file, url, loop)) {
        std::cerr << "Error: Failed to initialize stream\n";
        return 1;
    }

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Start streaming
    if (!stream->start()) {
        std::cerr << "Error: Failed to start stream\n";
        return 1;
    }

    std::cout << "Streaming started. Press Ctrl+C to stop.\n\n";

    // Main loop - wait for signal
    while (g_running && stream->isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Stop stream
    std::cout << "\nStopping stream...\n";
    stream->stop();

    std::cout << "Final statistics: " << stream->getStats() << "\n";
    std::cout << "MediaKit stopped.\n";

    return 0;
}
