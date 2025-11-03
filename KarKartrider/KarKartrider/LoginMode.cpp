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

	/*glUseProgram(shaderProgramID);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error in glUseProgram: " << error << std::endl;
	}*/



}

void LoginMode::draw_bb()
{
}

void LoginMode::finish()
{
}
