#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

void loadVideo(const std::string& fileName, const std::string& path, bool* isRunning) {
    // 1. OpenCV ���� ���
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    // 2. ������ ���� ����
    cv::VideoCapture cap(path + fileName);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file." << std::endl;
        return;
    }

    // 4. ������ ���� (â �̸� ����)
    cv::namedWindow("Kartrider Intro", cv::WINDOW_NORMAL);

    // 5. â ��ġ �� ũ�� ����
    cv::moveWindow("Kartrider Intro", location_x, location_y);
    cv::resizeWindow("Kartrider Intro", window_width, window_height);

    // 6. ������ ��� ����
    cv::Mat frame;
    while (isRunning) {
        cap >> frame; // ������ �б�
        if (frame.empty()) {
            break; // ������ ��
        }

        // â ���� Ȯ��: â�� ������ ���� ����
        if (cv::getWindowProperty("Kartrider Intro", cv::WND_PROP_VISIBLE) < 1) {
            exit(0);
            break;
        }

        cv::moveWindow("Kartrider Intro", location_x, location_y);
        cv::resizeWindow("Kartrider Intro", window_width, window_height);

        // ������ ȭ�鿡 ���
        cv::imshow("Kartrider Intro", frame);

        // ESC Ű�� ������ ����
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    // 7. �ڿ� ����
    cap.release();
    cv::destroyAllWindows();
}