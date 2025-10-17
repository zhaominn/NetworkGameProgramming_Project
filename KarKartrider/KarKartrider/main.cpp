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
	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);  
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT); 
	glutInitWindowPosition(location_x, location_y);
	glutInitWindowSize(window_width, window_height); 
	glutCreateWindow("KarKartrider");
	//glutFullScreen(); 

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();
	make_shaderProgram_ui();

	initPhysics(); 

	LogoMode* logoMode = new LogoMode();
	MM.SetMode(logoMode); 

	/*debug_model(models.back());
	debug_materials(models.back()->materials);*/


	InitBuffer();


	drawScene(); 

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

void InitBuffer() {
	//-----------------------------------------------------------------------------------------------------------
	for (const auto& model : karts) {
		model->initBuffer();
	}

	for (const auto& model : road1) { 
		model->initBuffer();
	}

	for (const auto& model : road2) {
		model->initBuffer();
	}

	for (const auto& model : selectMaps) {
		model->initBuffer();
	}

	for (const auto& model : countDown) { 
		model->initBuffer();
	}

	for (const auto& model : booster_uis) {
		model->initBuffer();
	}
	for (const auto& model : dashBoards) {
		model->initBuffer();
	}
}