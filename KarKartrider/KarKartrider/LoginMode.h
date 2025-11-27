#pragma once
#include "Mode.h" 
#include "root.h"

class LoginMode : public Mode
{
public:

	glm::vec3 cameraTargetPos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f); // 2D 투영
	glm::mat4 view = glm::mat4(1.0f); // 카메라 변환 없음

	GLuint login_tex;
	GLuint before_tex;
	GLuint after_tex;
	GLuint nickname_tex;

	LoginMode();
	~LoginMode();

	bool startButton = false;
	bool isButtonHovered = false;

	void init() override;

	GLuint loadTexture(const char* filename);

	void draw_login();

	void mouseClick(int button, int state, int x, int y) override;

	void passiveMotion(int x, int y) override;

	void keyboard(unsigned char key, int x, int y) override;

	void specialKey(int key, int x, int y) override;

	void specialKeyUp(int key, int x, int y) override;

	void draw_model() override;

	void draw_bb() override;

	void finish() override;

	virtual ModeType GetModeType() const override {
		return ModeType::LOGIN;
	}

private:
	std::string inputText;

	bool isRunning;

};

