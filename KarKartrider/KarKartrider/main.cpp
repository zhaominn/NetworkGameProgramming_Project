#include <iostream>
#include <vector> 

#include "shaderMaker.h"
#include "DefaultModel.h"
#include "KeyBoard.h"
#include "Camera.h"
#include "Light.h"
#include "Debug.h"
#include "LoadProgress.h"
#include "root.h"
#include "LogoMode.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);


int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);  // ������� ���� �ػ�
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT); // ������� ���� �ػ�
	glutInitWindowPosition(location_x, location_y);
	glutInitWindowSize(window_width, window_height); // â ũ�⸦ ȭ�� ũ��� ����
	glutCreateWindow("KarKartrider");
	//glutFullScreen(); // ��ü ȭ������ ��ȯ

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();
	make_shaderProgram_ui();

	initPhysics(); // Bullet �ʱ�ȭ �Լ� ȣ��

	LogoMode* logoMode = new LogoMode();
	MM.SetMode(logoMode); //�ʱ� ��� ����

	// ����� ���
	/*debug_model(models.back());
	debug_materials(models.back()->materials);*/


	InitBuffer();

	// �ʱ� ������ ���� ������
	drawScene();  // ���÷��� �ݹ� �Լ� ���� ȣ��

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(specialKey);
	glutSpecialUpFunc(specialKeyUp);
	glutMouseFunc(mouseClick);
	glutMainLoop();

	return 0;
}

GLvoid drawScene() {


	MM.draw_model();
	MM.draw_bb();

	glutSwapBuffers();

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error: " << err << endl;
	}
}

// ���� �ʱ�ȭ �Լ�
void InitBuffer() {
	//-----------------------------------------------------------------------------------------------------------
	for (const auto& model : karts) { // ��� �� initBuffer
		model->initBuffer();
	}

	for (const auto& model : road1) { // ��� �� initBuffer
		model->initBuffer();
	}

	for (const auto& model : road2) { // ��� �� initBuffer
		model->initBuffer();
	}

	for (const auto& model : selectMaps) { // ��� �� initBuffer
		model->initBuffer();
	}

	for (const auto& model : countDown) { // ��� �� initBuffer
		model->initBuffer();
	}

	for (const auto& model : booster_uis) {
		model->initBuffer();
	}
	for (const auto& model : dashBoards) {
		model->initBuffer();
	}
}