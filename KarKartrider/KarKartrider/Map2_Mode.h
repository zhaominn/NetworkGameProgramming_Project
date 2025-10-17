#pragma once
#include <iostream>
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"
#include "Light.h"

#include <glm/glm/gtc/quaternion.hpp>
#include <glm/glm/gtx/quaternion.hpp>
#include <functional>  // std::function�� ����ϱ� ���� �ʿ�

#include "CustomContactResultCallback.h"

class Map2_Mode : public Mode {
public:

	std::function<void()> goSelectMode; // ����Ʈ ���� ���ư��� �Լ�

	glm::quat cameraRotationQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)); // ���� ī�޶� ����� ���ʹϾ����� ����
	float reducedRotationInfluence = 0.0f; // ������ �ۼ�Ʈ

	GLfloat kart_speed = 0.0f;

	enum Move { NONE_M, UP, DOWN, LEFT, RIGHT, CTRL };
	float ACCELERATION = 0.004f;
	float DECELERATION = 0.003f;
	float LIMIT_SPEED = 1.0;
	float BOOSTER_SPEED = 2.0;
	float MAX_SPEED = 1.0;

	int finish_cnt = 0;

	int start_count;

	bool Pause;

	//Ű
	std::unordered_map<Move, bool> kart_keyState;

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	glm::vec3 cameraTargetPos = glm::vec3(0.0, 0.0, 5.0); // ī�޶��� ��ǥ ��ġ
	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);       // ī�޶��� ���� ��ġ
	glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	// ī�޶� ȸ�� ����
	float yaw = -90.0f; // ���� ȸ�� (�⺻: -Z��)
	float pitch = 0.0f; // ���� ȸ�� (�⺻: ����)
	float TURN_ANGLE = 1.0f; // ȸ�� ���� (�⺻ 1��) (īƮ ȸ�� ����)

	//sound
	bool isBackgroundSound = false;
	std::thread backgroundSoundThread;
	bool isCountNSound = false;
	std::thread countNSoundThread;
	bool isCountGoSound = false;
	std::thread countGoSoundThread;
	bool isMotorSound = false;
	std::thread motorSoundThread;
	bool isCrashSound = false;
	std::thread crashSoundThread;
	bool isBoosterSound = false;
	std::thread boosterSoundThread;
	bool isWinSound = false;
	std::thread winSoundThread;
	bool isLoseSound = false;
	std::thread loseSoundThread;

	// ----- game ------
	int booster_cnt = 2;
	const int MAX_BOOSTER_CNT = 2;        // �ִ� �ν��� ����
	bool isBoosterRegenActive = true;     // �ν��� ����� Ȱ��ȭ ����
	std::thread boosterRegenThread;       // �ν��� ����� ������
	bool isBoosterActive = false; // �ν�Ʈ Ȱ��ȭ ����
	bool isGameOver = false; // ���� ���� ���� �÷���
	int game_timer = 30;

	//ĳ���� �� ȸ�� ����
	float character_face_rotation = 0.0f; // ĳ���� ���� ���� Y�� ȸ�� ����
	const float MAX_FACE_ROTATION = 25.0f; // ����� �¿�� �ִ� ȸ���� ���� (�� ����)
	const float ROTATION_SPEED = 5.0f;     // ��� ȸ�� �ӵ� (�����Ӵ� ȸ�� ����)
	const float RETURN_SPEED = 2.0f;       // ����� �������� ���ư��� �ӵ� (�����Ӵ� ȸ�� ����)

	float booster_head_tilt = 0.0f; // ĳ���� �Ӹ��� X�� ȸ�� ����
	const float MAX_HEAD_TILT = 20.0f; // �ν��� ��� �� �ִ� X�� ȸ�� ����
	const float TILT_SPEED = 2.0f;     // �ν��� �� �Ӹ��� ȸ�� �ӵ�

	Map2_Mode() {
		Mode::currentInstance = this;  // Map1_Mode �ν��Ͻ��� currentInstance�� �Ҵ�
		isCountNSound = true;
		isCountGoSound = true;
		// �ν��� ����� ������ ����
		boosterRegenThread = std::thread(&Map2_Mode::startBoosterRegen, this);
	}
	~Map2_Mode(){}

	// �ν��� ����� ����
	void startBoosterRegen() {
		while (isBoosterRegenActive) {
			// �ν��Ͱ� �ִ� ������ �������� �ʾ��� ���� ��� �� ����
			if (booster_cnt < MAX_BOOSTER_CNT) {
				std::this_thread::sleep_for(std::chrono::seconds(6)); // 3�� ���

				// �ν��� ���� (�ٽ� Ȯ���� ���� ���� �� ����)
				if (booster_cnt < MAX_BOOSTER_CNT) {
					++booster_cnt; // �ν��� ���� ����
					std::cout << "Booster regenerated! Current boosters: " << booster_cnt << std::endl;
				}
			}
			else {
				// ���� �ν��Ͱ� �̹� �ִ�ġ��� ���� �ð� ��� �� �ٽ� Ȯ��
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 0.1�� ���
			}
		}
	}

	void draw_dashBoard() {
		glUseProgram(shaderProgramID_UI);

		// �ؽ�ó Ȱ��ȭ �÷���
		GLint isTextureLocation = glGetUniformLocation(shaderProgramID_UI, "isTexture");
		glUniform1i(isTextureLocation, true);

		// �ؽ�ó �� ������
		for (const auto& dashBoard : dashBoards) {
			dashBoard->draw(shaderProgramID_UI, isKeyPressed_s);
		}
		glUniform1i(isTextureLocation, false);

		glUseProgram(0); // ���� ���̴��� ����
	}

	void draw_speed() {
		glUseProgram(shaderProgramID_UI); // UI �������� ���̴� Ȱ��ȭ

		// isUI �÷��� Ȱ��ȭ
		GLint isUILocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
		glUniform1i(isUILocation, true); // UI ��� Ȱ��ȭ

		glPushMatrix();
		glPixelZoom(5.0f, 5.0f);  // x��, y�� Ȯ�� ���� (2�� Ȯ��)

		// �ڵ��� �ӵ� ���ڿ� ����
		std::string speedText = std::to_string(static_cast<int>(kart_speed * 100));

		// �ڵ��� �ӵ��� ȭ�� ���� ��ܿ� ǥ��
		glRasterPos2f(0.0f, -0.97f); // ȭ�� ���� ���
		for (char c : speedText) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}

		glPixelZoom(1.0f, 1.0f);  // ���� ũ��� ����
		glPopMatrix();
		glUniform1i(isUILocation, false); // UI ��� Ȱ��ȭ

		glUseProgram(0); // ���� ���̴��� ����
	}

	void draw_ui() {
		glUseProgram(shaderProgramID_UI);

		// Ȱ��ȭ �÷���
		GLint isUILocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
		glUniform1i(isUILocation, true);

		std::string uiText = "map : village road";
		glRasterPos2f(-0.95f, 0.85f); // �»�� ��ġ
		for (char c : uiText) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		glUniform1i(isUILocation, false);

		// �ؽ�ó Ȱ��ȭ �÷���
		GLint isTextureLocation = glGetUniformLocation(shaderProgramID_UI, "isTexture");
		glUniform1i(isTextureLocation, true);

		// �ؽ�ó �� ������
		for (int i = 0; i < booster_cnt; ++i) {
			booster_uis[i]->draw(shaderProgramID_UI, isKeyPressed_s);
		}
		glUniform1i(isTextureLocation, false);

		glUseProgram(0); // ���� ���̴��� ����
	}

	void draw_timer() {
		glUseProgram(shaderProgramID_UI);

		// Ȱ��ȭ �÷���
		GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
		glUniform1i(isTimerLocation, true);

		// Ÿ�̸� �ؽ�Ʈ
		std::string timerText = "Time: " + std::to_string(game_timer);
		glRasterPos2f(-0.95f, 0.9f); // �»�� ��ġ
		for (char c : timerText) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		glUniform1i(isTimerLocation, false);

		glUseProgram(0); // ���� ���̴��� ����
	}

	void init() override {

		UpdateRigidBodyTransforms(road2_barricate);
		UpdateRigidBodyTransforms(karts);

		// Move ���� �ʱ�ȭ
		kart_keyState[UP] = false;
		kart_keyState[DOWN] = false;
		kart_keyState[LEFT] = false;
		kart_keyState[RIGHT] = false;

		start_count = -1;

		for (const auto& kart : karts) { // īƮ ��ġ �ʱ�ȭ
			kart->translateMatrix = glm::mat4(1.0f);
			kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(165.0, 1.0, 30.0));
		}
		for (const auto& c : character) { //īƮ�� ���� ��� ����
			c->translateMatrix = karts[0]->translateMatrix;
		}
		for (const auto& c : countDown) { //īƮ�� ���� ��� ����
			c->translateMatrix = karts[0]->translateMatrix;
			c->translateMatrix = glm::translate(c->translateMatrix, glm::vec3(0.0, 4.0, 0.0));
		}

		isBackgroundSound = true;
		backgroundSoundThread = std::thread(&Map2_Mode::backgroundSound, this);

		

		kart_speed = 0.0f;
		draw_model();

		if (!isGameRunning2)
		{
			isGameRunning2 = true;
			glutTimerFunc(0, Map2_Mode::timerHelper, 0);
		}
		cameraPos = glm::vec3(165.0, 4.4, 45.0);
		updateCameraDirection();
	}

	void playCountdown(int count) {
		if (count < 3) { // count_n ���� 3�� ����
			if (countNSoundThread.joinable()) {
				countNSoundThread.join();
			}

			countNSoundThread = std::thread(&Map2_Mode::count_n, this);

			countNSoundThread.join(); // �� �κ��� count_n.wav�� ���� ������ ��ٸ��ϴ�.
		}
		else if (count == 3) { // count_go ���� ����
			if (countGoSoundThread.joinable()) {
				countGoSoundThread.join();
			}

			countGoSoundThread = std::thread(&Map2_Mode::count_go, this);

			// �÷��� �ʱ�ȭ
			//isCountGoSound = false;
			isCountNSound = false;
			isBackgroundSound = true;
		}
	}


	void updateCameraDirection() {
		glm::mat3 rotationMatrix = glm::mat3(karts[0]->translateMatrix);

		// �⺻ ī�޶� ���� ���� ���
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		// ȸ�� ����� ������ ���� ���� ���
		glm::vec3 rotatedDirection = rotationMatrix * direction;

		//-----------------------
		// īƮ�� �ٶ󺸵��� ����
		glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

		// �ڵ����� �ٶ󺸴� ���� ��� (�ڵ��� ��ġ - ī�޶� ��ġ)
		cameraDirection = glm::normalize(rotatedDirection) + carPosition;
	}

	void setCamera() {
		glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

		// �ڵ����� ȸ�� ��� ���� (3x3 ���)
		glm::mat3 carRotationMatrix = glm::mat3(karts[0]->translateMatrix);

		// �ڵ��� ȸ�� ����� ���ʹϾ����� ��ȯ
		glm::quat carRotationQuat = glm::quat_cast(carRotationMatrix);

		// �ڵ��� ȸ���� �⺻ ȸ���� ����
		glm::quat interpolatedRotation = glm::slerp(cameraRotationQuat, carRotationQuat, reducedRotationInfluence);
		cameraRotationQuat = interpolatedRotation;

		// ������ ȸ���� ��ķ� ��ȯ
		glm::mat3 adjustedRotationMatrix = glm::mat3_cast(interpolatedRotation);

		// ī�޶� ��ǥ ��ġ ���� (�ӵ��� ���� �������� ����)
		// "kart_speed"�� ����Ͽ� ī�޶� ��ǥ�� �������� ����
		glm::vec3 baseOffset = glm::vec3(0.0f, 6.0f + (kart_speed * 2.0f), 14.0f + (kart_speed * 10.0f));
		glm::vec3 rotatedOffset = adjustedRotationMatrix * baseOffset;

		// ��ǥ ��ġ ���
		cameraTargetPos = carPosition + rotatedOffset;

		// ���� ī�޶� ��ġ�� ��ǥ ��ġ�� ���������� �̵� (���� �ӵ� ���� ����)
		float cameraFollowSpeed = 0.1f; // ī�޶� ��ǥ�� ���󰡴� �ӵ� (0.0 ~ 1.0)
		cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

		// ī�޶� �ڵ����� �ٶ󺸵��� ���� ������Ʈ
		cameraDirection = carPosition; // ī�޶� �׻� �ڵ����� �ٶ�
	}

	void goSelectMode_() {
		Pause = true;
		if (goSelectMode) { // goSelectMode�� �����Ǿ� �ִٸ� ����
			isBackgroundSound = false;
			isMotorSound = false;
			if (motorSoundThread.joinable()) {
				motorSoundThread.join();
			}
			goSelectMode();
		}
		isBoosterRegenActive = false; // �ν��� ����� ����
		if (boosterRegenThread.joinable()) {
			boosterRegenThread.join(); // ������ ����
		}
	}

	void finish_game() {
		if (finish_cnt != 0) {
			isBackgroundSound = false;
			if (isWinSound) return; // �̹� ���� ���̸� ����
			isWinSound = true;
			isGameOver = true; // ���� ���� ���� ����

			// ���ο� ������ ���� �� �и�
			winSoundThread = std::thread([this]() {
				win_sound();  // ���� ���
				isWinSound = false; // ���� ��� �Ϸ� �� �÷��� ����
				});

			winSoundThread.detach();

			// 5�� �� goSelectMode_() ������ ���� ������ ����
			std::thread([this]() {
				std::this_thread::sleep_for(std::chrono::seconds(9)); // 5�� ���
				goSelectMode_(); // 5�� �� ����
				}).detach();
		}
		else {
			return;
		}
	}

	void draw_finish_time() {
		glUseProgram(shaderProgramID_UI);

		// Ȱ��ȭ �÷���
		GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isRed");
		glUniform1i(isTimerLocation, true);

		// Ÿ�̸� �ؽ�Ʈ
		std::string Text = "Time: " + std::to_string(30 - game_timer);

		glRasterPos2f(0.0f, 0.0f);
		for (char c : Text) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		glUniform1i(isTimerLocation, false);

		glUseProgram(0); // ���� ���̴��� ����
	}

	void lose_game() {
		if (isGameOver) return; // �̹� ���� ���¶�� �������� ����

		isGameOver = true; // ���� ���� ���� ����
		isBackgroundSound = false; // ������� ����

		std::cout << "Game Over! Time is up!" << std::endl;

		// �й� ���� ���
		if (!isLoseSound) {
			isLoseSound = true;
			loseSoundThread = std::thread([this]() {
				lose_sound();
				isLoseSound = false; // ���� ��� �Ϸ� �� �÷��� ����
				});
			loseSoundThread.detach();

			// 5�� �� goSelectMode_() ������ ���� ������ ����
			std::thread([this]() {
				std::this_thread::sleep_for(std::chrono::seconds(9)); // 5�� ���
				goSelectMode_(); // 5�� �� ����
				}).detach();
		}
	}

	void checkCollisionKart() {
		for (auto& kart : karts) {
			if (kart->name != "car") continue; // īƮ�� "car" �̸��� �ƴϸ� ��ŵ

			// �߷��� ���� (�� ���� ����)
			kart->rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));

			for (const auto& barri : road2_barricate) {
				// �浹 �ݹ� ��ü ����
				CustomContactResultCallback resultCallback;

				// �� ��ü�� �浹 ����
				dynamicsWorld->contactPairTest(kart->rigidBody, barri->rigidBody, resultCallback);

				if (resultCallback.hitDetected) { // �浹�� �����Ǿ��� ��

					if (barri->name == "finish_ch") {
						finish_cnt++;
						continue;
					}

					if (barri->name == "finish") { //����~~~~
						finish_game();
						continue;
					}

					// �浹 ���� ��� (isCrashSound�� �ߺ� ��� ����)
					if (!isCrashSound) {
						isCrashSound = true;
						crashSoundThread = std::thread(&Map2_Mode::crash_sound, this);
						crashSoundThread.detach(); // �����带 �и��Ͽ� �񵿱� ���
					}

					// 1. �浹 ���� �� ħ�� ���� ��������
					btVector3 collisionNormal = resultCallback.collisionNormal; // �浹 ����
					collisionNormal.setY(0.0f); // y�� ���� ���� (xz ��鿡���� ó��)

					// ħ�� ���� ����
					float penetrationDepth = std::abs(resultCallback.penetrationDepth);
					const float MAX_PENETRATION_DEPTH = 2.0f; // �ִ� ħ�� ����
					if (penetrationDepth > MAX_PENETRATION_DEPTH) {
						penetrationDepth = MAX_PENETRATION_DEPTH;
					}

					// 2. ���� �� ��� (ħ�� ���̿� ����� �̵�)
					float correctionScale = 0.5f; // ���� �� ���� ���
					btVector3 correction = correctionScale * collisionNormal * penetrationDepth;

					// 3. �ӵ� ��� �߰� �̵� ����
					glm::vec3 kartVelocity = glm::vec3(-kart->translateMatrix[2]) * kart_speed;
					float speedFactor = glm::length(kartVelocity); // īƮ�� �ӵ� ũ��
					correction += collisionNormal * speedFactor * 0.2f; // �ӵ��� ����� �߰� �̵� (0.2�� ���� ����)

					// 4. īƮ ��ġ ������Ʈ
					btTransform kartTransform;
					kart->rigidBody->getMotionState()->getWorldTransform(kartTransform);
					btVector3 kartPos = kartTransform.getOrigin();

					btVector3 newKartPos = kartPos + correction; // ���� ��ġ ���
					newKartPos.setY(1.0f); // y�� ����

					kartTransform.setOrigin(newKartPos);

					// ���� ������ ���ο� Transform ����
					kart->rigidBody->getMotionState()->setWorldTransform(kartTransform);
					kart->rigidBody->setWorldTransform(kartTransform);

					// OpenGL ��ȯ ��Ŀ� �ݿ�
					btScalar transformMatrix[16];
					kartTransform.getOpenGLMatrix(transformMatrix);
					kart->translateMatrix = glm::make_mat4(transformMatrix);

					// 5. �ӵ� ���� ó��
					float decelerationFactor = 0.2f; // ���� ���� (�浹 �� �ӵ� ����)
					kart_speed *= 1.0f - decelerationFactor;
					if (kart_speed < 0.01f) { // �ʹ� �������� ����
						kart_speed = 0.0f;
					}
				}
			}
		}
	}


	void checkEngineSound() {
		if (kart_speed != 0.0f) {
			if (!isMotorSound) { // ���� ���尡 ��� ������ ���� ���� ����
				isMotorSound = true;
				motorSoundThread = std::thread(&Map2_Mode::engine_sound, this); // ���� ���� ����
			}
		}
		else { // �ӵ��� 0�� ���
			if (isMotorSound) {
				isMotorSound = false;
				if (motorSoundThread.joinable()) {
					motorSoundThread.detach(); // ������ ���� (�ʿ��� ��� detach)
				}
			}
		}
	}

	void timer() {
		if (!Pause) {
			if (start_count < 4) {
				if (start_count >= 0)
					playCountdown(start_count);
				++start_count;
			}
			else {

				// ����/���� ó��
				if (kart_keyState[UP]) {
					if (kart_speed < MAX_SPEED) {
						kart_speed += ACCELERATION; // ���ӵ��� ���� ����
						if (kart_speed > MAX_SPEED) kart_speed = MAX_SPEED; // �ִ� �ӵ� ����
					}
				}
				else if (kart_keyState[DOWN]) {
					if (kart_speed > -MAX_SPEED / 2.0f) { // ���� �ӵ��� ������ ���ݱ����� ���
						kart_speed -= ACCELERATION; // ���� �ÿ��� ���ӵ� �ݿ�
						if (kart_speed < -MAX_SPEED / 2.0f) kart_speed = -MAX_SPEED / 2.0f; // ���� �ִ� �ӵ� ����
					}
				}
				else {
					// �ӵ��� ������ �� ����
					if (kart_speed > 0.0f) {
						kart_speed -= DECELERATION; // ���� ����
						if (kart_speed < 0.0f) kart_speed = 0.0f; // 0���� ����ȭ
					}
					else if (kart_speed < 0.0f) {
						kart_speed += DECELERATION; // ���� ����
						if (kart_speed > 0.0f) kart_speed = 0.0f; // 0���� ����ȭ
					}
				}

				// �ӵ��� ���� (MAX_SPEED�� �ʰ����� �ʵ���)
				if (kart_speed > MAX_SPEED) kart_speed = MAX_SPEED;

				// īƮ �̵� ó��
				if (kart_speed > 0.0f) { // ����
					for (const auto& kart : karts) {
						kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
					}
				}
				else if (kart_speed < 0.0f) { // ����
					for (const auto& kart : karts) {
						kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
					}
				}

				// ���� ��ȯ ó��
				if (kart_keyState[LEFT]) {
					if (kart_speed != 0.0f) {
						for (const auto& kart : karts) {
							kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
							kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(TURN_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
							kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
						}
					}
				}

				if (kart_keyState[RIGHT]) {
					if (kart_speed != 0.0f) {
						for (const auto& kart : karts) {
							kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
							kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(-TURN_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
							kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
						}
					}
				}

				//ĳ���� 
				for (const auto& c : character) { //īƮ�� ���� ��� ����
					c->translateMatrix = karts[0]->translateMatrix;
				}

				// ī�޶� ȸ�� ������ ������Ʈ (�ӵ��� ���� ī�޶� ȸ���� ������)
				if (kart_speed != 0.0f) {
					reducedRotationInfluence = 0.1f + (std::abs(kart_speed) / MAX_SPEED) * 0.4f; // �ӵ� ��� ������
				}
				else {
					reducedRotationInfluence += 0.01f; // Ű�� �� ���� �� õõ�� ȸ��
					if (reducedRotationInfluence > 1.0f) reducedRotationInfluence = 1.0f;
				}

				// ����� õõ�� �������� ���ư����� ����
				if (!kart_keyState[LEFT] && !kart_keyState[RIGHT]) {
					if (character_face_rotation > 0.0f) {
						character_face_rotation -= RETURN_SPEED;
						if (character_face_rotation < 0.0f) {
							character_face_rotation = 0.0f; // �������� ����
						}
					}
					else if (character_face_rotation < 0.0f) {
						character_face_rotation += RETURN_SPEED;
						if (character_face_rotation > 0.0f) {
							character_face_rotation = 0.0f; // �������� ����
						}
					}
				}

				if (isBoosterActive) {
					// �Ӹ��� X������ �ڷ� ����̱� (������ MAX_HEAD_TILT����)
					if (booster_head_tilt < MAX_HEAD_TILT) {
						booster_head_tilt += TILT_SPEED;
						if (booster_head_tilt > MAX_HEAD_TILT) {
							booster_head_tilt = MAX_HEAD_TILT;
						}
					}
				}
				else {
					// �Ӹ��� ���� ���·� ���� (������ 0���� ����)
					if (booster_head_tilt > 0.0f) {
						booster_head_tilt -= TILT_SPEED;
						if (booster_head_tilt < 0.0f) {
							booster_head_tilt = 0.0f;
						}
					}
				}

				// ĳ���� �� ������Ʈ (�Ӹ��� X�� ȸ�� ����)
				for (const auto& c : character) {
					if (c->name == "character_face") {
						// ���� Y�� ȸ�� (character_face_rotation) �߰�
						glm::mat4 headRotation = glm::rotate(
							glm::mat4(1.0f),
							glm::radians(-character_face_rotation),
							glm::vec3(0.0f, 0.0f, 1.0f)
						);

						// X�� ȸ�� �߰� (�ν��� ���¿� ���� �Ӹ� �����)
						headRotation = glm::rotate(
							headRotation,
							glm::radians(booster_head_tilt), // X�� ȸ��
							glm::vec3(1.0f, 0.0f, 0.0f)
						);

						c->translateMatrix = karts[0]->translateMatrix * headRotation;
					}
					else {
						c->translateMatrix = karts[0]->translateMatrix;
					}
				}

				// ī�޶� ������Ʈ
				setCamera();
				// ���� ī�޶� ��ġ�� ��ǥ ��ġ�� ���������� �̵�
				float cameraFollowSpeed = 0.1f; // ī�޶� ���󰡴� �ӵ� (0.0 ~ 1.0 ������ ��)
				cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

				checkCollisionKart();
				checkEngineSound();
			}
		}
	}

	void mouseClick(int button, int state, int x, int y) override {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			if (x <= 470 && x >= 400 && y <= 410 && y >= 360) { //�ٽýõ�
				Pause = true;
				isBackgroundSound = false;
				isMotorSound = false;
				if (motorSoundThread.joinable()) {
					motorSoundThread.join();
				}
				Map2_Mode* map2Mode = new Map2_Mode();
				map2Mode->goSelectMode = [this]() { goSelectMode(); }; // ���ٷ� ����
				MM.SetMode(map2Mode);
			}
			else if (x <= 580 && x >= 510 && y <= 410 && y >= 360) { //�޴�
				goSelectMode_();
			}
		}
	}

	void keyboard(unsigned char key, int x, int y) override {
		if (key == 27) { //esc
			if (Pause) {
				//glutTimerFunc(16, timerHelper, 0); // Ÿ�̸� ȣ��
				//isMotorSound = true;
			}
			else {
				glm::vec3 zAxis = glm::normalize(cameraPos - glm::vec3(karts[0]->translateMatrix[3]));
				// ������ ���� (X��) ���
				glm::vec3 xAxis = glm::normalize(glm::cross(cameraUp, zAxis));
				// ��� ���� (Y��) ���
				glm::vec3 yAxis = glm::cross(zAxis, xAxis);
				// 3x3 ȸ�� ��� ����
				glm::mat3 rotationMatrix = glm::mat3(
					xAxis, // X��
					yAxis, // Y��
					zAxis  // Z��
				);
				// 4x4 ��ķ� Ȯ��
				glm::mat4 modelMatrix = glm::mat4(1.0f); // ���� ��ķ� �ʱ�ȭ
				modelMatrix[0] = glm::vec4(rotationMatrix[0], 0.0f); // X��
				modelMatrix[1] = glm::vec4(rotationMatrix[1], 0.0f); // Y��
				modelMatrix[2] = glm::vec4(rotationMatrix[2], 0.0f); // Z��
				modelMatrix[3] = glm::vec4(cameraPos, 1.0f);          // ��ġ �߰�
				pause[0]->translateMatrix = modelMatrix;
				pause[0]->translateMatrix = glm::translate(pause[0]->translateMatrix, glm::vec3(0.0, 0.0, -2.0));


				isMotorSound = false;
				if (motorSoundThread.joinable()) {
					motorSoundThread.detach(); // ������ ���� (�ʿ��� ��� detach)
				}
			}
			Pause = !Pause;
		}
		if (key == 'p') {
			goSelectMode_();
		}
	}

	// �ν��� ���� �Լ�
	void activateBooster() {

		// �ν�Ʈ�� �̹� Ȱ��ȭ ���̸� �������� ����
		if (isBoosterActive) {
			std::cout << "Booster is already active!" << std::endl;
			return;
		}

		// �ν�Ʈ Ȱ��ȭ ���·� ����
		isBoosterActive = true;

		std::cout << "Booster activated! Remaining boosters: " << booster_cnt << std::endl;

		// ���� MAX_SPEED�� ACCELERATION ���� ����
		float originalMaxSpeed = MAX_SPEED;
		float originalAcceleration = ACCELERATION;

		// �ν��� �ӵ� �� ���ӵ� ����
		MAX_SPEED = BOOSTER_SPEED;           // �ν��� �ӵ�
		ACCELERATION *= 1.05f;                // ���ӵ��� 1.5��� ���� (���� ����)

		// �ν��� ���� ���
		if (!isBoosterSound) {
			isBoosterSound = true;
			boosterSoundThread = std::thread(&Map2_Mode::booster_sound, this);
			boosterSoundThread.detach(); // �񵿱� ����
		}

		// �ν��� ���� �� �ӵ� �� ���ӵ� ����
		std::thread([this, originalMaxSpeed, originalAcceleration]() {
			std::this_thread::sleep_for(std::chrono::duration<double>(4.4)); // �ν��� ���� �ð� 3��
			MAX_SPEED = originalMaxSpeed;        // ���� �ӵ� ����
			ACCELERATION = originalAcceleration; // ���� ���ӵ� ����
			isBoosterActive = false;             // �ν�Ʈ ��Ȱ��ȭ ���·� ����
			std::cout << "Booster ended. MAX_SPEED and ACCELERATION restored." << std::endl;
			}).detach();
	}

	void specialKey(int key, int x, int y) override {


		switch (key) {
		case GLUT_KEY_UP:
			kart_keyState[UP] = true;
			break;
		case GLUT_KEY_DOWN:
			kart_keyState[DOWN] = true;
			break;
		case GLUT_KEY_LEFT:
			kart_keyState[LEFT] = true;
			// ���� �������� ��� ȸ��
			if (character_face_rotation > -MAX_FACE_ROTATION) {
				character_face_rotation -= ROTATION_SPEED;
			}
			break;
		case GLUT_KEY_RIGHT:
			kart_keyState[RIGHT] = true;
			// ������ �������� ��� ȸ��
			if (character_face_rotation < MAX_FACE_ROTATION) {
				character_face_rotation += ROTATION_SPEED;
			}
			break;
		}

		int modifiers = glutGetModifiers();

		// Ctrl �ܵ� ����
		if (modifiers & GLUT_ACTIVE_CTRL) {
			// �ν�Ʈ�� �̹� Ȱ��ȭ ���̰ų� �ν�Ʈ ������ 0�̸� �������� ����
			if (isBoosterActive) {
				std::cout << "Booster is already active!" << std::endl;
				return;
			}

			if (booster_cnt > 0) { // �ν��Ͱ� ���� �ִ� ���
				booster_cnt--; // �ν��� ���� ����
				activateBooster(); // �ν��� Ȱ��ȭ
			}
			else {
				std::cout << "No boosters left!" << std::endl;
			}
		}

	}

	void specialKeyUp(int key, int x, int y) override {
		switch (key) {
		case GLUT_KEY_UP:
			kart_keyState[UP] = false;
			break;
		case GLUT_KEY_DOWN:
			kart_keyState[DOWN] = false;
			break;
		case GLUT_KEY_LEFT:
			kart_keyState[LEFT] = false;
			break;
		case GLUT_KEY_RIGHT:
			kart_keyState[RIGHT] = false;
			break;
		}
	}

	void draw_model() override {

		glClearColor(1.0, 1.0, 1.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgramID);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "Error in glUseProgram: " << error << std::endl;
		}

		this->view = glm::mat4(1.0f);
		this->view = glm::lookAt(
			this->cameraPos,
			this->cameraDirection,
			this->cameraUp
		);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		this->projection = glm::mat4(1.0f);
		this->projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 10000.0f);
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
		GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

		glEnable(GL_DEPTH_TEST);

		for (const auto& kart : karts) { // ���� �� draw
			kart->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& road : road2) { // ���� �� draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& c : character) { // ���� �� draw
			if (c->name == "booster" && !isBoosterActive)
				continue;
			c->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& barricate : road2_barricate) { // ���� �� draw
			barricate->draw(shaderProgramID, isKeyPressed_s);
		}
		if (start_count >= 0 && start_count < 4) {
			countDown[start_count]->draw(shaderProgramID, isKeyPressed_s);
		}

		if (Pause)
			pause[0]->draw(shaderProgramID, isKeyPressed_s);

		// Draw Timer
		glDisable(GL_DEPTH_TEST);
		draw_timer();
		draw_ui();
		draw_dashBoard();
		draw_speed();
		if (isGameOver)
			draw_finish_time();
		glEnable(GL_DEPTH_TEST);

		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
		if (!bb_status)
			return;
		for (const auto& model : karts) { // �� bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& barricate : road2_barricate) { // �� bb draw
			barricate->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}
private:

	void updatePhysics(float deltaTime) {
		// ���� ���� ������Ʈ (deltaTime�� ���� ��Ȯ�� ����)
		dynamicsWorld->stepSimulation(deltaTime);

		// ���� �������� ��ü�� Transform ������Ʈ
		UpdateRigidBodyTransforms(karts);
		UpdateRigidBodyTransforms(road2_barricate);

		// �浹 ó�� (���� ���� ������Ʈ �� ����)
		checkCollisionKart();
	}

	static void timerHelper(int value) {
		if (Map2_Mode* instance = dynamic_cast<Map2_Mode*>(Mode::currentInstance)) {
			const float deltaTime = 1.0f / 60.0f; // 60FPS ����, �� �������� �ð�

			// ���� ���� �� ���� ���� ������Ʈ (���� ���� ���¿����� ��� ����)
			instance->updatePhysics(deltaTime); // ���� ���� ������Ʈ
			instance->timer(); // ������ �� ���� ���� ������Ʈ

			// ���� Ÿ�̸� ���� (���� ���� ���¿����� Ÿ�̸Ӹ� ���ҽ�Ű�� ����)
			if (!instance->isGameOver) {
				static float elapsedTime = 0.0f;
				elapsedTime += deltaTime;
				if (elapsedTime >= 1.0f) { // 1�ʰ� �����ٸ�
					elapsedTime = 0.0f;
					instance->game_timer--; // Ÿ�̸� 1�� ����
					if (instance->game_timer <= 0) { // Ÿ�̸Ӱ� 0�� �Ǹ�
						instance->game_timer = 0;
						instance->lose_game(); // �й� ó��
					}
				}
			}
		}

		// ������ ������Ʈ�� 60FPS�� ����
		glutPostRedisplay();
		glutTimerFunc(16, timerHelper, value); // 16ms �������� ȣ�� (�� 60FPS)
	}


	// bgm ���� �Լ�
	void backgroundSound() {
		play_sound2D("village_04.ogg", "./asset/map_1/", true, &isBackgroundSound);
	}
	void count_go() {
		play_sound2D("count_go.wav", "./asset/map_1/", false, &isCountGoSound);
	}
	void count_n() {
		play_sound2D("count_n.wav", "./asset/map_1/", false, &isCountNSound);
	}
	void engine_sound() {
		play_sound2D("motor_x.ogg", "./asset/map_1/", true, &isMotorSound);
	}
	void crash_sound() {
		play_sound2D("crash.ogg", "./asset/map_1/", false, &isCrashSound);
		isCrashSound = false;
	}
	void booster_sound() {
		play_sound2D("booster.ogg", "./asset/map_1/", false, &isBoosterSound);
		isBoosterSound = false;
	}
	void win_sound() {
		play_sound2D("game_win.ogg", "./asset/map_1/", false, &isWinSound);
		isWinSound = false;
	}
	void lose_sound() {
		play_sound2D("game_lose.ogg", "./asset/map_1/", false, &isLoseSound);
		isLoseSound = false;
	}
};