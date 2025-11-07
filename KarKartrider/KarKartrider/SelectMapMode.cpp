#include "Pch.h"
#include "SelectMapMode.h"
#include "Map1_Mode.h"
#include "Map2_Mode.h"
#include "NetGlobal.h"

bool isAnimating = false;
float animationSpeed = 0.05f;
glm::vec3 targetCameraPos = glm::vec3(0.0, 0.0, 5.0);
glm::vec3 cameraPosMapMode = glm::vec3(0.0, 0.0, 5.0);

void SelectMap_timer(int value) {
	if (isAnimating) {
		cameraPosMapMode = glm::mix(cameraPosMapMode, targetCameraPos, animationSpeed);
		if (glm::distance(cameraPosMapMode, targetCameraPos) < 0.01f) {
			cameraPosMapMode = targetCameraPos;
			isAnimating = false;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, SelectMap_timer, 0);
}

void SelectMapMode::goSelectMode() {
	SelectMapMode* selectMode = new SelectMapMode();
	MM.SetMode(selectMode);
}

void SelectMapMode::init() {
	soundThread = std::thread(&SelectMapMode::runSound, this);
	targetCameraPos = cameraPosMapMode;
	glutTimerFunc(0, SelectMap_timer, 0);
}

void SelectMapMode::mouseClick(int button, int state, int x, int y) {}

void SelectMapMode::keyboard(unsigned char key, int x, int y) {
	std::thread clickSoundThread(&SelectMapMode::clickSound, this);
	switch (key) {
	case '\r': {
		isSoundRunning = false;
		MAP_TYPE myMap;
		if (map_num == 1) {
			Map1_Mode* map1Mode = new Map1_Mode();
			map1Mode->goSelectMode = [this]() { goSelectMode(); };
			MM.SetMode(map1Mode);
			myMap = STRAIGHT;
		}
		else if (map_num == 2) {
			Map2_Mode* map2Mode = new Map2_Mode();
			map2Mode->goSelectMode = [this]() { goSelectMode(); };
			MM.SetMode(map2Mode);
			myMap = RECTANGLE;
		}

		networkmgr.SendSelectMapPacket(myMap);

		break;
	}
	default: break;
	}
	clickSoundThread.detach();
}


void SelectMapMode::specialKey(int key, int x, int y) {
	std::thread clickSoundThread(&SelectMapMode::clickSound, this);
	if (key == GLUT_KEY_LEFT) {
		map_num--;
		if (map_num < 1) map_num = 1;
		updateTargetCameraPos();
	}
	else if (key == GLUT_KEY_RIGHT) {
		map_num++;
		if (map_num > 2) map_num = 2;
		updateTargetCameraPos();
	}
	clickSoundThread.detach();
}

void SelectMapMode::specialKeyUp(int key, int x, int y)
{
}




void SelectMapMode::updateTargetCameraPos()
{
	if (map_num == 1) {
		targetCameraPos = map1CamerPos;
	}
	else if (map_num == 2) {
		targetCameraPos = map2CamerPos;
	}

	isAnimating = true; 
}

void SelectMapMode::draw_model()
{
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);


	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error in glUseProgram: " << error << std::endl;
	}

	
	glm::vec3 fixedLookDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 lookAtTarget = cameraPosMapMode + fixedLookDirection;


	viewMapMode = glm::lookAt(
		cameraPosMapMode,      
		lookAtTarget,           
		cameraUpMapMode        
	);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &viewMapMode[0][0]);

	projectionMapMode = glm::mat4(1.0f);
	projectionMapMode = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 300.0f);
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projectionMapMode[0][0]);

	glEnable(GL_DEPTH_TEST);

	for (const auto& maps : selectMaps) { 
		maps->draw(shaderProgramID, isKeyPressed_s);
	}

	glDisable(GL_DEPTH_TEST);
}

void SelectMapMode::draw_bb()
{
}

void SelectMapMode::finish()
{
	if (soundThread.joinable()) {
		isSoundRunning = false; 
		soundThread.join();     
	}
}

void SelectMapMode::runSound() {

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

void SelectMapMode::clickSound() {

	isclickRunning = true;
	play_sound2D("click.wav", "./asset/select_mode/", false, &isclickRunning);
	isclickRunning = false;
}