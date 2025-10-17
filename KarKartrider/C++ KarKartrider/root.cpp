#include "root.h"
#include "Camera.h" // 전역 카메라 정의를 위해 포함

// 전역 모델/게임 상태 — 이미 있던 정의 유지
std::vector<Model*> karts;
std::vector<Model*> road1;
std::vector<Model*> road1_barricate;
std::vector<Model*> road2;
std::vector<Model*> road2_barricate;
std::vector<Model*> selectMaps;
std::vector<Model*> character;
std::vector<Model*> countDown;
std::vector<Model*> pause;
std::vector<Model*> booster_uis;
std::vector<Model*> dashBoards;

ModeManager MM;

int window_width = 980;
int window_height = 780;
int location_x = 0;
int location_y = 0;
float volume = 0.5f;

bool isGameRunning = false;
bool isGameRunning2 = false;
bool bb_status = false;

// 전역 카메라/뷰포트 실제 정의 (헤더에는 extern만 둠)
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

GLvoid Reshape(int w, int h) {
    glViewport(location_x, location_y, w, h);
    // width/height는 shaderMaker.cpp에서 정의된 전역입니다
    width = w;
    height = h;
}