#include "Pch.h"
#include "loadprogress.h"

void printProgressBar(int progress, int total) {
    const int barWidth = 50;
    float percentage = static_cast<float>(progress) / total;

    std::cout << "\r[";
    int pos = static_cast<int>(barWidth * percentage);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(percentage * 100.0) << "%";
    std::cout.flush();
}
