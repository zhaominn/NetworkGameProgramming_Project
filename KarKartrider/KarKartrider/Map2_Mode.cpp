#include "Pch.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"
#include "Light.h"
#include "Map2_Mode.h"
#include "CustomContactResultCallback.h"
#include "BulletPhysics.h"
#include "LoadSound.h"


Map2_Mode::Map2_Mode()
{
	Mode::currentInstance = this;
	isCountNSound = true;
	isCountGoSound = true;
	boosterRegenThread = std::thread(&Map2_Mode::startBoosterRegen, this);
}

void Map2_Mode::startBoosterRegen() {
	while (isBoosterRegenActive) {
		if (booster_cnt < MAX_BOOSTER_CNT) {
			std::this_thread::sleep_for(std::chrono::seconds(6));

			if (booster_cnt < MAX_BOOSTER_CNT) {
				++booster_cnt;
				std::cout << "Booster regenerated! Current boosters: " << booster_cnt << std::endl;
			}
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void Map2_Mode::draw_dashBoard() {
	glUseProgram(shaderProgramID_UI);

	GLint isTextureLocation = glGetUniformLocation(shaderProgramID_UI, "isTexture");
	glUniform1i(isTextureLocation, true);

	for (const auto& dashBoard : dashBoards) {
		dashBoard->draw(shaderProgramID_UI, isKeyPressed_s);
	}
	glUniform1i(isTextureLocation, false);

	glUseProgram(0);
}

void Map2_Mode::draw_speed() {
	glUseProgram(shaderProgramID_UI);

	GLint isUILocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
	glUniform1i(isUILocation, true);

	glPushMatrix();
	glPixelZoom(5.0f, 5.0f);

	std::string speedText = std::to_string(static_cast<int>(kart_speed * 100));

	glRasterPos2f(0.0f, -0.97f);
	for (char c : speedText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glPixelZoom(1.0f, 1.0f);
	glPopMatrix();
	glUniform1i(isUILocation, false);

	glUseProgram(0);
}

void Map2_Mode::draw_ui() {
	glUseProgram(shaderProgramID_UI);

	GLint isUILocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
	glUniform1i(isUILocation, true);

	std::string uiText = "map : village road";
	glRasterPos2f(-0.95f, 0.85f);
	for (char c : uiText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glUniform1i(isUILocation, false);

	GLint isTextureLocation = glGetUniformLocation(shaderProgramID_UI, "isTexture");
	glUniform1i(isTextureLocation, true);

	for (int i = 0; i < booster_cnt; ++i) {
		booster_uis[i]->draw(shaderProgramID_UI, isKeyPressed_s);
	}
	glUniform1i(isTextureLocation, false);

	glUseProgram(0);
}

void Map2_Mode::draw_timer(float deltaTime) {
	glUseProgram(shaderProgramID_UI);

	GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
	glUniform1i(isTimerLocation, true);

	std::string timerText = "Time: " + std::to_string(deltaTime);
	glRasterPos2f(-0.95f, 0.9f);
	for (char c : timerText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glUniform1i(isTimerLocation, false);

	glUseProgram(0);
}

void Map2_Mode::init() {

	UpdateRigidBodyTransforms(road2_barricate);
	UpdateRigidBodyTransforms(karts);

	kart_keyState[UP] = false;
	kart_keyState[DOWN] = false;
	kart_keyState[LEFT] = false;
	kart_keyState[RIGHT] = false;

	start_count = -1;

	for (const auto& kart : karts) {
		kart->translateMatrix = glm::mat4(1.0f);
		kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(165.0, 1.0, 30.0));
	}
	for (const auto& c : character) {
		c->translateMatrix = karts[0]->translateMatrix;
	}
	for (const auto& c : countDown) {
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

void Map2_Mode::playCountdown(int count) {
	if (count < 3) {
		if (countNSoundThread.joinable()) {
			countNSoundThread.join();
		}

		countNSoundThread = std::thread(&Map2_Mode::count_n, this);

		countNSoundThread.join();
	}
	else if (count == 3) {
		if (countGoSoundThread.joinable()) {
			countGoSoundThread.join();
		}

		countGoSoundThread = std::thread(&Map2_Mode::count_go, this);

		//isCountGoSound = false;
		isCountNSound = false;
		isBackgroundSound = true;
	}
}

void Map2_Mode::updateCameraDirection() {
	glm::mat3 rotationMatrix = glm::mat3(karts[0]->translateMatrix);

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	glm::vec3 rotatedDirection = rotationMatrix * direction;

	glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

	cameraDirection = glm::normalize(rotatedDirection) + carPosition;
}

void Map2_Mode::setCamera() {
	glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

	glm::mat3 carRotationMatrix = glm::mat3(karts[0]->translateMatrix);

	glm::quat carRotationQuat = glm::quat_cast(carRotationMatrix);

	glm::quat interpolatedRotation = glm::slerp(cameraRotationQuat, carRotationQuat, reducedRotationInfluence);
	cameraRotationQuat = interpolatedRotation;

	glm::mat3 adjustedRotationMatrix = glm::mat3_cast(interpolatedRotation);

	glm::vec3 baseOffset = glm::vec3(0.0f, 6.0f + (kart_speed * 2.0f), 14.0f + (kart_speed * 10.0f));
	glm::vec3 rotatedOffset = adjustedRotationMatrix * baseOffset;

	cameraTargetPos = carPosition + rotatedOffset;

	float cameraFollowSpeed = 0.1f;
	cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

	cameraDirection = carPosition;
}

void Map2_Mode::goSelectMode_() {
	Pause = true;
	if (goSelectMode) {
		isBackgroundSound = false;
		isMotorSound = false;
		if (motorSoundThread.joinable()) {
			motorSoundThread.join();
		}
		goSelectMode();
	}
	isBoosterRegenActive = false;
	if (boosterRegenThread.joinable()) {
		boosterRegenThread.join();
	}
}

void Map2_Mode::finish_game() {
	if (finish_cnt != 0) {
		isBackgroundSound = false;
		if (isWinSound) return;
		isWinSound = true;
		isGameOver = true;

		winSoundThread = std::thread([this]() {
			win_sound();
			isWinSound = false;
			});

		winSoundThread.detach();

		std::thread([this]() {
			std::this_thread::sleep_for(std::chrono::seconds(9));
			goSelectMode_();
			}).detach();
	}
	else {
		return;
	}
}

void Map2_Mode::draw_finish_time(float deltaTime) {
	glUseProgram(shaderProgramID_UI);

	GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isRed");
	glUniform1i(isTimerLocation, true);

	std::string Text = "Time: " + std::to_string(30 - deltaTime);

	glRasterPos2f(0.0f, 0.0f);
	for (char c : Text) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glUniform1i(isTimerLocation, false);

	glUseProgram(0);
}

void Map2_Mode::lose_game() {
	if (isGameOver) return;

	isGameOver = true;
	isBackgroundSound = false;

	std::cout << "Game Over! Time is up!" << std::endl;

	if (!isLoseSound) {
		isLoseSound = true;
		loseSoundThread = std::thread([this]() {
			lose_sound();
			isLoseSound = false;
			});
		loseSoundThread.detach();

		std::thread([this]() {
			std::this_thread::sleep_for(std::chrono::seconds(9));
			goSelectMode_();
			}).detach();
	}
}

void Map2_Mode::checkCollisionKart() {
	for (auto& kart : karts) {
		if (kart->name != "car") continue;

		kart->rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));

		for (const auto& barri : road2_barricate) {
			CustomContactResultCallback resultCallback;

			dynamicsWorld->contactPairTest(kart->rigidBody, barri->rigidBody, resultCallback);

			if (resultCallback.hitDetected) {

				if (barri->name == "finish_ch") {
					finish_cnt++;
					continue;
				}

				if (barri->name == "finish") {
					finish_game();
					continue;
				}

				if (!isCrashSound) {
					isCrashSound = true;
					crashSoundThread = std::thread(&Map2_Mode::crash_sound, this);
					crashSoundThread.detach();
				}

				btVector3 collisionNormal = resultCallback.collisionNormal;
				collisionNormal.setY(0.0f);

				float penetrationDepth = std::abs(resultCallback.penetrationDepth);
				const float MAX_PENETRATION_DEPTH = 2.0f;
				if (penetrationDepth > MAX_PENETRATION_DEPTH) {
					penetrationDepth = MAX_PENETRATION_DEPTH;
				}

				float correctionScale = 0.5f;
				btVector3 correction = correctionScale * collisionNormal * penetrationDepth;

				glm::vec3 kartVelocity = glm::vec3(-kart->translateMatrix[2]) * kart_speed;
				float speedFactor = glm::length(kartVelocity);
				correction += collisionNormal * speedFactor * 0.2f;

				btTransform kartTransform;
				kart->rigidBody->getMotionState()->getWorldTransform(kartTransform);
				btVector3 kartPos = kartTransform.getOrigin();

				btVector3 newKartPos = kartPos + correction;
				newKartPos.setY(1.0f);

				kartTransform.setOrigin(newKartPos);

				kart->rigidBody->getMotionState()->setWorldTransform(kartTransform);
				kart->rigidBody->setWorldTransform(kartTransform);

				btScalar transformMatrix[16];
				kartTransform.getOpenGLMatrix(transformMatrix);
				kart->translateMatrix = glm::make_mat4(transformMatrix);

				float decelerationFactor = 0.2f;
				kart_speed *= 1.0f - decelerationFactor;
				if (kart_speed < 0.01f) {
					kart_speed = 0.0f;
				}
			}
		}
	}
}

void Map2_Mode::checkEngineSound() {
	if (kart_speed != 0.0f) {
		if (!isMotorSound) {
			isMotorSound = true;
			motorSoundThread = std::thread(&Map2_Mode::engine_sound, this);
		}
	}
	else {
		if (isMotorSound) {
			isMotorSound = false;
			if (motorSoundThread.joinable()) {
				motorSoundThread.detach();
			}
		}
	}
}

void Map2_Mode::timer() {
	if (!Pause) {
		if (start_count < 4) {
			if (start_count >= 0)
				playCountdown(start_count);
			++start_count;
		}
		else {

			if (kart_keyState[UP]) {
				if (kart_speed < MAX_SPEED) {
					kart_speed += ACCELERATION;
					if (kart_speed > MAX_SPEED) kart_speed = MAX_SPEED;
				}
			}
			else if (kart_keyState[DOWN]) {
				if (kart_speed > -MAX_SPEED / 2.0f) {
					kart_speed -= ACCELERATION;
					if (kart_speed < -MAX_SPEED / 2.0f) kart_speed = -MAX_SPEED / 2.0f;
				}
			}
			else {
				if (kart_speed > 0.0f) {
					kart_speed -= DECELERATION;
					if (kart_speed < 0.0f) kart_speed = 0.0f;
				}
				else if (kart_speed < 0.0f) {
					kart_speed += DECELERATION;
					if (kart_speed > 0.0f) kart_speed = 0.0f;
				}
			}

			if (kart_speed > MAX_SPEED) kart_speed = MAX_SPEED;

			if (kart_speed > 0.0f) {
				for (const auto& kart : karts) {
					kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
				}
			}
			else if (kart_speed < 0.0f) {
				for (const auto& kart : karts) {
					kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
				}
			}

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

			for (const auto& c : character) {
				c->translateMatrix = karts[0]->translateMatrix;
			}

			if (kart_speed != 0.0f) {
				reducedRotationInfluence = 0.1f + (std::abs(kart_speed) / MAX_SPEED) * 0.4f;
			}
			else {
				reducedRotationInfluence += 0.01f;
				if (reducedRotationInfluence > 1.0f) reducedRotationInfluence = 1.0f;
			}

			if (!kart_keyState[LEFT] && !kart_keyState[RIGHT]) {
				if (character_face_rotation > 0.0f) {
					character_face_rotation -= RETURN_SPEED;
					if (character_face_rotation < 0.0f) {
						character_face_rotation = 0.0f;
					}
				}
				else if (character_face_rotation < 0.0f) {
					character_face_rotation += RETURN_SPEED;
					if (character_face_rotation > 0.0f) {
						character_face_rotation = 0.0f;
					}
				}
			}

			if (isBoosterActive) {
				if (booster_head_tilt < MAX_HEAD_TILT) {
					booster_head_tilt += TILT_SPEED;
					if (booster_head_tilt > MAX_HEAD_TILT) {
						booster_head_tilt = MAX_HEAD_TILT;
					}
				}
			}
			else {
				if (booster_head_tilt > 0.0f) {
					booster_head_tilt -= TILT_SPEED;
					if (booster_head_tilt < 0.0f) {
						booster_head_tilt = 0.0f;
					}
				}
			}

			for (const auto& c : character) {
				if (c->name == "character_face") {
					glm::mat4 headRotation = glm::rotate(
						glm::mat4(1.0f),
						glm::radians(-character_face_rotation),
						glm::vec3(0.0f, 0.0f, 1.0f)
					);

					headRotation = glm::rotate(
						headRotation,
						glm::radians(booster_head_tilt),
						glm::vec3(1.0f, 0.0f, 0.0f)
					);

					c->translateMatrix = karts[0]->translateMatrix * headRotation;
				}
				else {
					c->translateMatrix = karts[0]->translateMatrix;
				}
			}

			setCamera();
			float cameraFollowSpeed = 0.1f;
			cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

			checkCollisionKart();
			checkEngineSound();
		}
	}
}

void Map2_Mode::mouseClick(int button, int state, int x, int y)  {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (x <= 470 && x >= 400 && y <= 410 && y >= 360) {
			Pause = true;
			isBackgroundSound = false;
			isMotorSound = false;
			if (motorSoundThread.joinable()) {
				motorSoundThread.join();
			}
			Map2_Mode* map2Mode = new Map2_Mode();
			map2Mode->goSelectMode = [this]() { goSelectMode(); };
			MM.SetMode(map2Mode);
		}
		else if (x <= 580 && x >= 510 && y <= 410 && y >= 360) {
			goSelectMode_();
		}
	}
}

void Map2_Mode::keyboard(unsigned char key, int x, int y)  {
	if (key == 27) { //esc
		if (Pause) {
			//glutTimerFunc(16, timerHelper, 0);
			//isMotorSound = true;
		}
		else {
			glm::vec3 zAxis = glm::normalize(cameraPos - glm::vec3(karts[0]->translateMatrix[3]));
			glm::vec3 xAxis = glm::normalize(glm::cross(cameraUp, zAxis));
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);
			glm::mat3 rotationMatrix = glm::mat3(
				xAxis, // X
				yAxis, // Y
				zAxis  // Z
			);
			// 4x4
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix[0] = glm::vec4(rotationMatrix[0], 0.0f); // X
			modelMatrix[1] = glm::vec4(rotationMatrix[1], 0.0f); // Y
			modelMatrix[2] = glm::vec4(rotationMatrix[2], 0.0f); // Z
			modelMatrix[3] = glm::vec4(cameraPos, 1.0f);
			pause[0]->translateMatrix = modelMatrix;
			pause[0]->translateMatrix = glm::translate(pause[0]->translateMatrix, glm::vec3(0.0, 0.0, -2.0));


			isMotorSound = false;
			if (motorSoundThread.joinable()) {
				motorSoundThread.detach();
			}
		}
		Pause = !Pause;
	}
	if (key == 'p') {
		goSelectMode_();
	}
}

void Map2_Mode::activateBooster() {

	if (isBoosterActive) {
		std::cout << "Booster is already active!" << std::endl;
		return;
	}

	isBoosterActive = true;

	std::cout << "Booster activated! Remaining boosters: " << booster_cnt << std::endl;

	float originalMaxSpeed = MAX_SPEED;
	float originalAcceleration = ACCELERATION;

	MAX_SPEED = BOOSTER_SPEED;
	ACCELERATION *= 1.05f;

	if (!isBoosterSound) {
		isBoosterSound = true;
		boosterSoundThread = std::thread(&Map2_Mode::booster_sound, this);
		boosterSoundThread.detach();
	}

	std::thread([this, originalMaxSpeed, originalAcceleration]() {
		std::this_thread::sleep_for(std::chrono::duration<double>(4.4));
		MAX_SPEED = originalMaxSpeed;
		ACCELERATION = originalAcceleration;
		isBoosterActive = false;
		std::cout << "Booster ended. MAX_SPEED and ACCELERATION restored." << std::endl;
		}).detach();
}

void Map2_Mode::specialKey(int key, int x, int y)  {


	switch (key) {
	case GLUT_KEY_UP:
		kart_keyState[UP] = true;
		break;
	case GLUT_KEY_DOWN:
		kart_keyState[DOWN] = true;
		break;
	case GLUT_KEY_LEFT:
		kart_keyState[LEFT] = true;
		if (character_face_rotation > -MAX_FACE_ROTATION) {
			character_face_rotation -= ROTATION_SPEED;
		}
		break;
	case GLUT_KEY_RIGHT:
		kart_keyState[RIGHT] = true;
		if (character_face_rotation < MAX_FACE_ROTATION) {
			character_face_rotation += ROTATION_SPEED;
		}
		break;
	}

	int modifiers = glutGetModifiers();

	if (modifiers & GLUT_ACTIVE_CTRL) {
		if (isBoosterActive) {
			std::cout << "Booster is already active!" << std::endl;
			return;
		}

		if (booster_cnt > 0) {
			booster_cnt--;
			activateBooster();
		}
		else {
			std::cout << "No boosters left!" << std::endl;
		}
	}

}

void Map2_Mode::specialKeyUp(int key, int x, int y)  {
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

void Map2_Mode::draw_model()  {

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

	for (const auto& kart : karts) {
		kart->draw(shaderProgramID, isKeyPressed_s);
	}
	for (const auto& road : road2) {
		road->draw(shaderProgramID, isKeyPressed_s);
	}
	for (const auto& c : character) {
		if (c->name == "booster" && !isBoosterActive)
			continue;
		c->draw(shaderProgramID, isKeyPressed_s);
	}
	for (const auto& barricate : road2_barricate) {
		barricate->draw(shaderProgramID, isKeyPressed_s);
	}
	if (start_count >= 0 && start_count < 4) {
		countDown[start_count]->draw(shaderProgramID, isKeyPressed_s);
	}

	if (Pause)
		pause[0]->draw(shaderProgramID, isKeyPressed_s);

	// Draw Timer
	glDisable(GL_DEPTH_TEST);
	draw_timer(g_delta_time);
	draw_ui();
	draw_dashBoard();
	draw_speed();
	if (isGameOver)
		draw_finish_time(g_delta_time);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_DEPTH_TEST);
}

void Map2_Mode::draw_bb()  {
	if (!bb_status)
		return;
	for (const auto& model : karts) {
		model->draw_rigidBody(shaderProgramID);
	}
	for (const auto& barricate : road2_barricate) {
		barricate->draw_rigidBody(shaderProgramID);
	}
}

void Map2_Mode::finish()  {

}

void Map2_Mode::updatePhysics(float deltaTime) {
	dynamicsWorld->stepSimulation(deltaTime);

	UpdateRigidBodyTransforms(karts);
	UpdateRigidBodyTransforms(road2_barricate);

	checkCollisionKart();
}

 void Map2_Mode::timerHelper(int value) {
	if (Map2_Mode* instance = dynamic_cast<Map2_Mode*>(Mode::currentInstance)) {
		const float deltaTime = 1.0f / 60.0f;

		instance->updatePhysics(deltaTime);
		instance->timer();
	}

	glutPostRedisplay();
	glutTimerFunc(16, timerHelper, value);
}

void Map2_Mode::backgroundSound() {
	play_sound2D("village_04.ogg", "./asset/map_1/", true, &isBackgroundSound);
}

void Map2_Mode::count_go() {
	play_sound2D("count_go.wav", "./asset/map_1/", false, &isCountGoSound);
}
void Map2_Mode::count_n() {
	play_sound2D("count_n.wav", "./asset/map_1/", false, &isCountNSound);
}
void Map2_Mode::engine_sound() {
	play_sound2D("motor_x.ogg", "./asset/map_1/", true, &isMotorSound);
}
void Map2_Mode::crash_sound() {
	play_sound2D("crash.ogg", "./asset/map_1/", false, &isCrashSound);
	isCrashSound = false;
}
void Map2_Mode::booster_sound() {
	play_sound2D("booster.ogg", "./asset/map_1/", false, &isBoosterSound);
	isBoosterSound = false;
}
void Map2_Mode::win_sound() {
	play_sound2D("game_win.ogg", "./asset/map_1/", false, &isWinSound);
	isWinSound = false;
}
void Map2_Mode::lose_sound() {
	play_sound2D("game_lose.ogg", "./asset/map_1/", false, &isLoseSound);
	isLoseSound = false;
}