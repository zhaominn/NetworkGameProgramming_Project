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
	/*while (isBoosterRegenActive) {
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
	}*/
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

	for (int i = 0; i < g_players[g_myid].m_booster_cnt; ++i) {
		booster_uis[i]->draw(shaderProgramID_UI, isKeyPressed_s);
	}
	glUniform1i(isTextureLocation, false);

	glUseProgram(0);
}

void Map2_Mode::draw_timer(float deltaTime) {
	glUseProgram(shaderProgramID_UI);

	GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
	glUniform1i(isTimerLocation, true);

	/*std::string timerText = "Time: " + std::to_string(deltaTime);
	glRasterPos2f(-0.95f, 0.9f);
	for (char c : timerText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}*/

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

	// wall collision
	AABB befor_data_aabb[18];

	for (int i = 0; i < 18; ++i) {
		btVector3 aabbMin, aabbMax;
		road2_barricate[i]->rigidBody->getAabb(aabbMin, aabbMax);

		/*std::cout << "AABB Min: " << aabbMin.getX() << ", "
			<< aabbMin.getY() << ", " << aabbMin.getZ() << std::endl;

		std::cout << "AABB Max: " << aabbMax.getX() << ", "
			<< aabbMax.getY() << ", " << aabbMax.getZ() << std::endl;*/

		befor_data_aabb[i].minX = aabbMin.getX();
		befor_data_aabb[i].minY = aabbMin.getY();
		befor_data_aabb[i].minZ = aabbMin.getZ();

		befor_data_aabb[i].maxX = aabbMax.getX();
		befor_data_aabb[i].maxY = aabbMax.getY();
		befor_data_aabb[i].maxZ = aabbMax.getZ();

		if (road2_barricate[i]->name == "finish")
		{
			befor_data_aabb[i].rigid_status = false;
			befor_data_aabb[i].collision = FINISH;
		}
		else if (road2_barricate[i]->name == "finish_ch") {
			befor_data_aabb[i].rigid_status = false;
			befor_data_aabb[i].collision = FINISH_CH;
		}
		else
		{
			befor_data_aabb[i].rigid_status = true;
			befor_data_aabb[i].collision = WALL;
		}
	}

	networkmgr.SendWallCollisionPacket_2(befor_data_aabb);

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

	std::string Text = "Time: " + std::to_string(deltaTime);

	//std::cout << "finish deltaTime = " << deltaTime << std::endl;

	/*glRasterPos2f(0.0f, 0.0f);
	for (char c : Text) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}*/

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
	/*for (auto& kart : karts) {
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
	}*/
}

void Map2_Mode::checkEngineSound() {
	if (g_players[g_myid].m_speed != 0.0f) {
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

	if (!Pause)
	{
		// ================
		// 2) 네트워크 입력 송신
		// ================
		networkmgr.SendMovePacket(up, down, left, right);

		// ================
		// 3) 카메라 추적 계산
		// ================
		setCamera();
	}

	// ================
	// 4) 사운드 처리
	// ================
	checkEngineSound();
}

void Map2_Mode::mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && Pause) {
		if (x <= 470 && x >= 400 && y <= 410 && y >= 360) {
			Pause = !Pause;
		}
		else if (x <= 580 && x >= 510 && y <= 410 && y >= 360) {
			networkmgr.SendLeaveRoomPacket(RECTANGLE);
		}
	}
}

void Map2_Mode::keyboard(unsigned char key, int x, int y) {
	if (key == 27 && g_GameEnd) { //esc
		if (Pause) {
			glutTimerFunc(16, timerHelper, 0);
			isMotorSound = true;
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
		//goSelectMode_();
	}
}

void Map2_Mode::activateBoosterSound() {

	if (!isBoosterSound) {
		isBoosterSound = true;
		boosterSoundThread = std::thread(&Map2_Mode::booster_sound, this);
		boosterSoundThread.detach();
	}
}

void Map2_Mode::specialKey(int key, int x, int y) {


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

	if (modifiers & GLUT_ACTIVE_CTRL) {
		if (!ctrl)
		{
			ctrl = true;
		}

		if (!g_players[g_myid].isBoosterOn && 
			g_players[g_myid].m_booster_cnt > 0) {
			g_players[g_myid].isBoosterOn = true;
			networkmgr.SendBoosterPacket(g_players[g_myid].isBoosterOn, g_players[g_myid].m_booster_cnt);
			activateBoosterSound();
		}
		else {
			std::cout << "No boosters left!" << std::endl;
		}
	}

}

void Map2_Mode::specialKeyUp(int key, int x, int y) {
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
	}
}

void Map2_Mode::RenderPlayer() {
	for (int pid = 0; pid < MAX_USER; pid++)
	{
		/*if (!g_players[pid].isOnline)
			continue;*/

		bool isInMyRoom = false;

		for (int id : g_roomPlayers)
		{
			if (pid == id)
			{
				isInMyRoom = true;
				break;
			}
		}

		if (!isInMyRoom)
			continue;

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

			if (c->name == "booster" && !g_players[pid].isBoosterOn)
				continue;
			c->draw(shaderProgramID, isKeyPressed_s);
		}
	}
}

void Map2_Mode::draw_model() {

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

	if (Pause && g_GameEnd)
		pause[0]->draw(shaderProgramID, isKeyPressed_s);

	// Draw Timer
	glDisable(GL_DEPTH_TEST);
	draw_timer(g_delta_time);
	draw_ui();
	draw_dashBoard();
	draw_speed();
	if (g_GameEnd)
		draw_finish_time(g_delta_time);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_DEPTH_TEST);
}

void Map2_Mode::draw_bb() {
	/*if (!bb_status)
		return;*/
		//for (const auto& model : karts) {
		//	model->draw_rigidBody(shaderProgramID);
		//}
		//for (const auto& barricate : road2_barricate) {
		//	barricate->draw_rigidBody(shaderProgramID);
		//}
}

void Map2_Mode::finish() {

}

void Map2_Mode::updatePhysics(float deltaTime) {
	/*dynamicsWorld->stepSimulation(deltaTime);

	UpdateRigidBodyTransforms(karts);
	UpdateRigidBodyTransforms(road2_barricate);

	checkCollisionKart();*/
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