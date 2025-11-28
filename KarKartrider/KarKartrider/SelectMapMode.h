#pragma once

#include <iostream>
#include <thread>
#include <random>
#include <freeglut.h>
#include "root.h"
#include "KeyBoard.h"
#include "LoadSound.h"

// ?꾩뿭 蹂???좎뼵 (留곸빱 異⑸룎 諛⑹?)
extern bool isAnimating;
extern float animationSpeed;
extern glm::vec3 cameraPosMapMode;
extern glm::vec3 targetCameraPos;

void SelectMap_timer(int value);

class SelectMapMode : public Mode {
public:
    int map_num = 1;

    glm::vec3 map1CamerPos = glm::vec3(0.0, 0.0, 5.0);
    glm::vec3 map2CamerPos = glm::vec3(2.5, 0.0, 5.0);

    glm::vec3 cameraDirectionMapMode = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 cameraUpMapMode = glm::vec3(0.0, 1.0, 0.0);
    glm::mat4 projectionMapMode = glm::mat4(1.0f);
    glm::mat4 viewMapMode = glm::mat4(1.0f);

    bool isSoundRunning;
    std::thread soundThread;

    bool isclickRunning;

    SelectMapMode() :isSoundRunning(true), isclickRunning(true) {}
    ~SelectMapMode() {}

    void goSelectMode();
    void init() override;
    void mouseClick(int button, int state, int x, int y) override;
    void passiveMotion(int x, int y) override {}
    void keyboard(unsigned char key, int x, int y) override;
    void updateTargetCameraPos();
    void specialKey(int key, int x, int y) override;
    void specialKeyUp(int key, int x, int y) override;
    void draw_model() override;
    void draw_bb() override;
    void finish() override;

    virtual ModeType GetModeType() const override {
        return ModeType::SELECT_MAP;
    }
private:
    void runSound();
    void clickSound();
};