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

	std::string speedText = std::to_string(static_cast<int>(g_players[g_myid].m_speed * 100));


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
	glm::mat3 rotationMatrix = glm::mat3(myKartMatrix);

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	glm::vec3 rotatedDirection = rotationMatrix * direction;

	glm::vec3 carPosition = glm::vec3(myKartMatrix[3]);

	cameraDirection = glm::normalize(rotatedDirection) + smoothedCarPos;
}

void Map1_Mode::setCamera()
{
	glm::vec3 carPosition = glm::vec3(myKartMatrix[3]);
	glm::mat3 carRotationMatrix = glm::mat3(myKartMatrix);
	glm::quat carRotationQuat = glm::quat_cast(carRotationMatrix);

	float rawSpeed = g_players[g_myid].m_speed;

	if (bFirstCameraFrame)
	{
		smoothedCarPos = carPosition;
		smoothedCarRot = carRotationQuat;
		smoothedSpeed = rawSpeed;
		cameraPos = carPosition + carRotationQuat * glm::vec3(0.0f, 6.0f, 14.0f);
		cameraRotationQuat = carRotationQuat;
		bFirstCameraFrame = false;
	}

	smoothedCarPos = glm::mix(smoothedCarPos, carPosition, carPosLerp);
	smoothedCarRot = glm::slerp(smoothedCarRot, carRotationQuat, carRotLerp);
	smoothedCarRot = glm::normalize(smoothedCarRot);

	smoothedSpeed = glm::mix(smoothedSpeed, rawSpeed, speedLerp);

	glm::vec3 baseOffset = glm::vec3(
		0.0f,
		6.0f + (smoothedSpeed * 2.0f),
		14.0f + (smoothedSpeed * 10.0f)
	);

	glm::vec3 rotatedOffset = smoothedCarRot * baseOffset;

	cameraTargetPos = smoothedCarPos + rotatedOffset;

	float cameraFollowSpeed = 0.1f;
	cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

	cameraDirection = smoothedCarPos;
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

	// ================
   // 1) 내 위치 보간
   // ================
	float x = g_players[g_myid].x;
	float y = g_players[g_myid].y;
	float z = g_players[g_myid].z;

	glm::vec3 targetPos(x, y, z);

	if (g_firstRenderFrame)
	{
		g_kartRenderPos = targetPos;
		g_firstRenderFrame = false;
	}

	const float posLerp = 0.3f;
	g_kartRenderPos = glm::mix(g_kartRenderPos, targetPos, posLerp);

	// ================
	// 2) 네트워크 입력 송신
	// ================
	networkmgr.SendMovePacket(up, down, left, right);

	// ================
	// 3) 카메라 추적 계산
	// ================
	setCamera();

	// ================
	// 4) 충돌, 사운드 처리
	// ================
	checkCollisionKart();
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
		up = true;
	}
					break;
	case GLUT_KEY_DOWN: {
		down = true;
	}
					  break;
	case GLUT_KEY_LEFT: {
		left = true;
	}
					  break;
	case GLUT_KEY_RIGHT: {
		right = true;
	}
					   break;
	}

	int modifiers = glutGetModifiers();

	// Ctrl
	if (modifiers & GLUT_ACTIVE_CTRL) {
		if (!ctrl)
		{
			ctrl = true;
		}

		booster = true;
		networkmgr.SendBoosterPacket(booster);
	}

}

void Map1_Mode::specialKeyUp(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
	{
		up = false;
	}
	break;
	case GLUT_KEY_DOWN:
	{
		down = false;
	}
	break;
	case GLUT_KEY_LEFT:
	{
		left = false;
	}
	break;
	case GLUT_KEY_RIGHT:
	{
		right = false;
	}
	break;
	case GLUT_ACTIVE_CTRL:
	{
		ctrl = false;
	}
	}

}


void Map1_Mode::RenderPlayer() {
	for (int pid = 0; pid < MAX_USER; pid++)
	{
		/*if (!g_players[pid].isOnline)
			continue;*/

		float px = g_players[pid].x;
		float py = g_players[pid].y;
		float pz = g_players[pid].z;
		float pyaw = g_players[pid].m_yaw;
		float pbody = g_players[pid].m_body_rotation;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(px, py, pz));
		model = glm::rotate(model, glm::radians(pyaw), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(pbody), glm::vec3(0, 0, 1));

		if (pid == g_myid)
			myKartMatrix = model;

		// --- 카트 파츠 ---
		for (auto& part : karts)
		{
			part->translateMatrix = model;
			part->draw(shaderProgramID, isKeyPressed_s);
		}

		// --- 캐릭터 파츠 ---
		for (auto& c : character)
		{
			glm::mat4 cm = model;

			if (c->name == "booster" && !isBoosterActive)
				continue;

			if (c->name == "character_face")
			{
				glm::mat4 headRot = glm::rotate(
					glm::mat4(1.0f),
					glm::radians(-g_players[pid].m_face_rotation),
					glm::vec3(0, 0, 1)
				);

				headRot = glm::rotate(
					headRot,
					glm::radians(g_players[pid].m_booster_head_tilt),
					glm::vec3(1, 0, 0)
				);

				cm = model * headRot;
			}

			c->translateMatrix = cm;
			c->draw(shaderProgramID, isKeyPressed_s);
		}
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

	RenderPlayer();

	for (const auto& road : road1) {
		road->draw(shaderProgramID, isKeyPressed_s);
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