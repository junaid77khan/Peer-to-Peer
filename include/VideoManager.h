#pragma once
#include <string>
#include <opencv2/opencv.hpp>

class VideoManager {
public:
    VideoManager();
    bool startVideoCapture(int deviceId = 0);
    bool stopVideoCapture();
    cv::Mat getNextFrame();
    bool isStreamActive() const;

private:
    cv::VideoCapture capture_;
    bool isStreaming_;
};