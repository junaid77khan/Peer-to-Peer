#include "VideoManager.h"
#include <fmt/format.h>
#include <iostream>

VideoManager::VideoManager() : isStreaming_(false) {
    std::cout << "VideoManager initialized\n";
}

bool VideoManager::startVideoCapture(int deviceId) {
    try {
        if (isStreaming_) {
            std::cout << "Video capture already active\n";
            return true;
        }

        capture_.open(deviceId);
        if (!capture_.isOpened()) {
            throw std::runtime_error(fmt::format("Could not open video device {}", deviceId));
        }

        capture_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        capture_.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        capture_.set(cv::CAP_PROP_FPS, 30);

        isStreaming_ = true;
        std::cout << fmt::format("Started video capture on device {}\n", deviceId);
        return true;

    } catch (const std::exception& e) {
        std::cerr << fmt::format("Failed to start video capture: {}\n", e.what());
        return false;
    }
}

bool VideoManager::stopVideoCapture() {
    if (!isStreaming_) {
        std::cout << "Video capture already stopped\n";
        return true;
    }

    capture_.release();
    isStreaming_ = false;
    std::cout << "Stopped video capture\n";
    return true;
}

cv::Mat VideoManager::getNextFrame() {
    if (!isStreaming_) {
        throw std::runtime_error("Video capture not active");
    }

    cv::Mat frame;
    if (!capture_.read(frame)) {
        throw std::runtime_error("Failed to read frame from camera");
    }

    return frame;
}

bool VideoManager::isStreamActive() const {
    return isStreaming_;
}