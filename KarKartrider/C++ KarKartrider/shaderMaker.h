#pragma once
#include <glew.h>

// 전역 변수 선언 (정의는 shaderMaker.cpp 한 곳에만 둡니다)
extern GLint width;
extern GLint height;

extern GLuint shaderProgramID;
extern GLuint vertexShader;
extern GLuint fragmentShader;

extern GLchar* vertexSource;
extern GLchar* fragmentSource;

extern GLuint shaderProgramID_UI;
extern GLuint vertexShader_UI;
extern GLuint fragmentShader_UI;

extern GLchar* vertexSource_UI;
extern GLchar* fragmentSource_UI;

// 함수 선언(구현은 shaderMaker.cpp에 있음)
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();

void make_vertexShaders_ui();
void make_fragmentShaders_ui();
void make_shaderProgram_ui();