#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "root.h"

void loadImage(const std::string& fileName, const std::string& path) {
    // 1. OpenCV ���� ���
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    // 2. �̹��� ���� ����
    cv::Mat image = cv::imread(path + fileName);

    if (image.empty()) {
        std::cerr << "Error: Could not open image file." << std::endl;
        return;
    }

    // 3. ������ ���� (â �̸� ����)
    cv::namedWindow("Kartrider Image", cv::WINDOW_NORMAL);

    // 4. â ��ġ �� ũ�� ����
    cv::moveWindow("Kartrider Image", location_x, location_y);
    cv::resizeWindow("Kartrider Image", window_width, window_height);

    // 5. �̹��� ��� ����
    while (true) {
        // â ���� Ȯ��: â�� ������ ���� ����
        if (cv::getWindowProperty("Kartrider Image", cv::WND_PROP_VISIBLE) < 1) {
            break;
        }

        // �̹��� ȭ�鿡 ���
        cv::imshow("Kartrider Image", image);

        // ESC Ű�� ������ ����
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    // 6. ������ �ݱ�
    cv::destroyAllWindows();
}