#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

void loadVideo(const std::string& fileName, const std::string& path, bool* isRunning) {
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    cv::VideoCapture cap(path + fileName);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file." << std::endl;
        return;
    }

    cv::namedWindow("Kartrider Intro", cv::WINDOW_NORMAL);

    cv::moveWindow("Kartrider Intro", location_x, location_y);
    cv::resizeWindow("Kartrider Intro", window_width, window_height);

    cv::Mat frame;
    while (isRunning) {
        cap >> frame; 
        if (frame.empty()) {
            break;
        }

        if (cv::getWindowProperty("Kartrider Intro", cv::WND_PROP_VISIBLE) < 1) {
            exit(0);
            break;
        }

        cv::moveWindow("Kartrider Intro", location_x, location_y);
        cv::resizeWindow("Kartrider Intro", window_width, window_height);

        cv::imshow("Kartrider Intro", frame);

        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
}