#pragma once
#include <iostream>
#include <thread>
#include <random>

#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
//#include "PlayMode.h"
#include "Map1_Mode.h"
#include "Map2_Mode.h"


bool isAnimating = false;  // ī�޶� �̵� ������ Ȯ��
float animationSpeed = 0.05f; // �ִϸ��̼� �ӵ� (0.05 = �ε巯�� �̵�)
glm::vec3 targetCameraPos = glm::vec3(0.0, 0.0, 5.0);   // ��ǥ ī�޶� ��ġ
glm::vec3 cameraPosMapMode = glm::vec3(0.0, 0.0, 5.0);

void SelectMap_timer(int value) {
	if (isAnimating) {
		// ���� ī�޶� ��ġ�� ��ǥ ��ġ(targetCameraPos)�� LERP
		cameraPosMapMode = glm::mix(cameraPosMapMode, targetCameraPos, animationSpeed);

		// ī�޶� ��ǥ ��ġ�� ���� �����ߴ��� Ȯ�� (���� ���� ���)
		if (glm::distance(cameraPosMapMode, targetCameraPos) < 0.01f) {
			cameraPosMapMode = targetCameraPos; // ��ǥ ��ġ�� ��Ȯ�� ����
			isAnimating = false; // �ִϸ��̼� ����
		}
	}

	glutPostRedisplay();            // ȭ�� ������Ʈ ��û
	glutTimerFunc(16, SelectMap_timer, 0);    // �� 60FPS�� Ÿ�̸� �ݺ� ȣ��
}

class SelectMapMode : public Mode {
public:

	int map_num = 1;

	glm::vec3 map1CamerPos = glm::vec3(0.0, 0.0, 5.0);      // Map1 ī�޶� ��ġ
	glm::vec3 map2CamerPos = glm::vec3(2.5, 0.0, 5.0);      // Map2 ī�޶� ��ġ

	glm::vec3 cameraDirectionMapMode = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 cameraUpMapMode = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projectionMapMode = glm::mat4(1.0f);
	glm::mat4 viewMapMode = glm::mat4(1.0f);


	bool isSoundRunning;
	std::thread soundThread;

	bool isclickRunning;

	SelectMapMode() :isSoundRunning(true), isclickRunning(true) {}
	~SelectMapMode(){}

	void goSelectMode() {
		SelectMapMode* selectMode = new SelectMapMode();
		MM.SetMode(selectMode);
	}

	void init() override {

		soundThread = std::thread(&SelectMapMode::runSound, this);

		//// ī�޶� ��ġ�� Y�� �������� ȸ��
		//float angleInRadians = glm::radians(-15.0f); // 10�� ȸ��
		//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleInRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		//glm::vec4 rotatedCameraPos = rotation * glm::vec4(cameraPosMapMode, 1.0f);
		//cameraPosMapMode = glm::vec3(rotatedCameraPos); // ȸ���� ��ġ�� ī�޶� ��ġ�� ����
		targetCameraPos = cameraPosMapMode; // ���� ��ġ�� ��ǥ ��ġ�� ����
		glutTimerFunc(0, SelectMap_timer, 0);         // Ÿ�̸� �Լ� ����

	}

	void mouseClick(int button, int state, int x, int y) override {}
	 
	void keyboard(unsigned char key, int x, int y) override {
		std::thread clickSoundThread(&SelectMapMode::clickSound, this);
		switch (key) {
		case '\r': {
			isSoundRunning = false;
			/*
			PlayMode* playMode = new PlayMode();
			playMode->map_num = map_num;
			MM.SetMode(playMode);
			*/
			if (map_num == 1) {
				Map1_Mode* map1Mode = new Map1_Mode();
				map1Mode->goSelectMode = [this]() { goSelectMode(); }; // ���ٷ� ����
				MM.SetMode(map1Mode);
			}
			else if (map_num == 2) {
				Map2_Mode* map2Mode = new Map2_Mode();
				map2Mode->goSelectMode = [this]() { goSelectMode(); }; // ���ٷ� ����
				MM.SetMode(map2Mode);
			}
			break;
		}
		default:
			break;
		}
		clickSoundThread.detach();
	}


	void updateTargetCameraPos() {
		if (map_num == 1) {
			targetCameraPos = map1CamerPos;
		}
		else if (map_num == 2) {
			targetCameraPos = map2CamerPos;
		}

		isAnimating = true; // �ִϸ��̼� ����
	}

	void specialKey(int key, int x, int y) override {
		std::thread clickSoundThread(&SelectMapMode::clickSound, this);
		if (key == GLUT_KEY_LEFT) {
			map_num--;
			if (map_num < 1) {
				map_num = 1;
			}
			this->updateTargetCameraPos(); // ��ǥ ī�޶� ��ġ ������Ʈ
		}
		else if (key == GLUT_KEY_RIGHT) {
			map_num++;
			if (map_num > 2) {
				map_num = 2;
			}
			this->updateTargetCameraPos(); // ��ǥ ī�޶� ��ġ ������Ʈ
		}
		clickSoundThread.detach(); // ������ ���� ���� (���� ������ ��� ����)
	}

	void specialKeyUp(int key, int x, int y) override {}

	void draw_model() override {

		glClearColor(0.0, 0.0, 0.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgramID);


		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "Error in glUseProgram: " << error << std::endl;
		}

		// ������ �ٶ󺸵��� ���� (Z�� -1 ����)
		glm::vec3 fixedLookDirection = glm::vec3(0.0, 0.0, -1.0); // �׻� ����(-Z) ����
		glm::vec3 lookAtTarget = cameraPosMapMode + fixedLookDirection;

		// View ��� ����
		viewMapMode = glm::lookAt(
			cameraPosMapMode,       // ī�޶� ��ġ
			lookAtTarget,           // ������ �ٶ󺸵��� ����
			cameraUpMapMode         // �� ����
		);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &viewMapMode[0][0]);

		projectionMapMode = glm::mat4(1.0f);
		projectionMapMode = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 300.0f);
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projectionMapMode[0][0]);

		glEnable(GL_DEPTH_TEST);

		for (const auto& maps : selectMaps) { // ���� �� draw
			maps->draw(shaderProgramID, isKeyPressed_s);
		}

		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
	}

	void finish() override {
		// ������ ���� ���� ó��
		if (soundThread.joinable()) {
			isSoundRunning = false; // ���� ���� �÷��� ����
			soundThread.join();     // �����尡 ����� ������ ���
		}
	}
private:
	// ���� ���� �Լ�
	void runSound() {

		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> file_random_dis(0, 2);

		std::string file1 = "title_xmas_01.ogg";
		std::string file2 = "title_original.ogg";
		std::string file3 = "title_world.ogg";
		std::string file = "";

		switch (file_random_dis(gen))
		{
		case 0:
			file = file1;
			break;
		case 1:
			file = file2;
			break;
		case 2:
			file = file3;
			break;
		default:
			file = file2;
			break;
		}

		play_sound2D(file, "./asset/select_mode/", true, &isSoundRunning);
	}

	void clickSound() {
		isclickRunning = true;
		play_sound2D("click.wav", "./asset/select_mode/", false, &isclickRunning);
		isclickRunning = false;
	}
};