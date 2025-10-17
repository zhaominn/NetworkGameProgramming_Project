#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "root.h"

void loadImage(const std::string& fileName, const std::string& path) {
    // 1. OpenCV
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    cv::Mat image = cv::imread(path + fileName);

    if (image.empty()) {
        std::cerr << "Error: Could not open image file." << std::endl;
        return;
    }

    cv::namedWindow("Kartrider Image", cv::WINDOW_NORMAL);

    cv::moveWindow("Kartrider Image", location_x, location_y);
    cv::resizeWindow("Kartrider Image", window_width, window_height);

    while (true) {
        if (cv::getWindowProperty("Kartrider Image", cv::WND_PROP_VISIBLE) < 1) {
            break;
        }

        cv::imshow("Kartrider Image", image);

        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cv::destroyAllWindows();
}