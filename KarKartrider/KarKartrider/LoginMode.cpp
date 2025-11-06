#include "Pch.h"
#include "LoginMode.h"
#include "Light.h"
#include "SelectMapMode.h"
#include "NetGlobal.h"

LoginMode::LoginMode()
{
    inputText = "";
}

LoginMode::~LoginMode()
{
	delete this;
}

void LoginMode::init()
{
	//draw_model();
}

void LoginMode::mouseClick(int button, int state, int x, int y)
{
}

void LoginMode::keyboard(unsigned char key, int x, int y)
{
	// Enter 키 처리
	if (key == '\r') {
		std::cout << "입력 완료: " << inputText << std::endl;

		C2S_Login_Packet* packet = new C2S_Login_Packet;
		packet->size = sizeof(C2S_Login_Packet);
		packet->type = C2S_LOGIN;
		strncpy(packet->name, inputText.c_str(), NAME_SIZE);
		std::cout << packet->name << std::endl;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), packet->size);
		delete packet;

		SelectMapMode* selectMapMode = new SelectMapMode();
		//selectMapMode->goSelectMode = [this]() { goSelectMode(); };
		MM.SetMode(selectMapMode);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(-0.6f, 0.2f);
	for (char c : std::string("Enter text:"))
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

	glColor3f(0.2f, 0.2f, 0.8f);
	glRasterPos2f(-0.6f, -0.3f);
	for (char c : inputText)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

}

void LoginMode::draw_bb()
{
}

void LoginMode::finish()
{
}