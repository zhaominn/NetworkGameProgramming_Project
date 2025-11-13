#pragma once
#include <glew.h>
#include <freeglut.h>
#include "Mode.h" 


class LogoMode : public Mode {
public:
    bool isRunning;

    LogoMode() : isRunning(true) {}
    ~LogoMode() {
        delete this;
    }

    void init() override;

    void mouseClick(int button, int state, int x, int y) override {}

    void keyboard(unsigned char key, int x, int y) override {}

    void specialKey(int key, int x, int y) override {}

    void specialKeyUp(int key, int x, int y) override {}

    void draw_model() override {}

    void draw_bb() override {}

    void finish() override {

    }

private:
    void runVideo();

    void runSound();
};