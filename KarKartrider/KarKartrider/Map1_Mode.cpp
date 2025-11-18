#include "Pch.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "LoadProgress.h"
#include "Light.h"
#include "SelectMapMode.h"
#include "CustomContactResultCallback.h"
#include "BulletPhysics.h"
#include "Map1_Mode.h"

Map1_Mode::Map1_Mode()
{
	Mode::currentInstance = this;
	isCountNSound = true;
	isCountGoSound = true;

	boosterRegenThread = std::thread(&Map1_Mode::startBoosterRegen, this);
}

void Map1_Mode::startBoosterRegen()
{
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

void Map1_Mode::draw_dashBoard()
{
	glUseProgram(shaderProgramID_UI);


	GLint isTextureLocation = glGetUniformLocation(shaderProgramID_UI, "isTexture");
	glUniform1i(isTextureLocation, true);


	for (const auto& dashBoard : dashBoards) {
		dashBoard->draw(shaderProgramID_UI, isKeyPressed_s);
	}
	glUniform1i(isTextureLocation, false);

	glUseProgram(0);
}

void Map1_Mode::draw_speed()
{
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

void Map1_Mode::draw_ui() {
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

void Map1_Mode::draw_timer(float deltaTime) {
	glUseProgram(shaderProgramID_UI);


	GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
	glUniform1i(isTimerLocation, true);

	/*std::string timerText = "Time: " + std::to_string(deltaTime);
	glRasterPos2f(-0.95f, 0.9f);
	for (char c : timerText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}*/

	/*auto RenderText = [](float x, float y, std::string text) {
		glRasterPos2f(x, y);
		for (char c : text) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		};

	std::string dtText = "Time: " + std::to_string(deltaTime);
	RenderText(-0.95f, 0.90f, dtText);

	std::string rankText = "Rank: " + std::to_string(g_players[g_myid].m_rank) + " / " + std::to_string(MAX_USER);
	RenderText(-0.95f, 0.80f, rankText);*/


	glUniform1i(isTimerLocation, false);

	glUseProgram(0);
}

void Map1_Mode::init()
{

	UpdateRigidBodyTransforms(road1_barricate);
	UpdateRigidBodyTransforms(karts);
	
	/*std::cout << "---------------------- Road1 Barricate Coordinates ----------------------" << std::endl;

	for (auto& model : road1_barricate) {
		if (!model || !model->rigidBody) continue;

		float x = model->translateMatrix[3][0];
		float y = model->translateMatrix[3][1];
		float z = model->translateMatrix[3][2];

		std::cout << "Barricate  Pos : "
			<< x << ", " << y << ", " << z << std::endl;
	}

	std::cout << "---------------------- Road1 Barricate Coordinates ----------------------" << std::endl;*/

	kart_keyState[UP] = false;
	kart_keyState[DOWN] = false;
	kart_keyState[LEFT] = false;
	kart_keyState[RIGHT] = false;

	start_count = -1;

	Pause = false;

	for (const auto& kart : karts) {
		kart->translateMatrix = glm::mat4(1.0f);
		kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 2.6, 238.0));
	}
	for (const auto& c : character) {
		c->translateMatrix = karts[0]->translateMatrix;
	}
	for (const auto& c : countDown) {
		c->translateMatrix = karts[0]->translateMatrix;
		c->translateMatrix = glm::translate(c->translateMatrix, glm::vec3(0.0, 4.0, 0.0));
	}

	isBackgroundSound = true;
	backgroundSoundThread = std::thread(&Map1_Mode::backgroundSound, this);

	kart_speed = 0.0f;
	draw_model();
	if (!isGameRunning)
	{
		isGameRunning = true;
		glutTimerFunc(0, Map1_Mode::timerHelper, 0);
	}

	cameraPos = glm::vec3(0.0, 6.0, 253.0);
	updateCameraDirection();
}

void Map1_Mode::playCountdown(int count) {
	if (count < 3) {
		if (countNSoundThread.joinable()) {
			countNSoundThread.join();
		}

		countNSoundThread = std::thread(&Map1_Mode::count_n, this);

		countNSoundThread.join();
	}
	else if (count == 3) {
		if (countGoSoundThread.joinable()) {
			countGoSoundThread.join();
		}

		countGoSoundThread = std::thread(&Map1_Mode::count_go, this);


		//isCountGoSound = false;
		isCountNSound = false;
		isBackgroundSound = true;
	}
}

void Map1_Mode::updateCameraDirection()
{
	glm::mat3 rotationMatrix = glm::mat3(karts[0]->translateMatrix);


	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));


	glm::vec3 rotatedDirection = rotationMatrix * direction;


	glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

	cameraDirection = glm::normalize(rotatedDirection) + carPosition;
}

void Map1_Mode::setCamera()
{
	glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);


	glm::mat3 carRotationMatrix = glm::mat3(karts[0]->translateMatrix);


	glm::quat carRotationQuat = glm::quat_cast(carRotationMatrix);

	glm::quat interpolatedRotation = glm::slerp(cameraRotationQuat, carRotationQuat, reducedRotationInfluence);
	cameraRotationQuat = interpolatedRotation;


	glm::mat3 adjustedRotationMatrix = glm::mat3_cast(interpolatedRotation);


	glm::vec3 baseOffset = glm::vec3(0.0f, 6.0f + (g_players[g_myid].m_speed * 2.0f), 14.0f + (g_players[g_myid].m_speed * 10.0f));
	glm::vec3 rotatedOffset = adjustedRotationMatrix * baseOffset;


	cameraTargetPos = carPosition + rotatedOffset;


	float cameraFollowSpeed = 0.1f;
	cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);


	cameraDirection = carPosition;
}

void Map1_Mode::goSelectMode_() {
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

void Map1_Mode::finish_game() {
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

void Map1_Mode::draw_finish_time(float deltaTime) {
	glUseProgram(shaderProgramID_UI);


	GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isRed");
	glUniform1i(isTimerLocation, true);


	std::string Text = "Time: " + std::to_string(deltaTime);

	//glRasterPos2f(0.0f, 0.0f);
	//for (char c : Text) {
	//	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	//}

	auto RenderText = [](float x, float y, std::string text) {
		glRasterPos2f(x, y);
		for (char c : text) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		};

	std::string dtText = "Time: " + std::to_string(deltaTime);
	RenderText(0.0f, 0.0f, dtText);

	std::string rankText = "Rank: " + std::to_string(g_players[g_myid].m_rank) + " / " + std::to_string(MAX_USER);
	RenderText(0.0f, 0.1f, rankText);


	glUniform1i(isTimerLocation, false);

	glUseProgram(0);
}

void Map1_Mode::lose_game() {
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

void Map1_Mode::checkCollisionKart() {
	for (auto& kart : karts) {
		if (kart->name != "car") continue;

		/*glm::vec3 size = calculateModelSize(kart);
		std::cout << "KART SIZE: " << size.x << ", " << size.y << ", " << size.z << std::endl;*/

		kart->rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));

		for (const auto& barri : road1_barricate) {
			CustomContactResultCallback resultCallback;
			dynamicsWorld->contactPairTest(kart->rigidBody, barri->rigidBody, resultCallback);
			if (!resultCallback.hitDetected) continue;

			// 결승선
			if (barri->name == "finish") {
				std::cout << "결승선 도착!" << std::endl;
				finish_game();
				continue;
			}

			// 사운드
			if (!isCrashSound) {
				isCrashSound = true;
				crashSoundThread = std::thread(&Map1_Mode::crash_sound, this);
				crashSoundThread.detach();
			}

			// --- 자연스러운 충돌 처리 ---
			btVector3 collisionNormal = resultCallback.collisionNormal;
			collisionNormal.setY(0.0f);
			collisionNormal.normalize();

			float penetrationDepth = std::abs(resultCallback.penetrationDepth);
			if (penetrationDepth < 0.05f) continue; // 너무 얕은 충돌 무시

			// 위로 살짝 보정하여 벽 껌딱지 방지
			btVector3 adjustDir = collisionNormal + btVector3(0.0f, 0.2f, 0.0f);
			adjustDir.normalize();

			float correctionScale = 0.3f;
			btVector3 correction = adjustDir * penetrationDepth * correctionScale;

			// transform 반영
			btTransform kartTransform;
			kart->rigidBody->getMotionState()->getWorldTransform(kartTransform);
			btVector3 kartPos = kartTransform.getOrigin();
			btVector3 newKartPos = kartPos + correction;
			newKartPos.setY(2.6f);
			kartTransform.setOrigin(newKartPos);
			kart->rigidBody->setWorldTransform(kartTransform);
			kart->rigidBody->getMotionState()->setWorldTransform(kartTransform);

			btScalar transformMatrix[16];
			kartTransform.getOpenGLMatrix(transformMatrix);
			kart->translateMatrix = glm::make_mat4(transformMatrix);

			// --- 감속 처리 ---
			glm::vec3 forwardDir = glm::normalize(glm::vec3(-kart->translateMatrix[2]));
			glm::vec3 normalDir = glm::vec3(collisionNormal.getX(), 0.0f, collisionNormal.getZ());
			float impact = glm::dot(forwardDir, normalDir);
			impact = glm::clamp(impact, 0.0f, 1.0f);

			float decelerationFactor = 0.25f * impact + 0.1f;
			g_players[g_myid].m_speed *= 1.0f - decelerationFactor;

			// 완전 멈추지 않게 최소 움직임 유지 (벽에서 빠져나오기 쉽게)
			if (g_players[g_myid].m_speed < 0.01f)
				g_players[g_myid].m_speed = 0.01f;
		}
	}
}


void Map1_Mode::checkEngineSound() {
	if (g_players[g_myid].m_speed != 0.0f) {
		if (!isMotorSound) {
			isMotorSound = true;
			motorSoundThread = std::thread(&Map1_Mode::engine_sound, this);
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

void Map1_Mode::timer() {

	std::cout << "Server Z = " << g_players[g_myid].z << std::endl;

	int id = g_myid;

	// -------------------------
	// 1) 서버에서 받은 XZ 사용
	// -------------------------
	glm::vec3 serverXZ(
		g_players[id].x,
		0.0f,             // Y는 아직 unknown, 각 부품마다 유지됨
		g_players[id].z
	);

	// -------------------------
	// 2) 모든 부품(karts)을 동일한 서버 위치로 동기화
	// -------------------------
	for (auto& kart : karts)
	{
		float currentY = kart->translateMatrix[3].y;  // 부품 고유 Y 유지

		glm::vec3 finalPos(
			serverXZ.x,
			currentY,      // 높이는 유지
			serverXZ.z
		);

		kart->translateMatrix = glm::mat4(1.0f);
		kart->translateMatrix = glm::translate(kart->translateMatrix, finalPos);
	}

	// -------------------------
	// 3) 모든 부품 yaw 회전 적용
	// -------------------------
	for (auto& kart : karts)
	{
		kart->translateMatrix = glm::rotate(
			kart->translateMatrix,
			glm::radians(g_players[id].m_yaw),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}

	// -------------------------
	// 4) 얼굴 회전/기타 부품 처리
	// -------------------------
	for (auto& c : character)
	{
		if (c->name == "character_face")
		{
			glm::mat4 headRot = glm::rotate(
				glm::mat4(1.0f),
				glm::radians(-g_players[id].m_face_rotation),
				glm::vec3(0, 0, 1)
			);

			headRot = glm::rotate(
				headRot,
				glm::radians(booster_head_tilt),
				glm::vec3(1, 0, 0)
			);

			c->translateMatrix = karts[0]->translateMatrix * headRot;
		}
		else
		{
			c->translateMatrix = karts[0]->translateMatrix;
		}
	}

	// -------------------------
	// 5) 카메라
	// -------------------------
	setCamera();
	cameraPos = glm::mix(cameraPos, cameraTargetPos, 0.1f);

	// -------------------------
	// 6) 사운드
	// -------------------------
	checkEngineSound();
}

void Map1_Mode::mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (x <= 470 && x >= 400 && y <= 410 && y >= 360) { //
			Pause = true;
			isBackgroundSound = false;
			isMotorSound = false;
			if (motorSoundThread.joinable()) {
				motorSoundThread.join();
			}
			Map1_Mode* map1Mode = new Map1_Mode();
			map1Mode->goSelectMode = [this]() { goSelectMode(); }; //
			MM.SetMode(map1Mode);
		}
		else if (x <= 580 && x >= 510 && y <= 410 && y >= 360) { //
			goSelectMode_();
		}
	}
}

void Map1_Mode::keyboard(unsigned char key, int x, int y) {
	if (key == 27) { //esc
		if (Pause) {
			//glutTimerFunc(16, timerHelper, 0); //
			//isMotorSound = true;
		}
		else {
			glm::vec3 zAxis = glm::normalize(cameraPos - glm::vec3(karts[0]->translateMatrix[3]));
			// 
			glm::vec3 xAxis = glm::normalize(glm::cross(cameraUp, zAxis));
			//
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);
			// 3x3
			glm::mat3 rotationMatrix = glm::mat3(
				xAxis, // X
				yAxis, // Y
				zAxis  // 
			);

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix[0] = glm::vec4(rotationMatrix[0], 0.0f);
			modelMatrix[1] = glm::vec4(rotationMatrix[1], 0.0f);
			modelMatrix[2] = glm::vec4(rotationMatrix[2], 0.0f);
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


void Map1_Mode::activateBooster() {


	//if (isBoosterActive) {
	//	std::cout << "Booster is already active!" << std::endl;
	//	return;
	//}


	//isBoosterActive = true;

	//std::cout << "Booster activated! Remaining boosters: " << booster_cnt << std::endl;


	//float originalMaxSpeed = MAX_SPEED;
	//float originalAcceleration = ACCELERATION;


	//MAX_SPEED = BOOSTER_SPEED;
	//ACCELERATION *= 1.05f;


	//if (!isBoosterSound) {
	//	isBoosterSound = true;
	//	boosterSoundThread = std::thread(&Map1_Mode::booster_sound, this);
	//	boosterSoundThread.detach();
	//}


	//std::thread([this, originalMaxSpeed, originalAcceleration]() {
	//	std::this_thread::sleep_for(std::chrono::duration<double>(4.4));
	//	MAX_SPEED = originalMaxSpeed;
	//	ACCELERATION = originalAcceleration;
	//	isBoosterActive = false;
	//	std::cout << "Booster ended. MAX_SPEED and ACCELERATION restored." << std::endl;
	//	}).detach();
}

void Map1_Mode::specialKey(int key, int x, int y) {


	switch (key) {
	case GLUT_KEY_UP: {
		C2S_Move_Packet* packet = new C2S_Move_Packet;
		packet->type = C2S_MOVE;
		packet->key_type = KEY_TYPE::UP;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
		delete packet;
	}
					break;
	case GLUT_KEY_DOWN: {
		C2S_Move_Packet* packet = new C2S_Move_Packet;
		packet->type = C2S_MOVE;
		packet->key_type = KEY_TYPE::DOWN;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
		delete packet;
	}
					  break;
	case GLUT_KEY_LEFT: {
		C2S_Move_Packet* packet = new C2S_Move_Packet;
		packet->type = C2S_MOVE;
		packet->key_type = KEY_TYPE::LEFT;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
		delete packet;
	}
					  break;
	case GLUT_KEY_RIGHT: {
		C2S_Move_Packet* packet = new C2S_Move_Packet;
		packet->type = C2S_MOVE;
		packet->key_type = KEY_TYPE::RIGHT;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
		delete packet;
	}
					   break;
	}

	int modifiers = glutGetModifiers();

	// Ctrl
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

void Map1_Mode::specialKeyUp(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
	{
		C2S_Move_Packet* packet = new C2S_Move_Packet;
		packet->type = C2S_MOVE;
		packet->key_type = KEY_TYPE::UP_RELEASED;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
		delete packet;
	}
	case GLUT_KEY_DOWN:
	{
		C2S_Move_Packet* packet = new C2S_Move_Packet;
		packet->type = C2S_MOVE;
		packet->key_type = KEY_TYPE::DOWN_RELEASED;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
		delete packet;
	}
	case GLUT_KEY_LEFT:
	{
		C2S_Move_Packet* packet = new C2S_Move_Packet;
		packet->type = C2S_MOVE;
		packet->key_type = KEY_TYPE::LEFT_RELEASED;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
		delete packet;
	}
	case GLUT_KEY_RIGHT:
	{
		C2S_Move_Packet* packet = new C2S_Move_Packet;
		packet->type = C2S_MOVE;
		packet->key_type = KEY_TYPE::RIGHT_RELEASED;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
		delete packet;
	}
	break;
	}

}

void Map1_Mode::draw_model() {

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
	for (const auto& road : road1) {
		road->draw(shaderProgramID, isKeyPressed_s);
	}
	for (const auto& c : character) {
		if (c->name == "booster" && !isBoosterActive)
			continue;
		c->draw(shaderProgramID, isKeyPressed_s);
	}
	for (const auto& barricate : road1_barricate) {
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

void Map1_Mode::draw_bb() {
	if (!bb_status)
		return;
	for (const auto& model : karts) { // �� bb draw
		model->draw_rigidBody(shaderProgramID);
	}
	for (const auto& barricate : road1_barricate) { // �� bb draw
		barricate->draw_rigidBody(shaderProgramID);
	}
}

void Map1_Mode::finish() {

}


void Map1_Mode::updatePhysics(float deltaTime) {
	// ���� ���� ������Ʈ (deltaTime�� ���� ��Ȯ�� ����)
	dynamicsWorld->stepSimulation(deltaTime);

	// ���� �������� ��ü�� Transform ������Ʈ
	UpdateRigidBodyTransforms(karts);
	UpdateRigidBodyTransforms(road1_barricate);

	// �浹 ó�� (���� ���� ������Ʈ �� ����)
	checkCollisionKart();
}

void Map1_Mode::timerHelper(int value) {
	if (Map1_Mode* instance = dynamic_cast<Map1_Mode*>(Mode::currentInstance)) {
		const float deltaTime = 1.0f / 60.0f; // 60FPS ����, �� �������� �ð�

		// ���� ���� �� ���� ���� ������Ʈ (���� ���� ���¿����� ��� ����)
		instance->updatePhysics(deltaTime); // ���� ���� ������Ʈ
		instance->timer(); // ������ �� ���� ���� ������Ʈ

	}

	//
	glutPostRedisplay();
	glutTimerFunc(16, timerHelper, value); //
}



void Map1_Mode::backgroundSound() {
	play_sound2D("village_04.ogg", "./asset/map_1/", true, &isBackgroundSound);
}
void Map1_Mode::count_go() {
	play_sound2D("count_go.wav", "./asset/map_1/", false, &isCountGoSound);
}
void Map1_Mode::count_n() {
	play_sound2D("count_n.wav", "./asset/map_1/", false, &isCountNSound);
}
void Map1_Mode::engine_sound() {
	play_sound2D("motor_x.ogg", "./asset/map_1/", true, &isMotorSound);
}
void Map1_Mode::crash_sound() {
	play_sound2D("crash.ogg", "./asset/map_1/", false, &isCrashSound);
	isCrashSound = false;
}
void Map1_Mode::booster_sound() {
	play_sound2D("booster.ogg", "./asset/map_1/", false, &isBoosterSound);
	isBoosterSound = false;
}
void Map1_Mode::win_sound() {
	play_sound2D("game_win.ogg", "./asset/map_1/", false, &isWinSound);
	isWinSound = false;
}
void Map1_Mode::lose_sound() {
	play_sound2D("game_lose.ogg", "./asset/map_1/", false, &isLoseSound);
	isLoseSound = false;
}