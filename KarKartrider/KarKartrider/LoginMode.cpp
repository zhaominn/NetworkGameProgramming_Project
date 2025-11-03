#include "Pch.h"
#include "LoginMode.h"

LoginMode::LoginMode()
{
}

LoginMode::~LoginMode()
{
	delete this;
}

void LoginMode::init()
{
	draw_model();
}

void LoginMode::mouseClick(int button, int state, int x, int y)
{
}

void LoginMode::keyboard(unsigned char key, int x, int y)
{
	// Enter 키 처리
	if (key == '\r') {
		std::cout << "입력 완료: " << inputText << std::endl;
		return;
	}

	// Backspace 처리
	if (key == 8 && !inputText.empty()) {
		inputText.pop_back();
	}
	else {
		// 일반 문자 입력
		if (key >= 32 && key <= 126) {  // ASCII 출력 가능한 문자만 허용
			inputText += key;
		}
	}

	glutPostRedisplay(); // 다시 그리기 요청
}

void LoginMode::specialKey(int key, int x, int y)
{
}

void LoginMode::specialKeyUp(int key, int x, int y)
{
}

void LoginMode::draw_model()
{
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);  // 2D 좌표계 설정

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 입력 안내 문구
	glColor3f(0.0, 0.0, 0.0);  // 글자 색: 검정
	RenderBitmapString(-0.3f, 0.2f, GLUT_BITMAP_HELVETICA_18, "Enter text:");

	// 현재 입력 중인 문자열 출력
	glColor3f(0.2f, 0.2f, 0.8f);  // 파란색 텍스트
	RenderBitmapString(-0.3f, 0.0f, GLUT_BITMAP_HELVETICA_18, inputText);

	glFlush();
}

void LoginMode::draw_bb()
{
}

void LoginMode::finish()
{
}

// --- 글자 그리기 함수 ---
void LoginMode::RenderBitmapString(float x, float y, void* font, const std::string& str)
{
	glRasterPos2f(x, y);
	for (char c : str)
		glutBitmapCharacter(font, c);
}
