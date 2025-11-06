#include "Pch.h"
#include "LoginMode.h"
#include "Light.h"
#include "SelectMapMode.h"
#include "NetGlobal.h"
//-----------------------------
#include "LoadProgress.h"
#include "KartModel.h"
#include "RoadModel.h"
#include "BulletPhysics.h"
#include "MapModel.h"
#include "BarricateMap1Model.h"
#include "Ui_v2.h"

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
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(184.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 400.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-184.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 400.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 184.7)), glm::vec3(400.0, 15.0, 15.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -184.7)), glm::vec3(400.0, 15.0, 15.0)), road2_barricate, true, false);
    //loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)), glm::vec3(331.0, 331.0, 331.0)), road2_barricate, true, false); //��� ��ֹ�
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(145.3, 0.0, 0.0)), glm::vec3(15.0, 15.0, 315.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-145.3, 0.0, 0.0)), glm::vec3(15.0, 15.0, 315.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 145.3)), glm::vec3(315.0, 15.0, 15.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -145.3)), glm::vec3(315.0, 15.0, 15.0)), road2_barricate, true, false);

    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(165.0, 0.0, 18.0)), glm::vec3(40.0, 1.0, 1.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish_ch", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(165.0, 0.0, 50.0)), glm::vec3(40.0, 1.0, 1.0)), road2_barricate, true, false);

    glm::mat4 m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(174.7, 0.0, -175.3));
    m1 = glm::rotate(m1, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

    m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(174.7, 0.0, 175.3));
    m1 = glm::rotate(m1, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

    m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(-174.7, 0.0, 175.3));
    m1 = glm::rotate(m1, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

    m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(-174.7, 0.0, -175.3));
    m1 = glm::rotate(m1, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

    glm::mat4 m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(146.8, 0.0, -146.8));
    m2 = glm::rotate(m2, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    m2 = glm::scale(m2, glm::vec3(5.0, 15.0, 10.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

    m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(146.8, 0.0, 146.8));
    m2 = glm::rotate(m2, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    m2 = glm::scale(m2, glm::vec3(5.0, 15.0, 10.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

    m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(-146.8, 0.0, 146.8));
    m2 = glm::rotate(m2, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    m2 = glm::scale(m2, glm::vec3(5.0, 15.0, 10.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

    m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(-146.8, 0.0, -146.8));
    m2 = glm::rotate(m2, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    m2 = glm::scale(m2, glm::vec3(5.0, 15.0, 10.0));
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



    isRunning = false;
}

void LoginMode::mouseClick(int button, int state, int x, int y)
{
}

void LoginMode::keyboard(unsigned char key, int x, int y)
{
	// Enter 키 처리
	if (key == '\r') {
		std::cout << "입력 완료: " << inputText << std::endl;

		C2S_Login_Packet* packet = new C2S_Login_Packet;
		packet->size = sizeof(C2S_Login_Packet);
		packet->type = C2S_LOGIN;
        strncpy(packet->name, inputText.c_str(), NAME_SIZE - 1);
        packet->name[NAME_SIZE - 1] = '\0';
        networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Login_Packet));
		delete packet;

		SelectMapMode* selectMapMode = new SelectMapMode();
		//selectMapMode->goSelectMode = [this]() { goSelectMode(); };
		MM.SetMode(selectMapMode);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(-0.6f, 0.2f);
	for (char c : std::string("Enter text:"))
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

	glColor3f(0.2f, 0.2f, 0.8f);
	glRasterPos2f(-0.6f, -0.3f);
	for (char c : inputText)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

}

void LoginMode::draw_bb()
{
}

void LoginMode::finish()
{
}