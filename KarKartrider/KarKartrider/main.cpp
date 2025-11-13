#include "Pch.h"
#include "DefaultModel.h"
#include "KeyBoard.h"
#include "Light.h"
#include "Debug.h"
#include "LogoMode.h"
#include "shaderMaker.h"
#include "root.h"
#include "NetworkMgr.h"
#include "protocol.h"
#include "LoginMode.h"
#include "NetGlobal.h"
#include "RoomMode.h"



using namespace std;

void InitBuffer();
void initPhysics();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

DWORD WINAPI RecvThread(LPVOID lpParam)
{
	while (true)
	{
		std::cout << "recv" << std::endl;
		int size = 0;
		char buf[BUF_SIZE];

		int retval = recv(networkmgr.GetSocket(), (char*)&size, sizeof(int), 0);
		if (retval <= 0)
			cout << "error" << endl;

		retval = recv(networkmgr.GetSocket(), buf, size, 0);
		if (retval <= 0)
			cout << "error" << endl;

		networkmgr.ProcessPacket(buf);
	}
	return 0;
}

int main(int argc, char** argv) {
	InitializeCriticalSection(&CS);
	if (!networkmgr.Init()) {
		std::cout << "Init Socket error " << std::endl;
		return 0;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowPosition(location_x, location_y);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("KarKartrider");
	//glutFullScreen(); 

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();
	make_shaderProgram_ui();

	initPhysics();

	/*LogoMode* logoMode = new LogoMode();
	MM.SetMode(logoMode);*/

	LoginMode* loginMode = new LoginMode();
	MM.SetMode(loginMode);

	/*debug_model(models.back());
	debug_materials(models.back()->materials);*/


	InitBuffer();


	//drawScene(); 

	glutDisplayFunc(drawScene);
	//glutIdleFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(specialKey);
	glutSpecialUpFunc(specialKeyUp);
	glutMouseFunc(mouseClick);

	HANDLE hThread = CreateThread(NULL, 0, RecvThread, 0, 0, 0);
	if (hThread == NULL) {
		closesocket(networkmgr.GetSocket());
	}

	glutMainLoop();

	DeleteCriticalSection(&CS);

	return 0;
}

GLvoid drawScene() {

	// RoomMode인지 체크
	if (dynamic_cast<RoomMode*>(MM.GetMode()) != nullptr)
	{
		// RoomMode는 배경을 하얀색으로 클리어
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		MM.draw_model();   // <<--- RoomMode의 text 렌더링

		glutSwapBuffers();
		return;  // 3D 렌더링 차단!
	}

	// ====== 여기부터는 3D 게임 화면 ======
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MM.draw_model();  // 3D 모델 + 카메라 + 맵 + 플레이어 등
	MM.draw_bb();

	glutSwapBuffers();
}

void InitBuffer() {
	//-----------------------------------------------------------------------------------------------------------
	for (const auto& model : karts) {
		model->initBuffer();
	}

	for (const auto& model : road1) {
		model->initBuffer();
	}

	for (const auto& model : road2) {
		model->initBuffer();
	}

	for (const auto& model : selectMaps) {
		model->initBuffer();
	}

	for (const auto& model : countDown) {
		model->initBuffer();
	}

	for (const auto& model : booster_uis) {
		model->initBuffer();
	}
	for (const auto& model : dashBoards) {
		model->initBuffer();
	}
}