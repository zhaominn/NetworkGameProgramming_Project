#include "KeyBoard.h"
#include <iostream>

// 실제 정의는 .cpp에 한 번만
std::unordered_map<char, bool> keyState;

void keyDown_s(const char& key) {
	keyState[key] = !keyState[key];
}

void keyUp_s(const char& key) {
	keyState[key] = false;
}

bool isKeyPressed_s(const char& key) {
	return keyState[key];
}

void keyUp(unsigned char key, int x, int y) {
	//keyUp_s(key);
	glutPostRedisplay();
}

void keyDown(unsigned char key, int x, int y) {

	MM.keyboard(key, x, y);

	keyDown_s(key);

	switch (key)
	{
	case '[':
		volume -= 0.05f;
		break;
	case ']':
		volume += 0.05f;
		break;
	case 'b':
		bb_status = !bb_status;
		break;
	case 'q':
		std::cout << " ���α׷� ���� " << std::endl;
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void specialKey(int key, int x, int y) {
	MM.specialKey(key, x, y);

	glutPostRedisplay();
}

void specialKeyUp(int key, int x, int y) {
	MM.specialKeyUp(key, x, y);

	glutPostRedisplay();
}

void mouseClick(int button, int state, int x, int y) {
	MM.mouseClick(button, state, x, y);

	glutPostRedisplay();
}