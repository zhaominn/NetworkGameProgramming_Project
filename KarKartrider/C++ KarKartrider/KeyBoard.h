#pragma once
#include <glew.h>
#include <freeglut.h>
#include <glm/glm/glm.hpp>

#include <unordered_map>

#include "root.h"
#include "Camera.h"

// 전역 key 상태는 헤더에선 extern으로 선언
extern std::unordered_map<char, bool> keyState;

// 함수 원형(정의는 KeyBoard.cpp로 이동)
void keyDown_s(const char& key);
void keyUp_s(const char& key);
bool isKeyPressed_s(const char& key);

void keyUp(unsigned char key, int x, int y);
void keyDown(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void mouseClick(int button, int state, int x, int y);