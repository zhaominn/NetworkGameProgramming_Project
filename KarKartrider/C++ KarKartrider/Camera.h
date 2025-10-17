#pragma once
#include <glm/glm/glm.hpp>

// 전역 카메라 변수들 선언(정의는 한 cpp 파일에만 둡니다)
extern glm::vec3 cameraPos;
extern glm::vec3 cameraDirection;
extern glm::vec3 cameraUp;
extern glm::mat4 projection;
extern glm::mat4 view;