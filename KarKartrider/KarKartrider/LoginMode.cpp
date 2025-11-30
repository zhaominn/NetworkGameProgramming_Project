#include "Pch.h"
#include "LoginMode.h"
#include "Light.h"
#include "RoomMode.h"
#include "NetGlobal.h"
//-----------------------------
#include "LoadProgress.h"
#include "KartModel.h"
#include "RoadModel.h"
#include "BulletPhysics.h"
#include "MapModel.h"
#include "BarricateMap1Model.h"
#include "Ui_v2.h"
//
#include "include/stb_image.h"

LoginMode::LoginMode()
{
	inputText = "";
}

LoginMode::~LoginMode()
{
	delete this;
}

void LoginMode::init()
{
	loadModelWithProgress<KartModel>("bazzi_face2.obj", "obj/character/", "character_face", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
	loadModelWithProgress<KartModel>("bazzi_body.obj", "obj/character/", "character_body", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
	loadModelWithProgress<KartModel>("booster.obj", "obj/car/booster/", "booster", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
	loadModelWithProgress<KartModel>("kronos.obj", "obj/car/kronos/", "car", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), karts, true, true);

	loadModelWithProgress<Ui_v2>("booster_ui.obj", "obj/ui/", "booster_ui1", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.90, 0.75, 0.0)), glm::vec3(0.1, 0.1, 0.1)), booster_uis, false, true);
	loadModelWithProgress<Ui_v2>("booster_ui.obj", "obj/ui/", "booster_ui2", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.78, 0.75, 0.0)), glm::vec3(0.1, 0.1, 0.1)), booster_uis, false, true);

	loadModelWithProgress<Ui_v2>("dashboard.obj", "obj/ui/", "dashboard", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -0.92, 0.0)), glm::vec3(0.7, 0.6, 0.5)), dashBoards, false, true);
	loadModelWithProgress<KartModel>("pause.obj", "obj/ui/", "pause", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), pause, true, true);

	loadModelWithProgress<RoadModel>("road_all_01.obj", "obj/road/", "road_all_1", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(15.0, 15.0, 15.0)), road1, false, true);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(16.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 700.0)), road1_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-16.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 700.0)), road1_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 304.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, true);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -304.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, true);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -212.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, false);

	loadModelWithProgress<RoadModel>("road_all_02.obj", "obj/road/", "road_all_2", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(20.0, 20.0, 20.0)), road2, false, true);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(184.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 380.0)), road2_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-184.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 380.0)), road2_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 184.7)), glm::vec3(380.0, 15.0, 15.0)), road2_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -184.7)), glm::vec3(380.0, 15.0, 15.0)), road2_barricate, true, false);
	//loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)), glm::vec3(331.0, 331.0, 331.0)), road2_barricate, true, false); //��� ��ֹ�
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(145.3, 0.0, 0.0)), glm::vec3(15.0, 15.0, 290.0)), road2_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-145.3, 0.0, 0.0)), glm::vec3(15.0, 15.0, 290.0)), road2_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 145.3)), glm::vec3(290.0, 15.0, 15.0)), road2_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -145.3)), glm::vec3(290.0, 15.0, 15.0)), road2_barricate, true, false);

	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(165.0, 0.0, 18.0)), glm::vec3(40.0, 1.0, 1.0)), road2_barricate, true, false);
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish_ch", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(165.0, 0.0, 50.0)), glm::vec3(40.0, 1.0, 1.0)), road2_barricate, true, false);

	glm::mat4 m1 = glm::mat4(1.0f);
	m1 = glm::translate(m1, glm::vec3(174.7, 0.0, -175.3));
	m1 = glm::rotate(m1, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
	m1 = glm::scale(m1, glm::vec3(0.0, 0.0, 0.0));
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

	m1 = glm::mat4(1.0f);
	m1 = glm::translate(m1, glm::vec3(174.7, 0.0, 175.3));
	m1 = glm::rotate(m1, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
	m1 = glm::scale(m1, glm::vec3(0.0, 0.0, 0.0));
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

	m1 = glm::mat4(1.0f);
	m1 = glm::translate(m1, glm::vec3(-174.7, 0.0, 175.3));
	m1 = glm::rotate(m1, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
	m1 = glm::scale(m1, glm::vec3(0.0, 0.0, 0.0));
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

	m1 = glm::mat4(1.0f);
	m1 = glm::translate(m1, glm::vec3(-174.7, 0.0, -175.3));
	m1 = glm::rotate(m1, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
	m1 = glm::scale(m1, glm::vec3(0.0, 0.0, 0.0));
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

	glm::mat4 m2 = glm::mat4(1.0f);
	m2 = glm::translate(m2, glm::vec3(146.8, 0.0, -146.8));
	m2 = glm::rotate(m2, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
	m2 = glm::scale(m2, glm::vec3(0.0, 0.0, 0.0));
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

	m2 = glm::mat4(1.0f);
	m2 = glm::translate(m2, glm::vec3(146.8, 0.0, 146.8));
	m2 = glm::rotate(m2, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
	m2 = glm::scale(m2, glm::vec3(0.0, 0.0, 0.0));
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

	m2 = glm::mat4(1.0f);
	m2 = glm::translate(m2, glm::vec3(-146.8, 0.0, 146.8));
	m2 = glm::rotate(m2, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
	m2 = glm::scale(m2, glm::vec3(0.0, 0.0, 0.0));
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

	m2 = glm::mat4(1.0f);
	m2 = glm::translate(m2, glm::vec3(-146.8, 0.0, -146.8));
	m2 = glm::rotate(m2, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
	m2 = glm::scale(m2, glm::vec3(0.0, 0.0, 0.0));
	loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

	loadModelWithProgress<KartModel>("3.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
	loadModelWithProgress<KartModel>("2.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
	loadModelWithProgress<KartModel>("1.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
	loadModelWithProgress<KartModel>("start.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);

	//loadModelWithProgress<KartModel>("pause.obj", "obj/ui/", "pause", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), pause, true, true);

	loadModelWithProgress<MapModel>("village_road.obj", "asset/select_mode/", "village_road", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), selectMaps, false, true);

	glm::mat4 map2_matrix = glm::mat4(1.0f);
	map2_matrix = glm::translate(map2_matrix, glm::vec3(2.5, 0.0, 0.0));
	map2_matrix = glm::scale(map2_matrix, glm::vec3(1.0, 1.0, 1.0));
	loadModelWithProgress<MapModel>("villiage_unha.obj", "asset/select_mode/", "villiage_unha", "box", map2_matrix, selectMaps, false, true);

	glm::mat4 arrow_matrix = glm::mat4(1.0f);
	arrow_matrix = glm::translate(arrow_matrix, glm::vec3(-0.2, -1.0, 1.0));
	arrow_matrix = glm::scale(arrow_matrix, glm::vec3(0.5, 0.5, 0.5));
	arrow_matrix = glm::rotate(arrow_matrix, glm::radians(-80.0f), glm::vec3(1.0, 0.0, 0.0));
	loadModelWithProgress<MapModel>("arrow_key.obj", "asset/select_mode/", "arrow_key", "box", arrow_matrix, selectMaps, false, true);

	glm::mat4 enter_matrix = glm::mat4(1.0f);
	enter_matrix = glm::translate(enter_matrix, glm::vec3(0.6, -1.0, 1.0));
	enter_matrix = glm::scale(enter_matrix, glm::vec3(0.4, 0.3, 0.4));
	enter_matrix = glm::rotate(enter_matrix, glm::radians(-80.0f), glm::vec3(1.0, 0.0, 0.0));
	loadModelWithProgress<MapModel>("enter_key.obj", "asset/select_mode/", "enter_key", "box", enter_matrix, selectMaps, false, true);

	//-----------------------------------
	login_tex = loadTexture("asset/login/login_img.png");
	before_tex = loadTexture("asset/login/game_start_after.png");
	after_tex = loadTexture("asset/login/game_start_before.png");
	nickname_tex = loadTexture("asset/login/enter_name.png");

	isRunning = false;
}

GLuint LoginMode::loadTexture(const char* filename) {
	int width, height, channel;
	unsigned char* data = stbi_load(filename, &width, &height, &channel, 4);

	if (!data) {
		printf("ERROR: Fail to load image %s\n", filename);
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	return texture;
}

void LoginMode::draw_login()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, login_tex);
	glColor3f(1.0f, 1.0f, 1.0f); // 색상 초기화

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void LoginMode::mouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (startButton == false && isButtonHovered)
		{
			printf("Start 버튼 클릭 성공! 입력창으로 이동합니다.\n");

			startButton = true;
			glutPostRedisplay();
		}
	}
}

void LoginMode::passiveMotion(int x, int y)
{
	if(!startButton){
		float glX = ((float)x / (float)glutGet(GLUT_WINDOW_WIDTH)) * 2.0f - 1.0f;
		float glY = -(((float)y / (float)glutGet(GLUT_WINDOW_HEIGHT)) * 2.0f - 1.0f);

		float btnWidth = 0.5f;
		float btnHeight = btnWidth * (74.0f / 309.0f) * 1.2f;
		float btnX = 0.0f;
		float btnY = -0.7f;

		float left = btnX - (btnWidth / 2.0f);
		float right = btnX + (btnWidth / 2.0f);
		float bottom = btnY - (btnHeight / 2.0f);
		float top = btnY + (btnHeight / 2.0f);

		if (glX >= left && glX <= right && glY >= bottom && glY <= top) {
			isButtonHovered = true;
		}
		else {
			isButtonHovered = false;
		}

		glutPostRedisplay();
	}
}

void LoginMode::keyboard(unsigned char key, int x, int y)
{
	// Enter 키 처리
	if (key == '\r') {
		if (startButton) {
			std::cout << "입력 완료: " << inputText << std::endl;

			networkmgr.SendLoginPacket(inputText);

			//SelectMapMode* selectMapMode = new SelectMapMode();
			RoomMode* roomMapMode = new RoomMode();
			//selectMapMode->goSelectMode = [this]() { goSelectMode(); };
			MM.SetMode(roomMapMode);
		}

		return;
	}

	// Backspace 처리
	if (key == 8 && !inputText.empty()) {
		inputText.pop_back();
	}
	else {
		// 일반 문자 입력
		if (key >= 32 && key <= 126) {  // ASCII 출력 가능한 문자만 허용
			inputText += key;
		}
	}

	glutPostRedisplay(); // 다시 그리기 요청
}

void LoginMode::specialKey(int key, int x, int y)
{
}

void LoginMode::specialKeyUp(int key, int x, int y)
{
}

void LoginMode::draw_model()
{
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	draw_login();

	if (startButton == false)
	{
		glEnable(GL_TEXTURE_2D);

		if (isButtonHovered) {
			glBindTexture(GL_TEXTURE_2D, before_tex);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, after_tex);
		}

		float btnWidth = 0.5f;
		float btnHeight = btnWidth * (74.0f / 309.0f) * 1.2f; 
		float cx = 0.0f;  
		float cy = -0.7f; 

		float hw = btnWidth / 2.0f;
		float hh = btnHeight / 2.0f;

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(cx - hw, cy - hh, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(cx - hw, cy + hh, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(cx + hw, cy + hh, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(cx + hw, cy - hh, 0.0f);
		glEnd();

		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, nickname_tex);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		float labelWidth = 0.6f;
		float labelHeight = labelWidth * (142.0f / 265.0f) * 1.2f;

		float cx = 0.0f;
		float cy = - 0.1f;

		float hw = labelWidth / 2.0f;
		float hh = labelHeight / 2.0f;

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(cx - hw, cy - hh, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(cx - hw, cy + hh, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(cx + hw, cy + hh, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(cx + hw, cy - hh, 0.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glRasterPos2f(-0.25f, -0.2f);
		for (char c : inputText)glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

		glRasterPos2f(-0.25f + 0.002f, -0.2f);
		for (char c : inputText) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

		glRasterPos2f(-0.25f, -0.2f - 0.002f);
		for (char c : inputText) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

void LoginMode::draw_bb()
{
}

void LoginMode::finish()
{
}