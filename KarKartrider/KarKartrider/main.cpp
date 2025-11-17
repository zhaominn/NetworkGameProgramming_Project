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

using namespace std;

void InitBuffer();
void initPhysics();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

DWORD WINAPI RecvThread(LPVOID lpParam)
{
	while (networkmgr.IsRunning())
	{
		int size = 0;
		char buf[BUF_SIZE];

		int retval = recv(networkmgr.GetSocket(), (char*)&size, sizeof(int), 0);

		if (retval <= 0) {
			cout << "서버 연결 끊김 (헤더 수신 실패)" << endl;
			networkmgr.StopRunning();
			break;
		}

		if (size <= 0 || size > BUF_SIZE) {
			cout << "비정상적인 패킷 크기 수신: " << size << endl;
			networkmgr.StopRunning();
			break;
		}

		int received_total = 0;
		while (received_total < size)
		{
			retval = recv(networkmgr.GetSocket(),
				buf + received_total,
				size - received_total,
				0);

			if (retval <= 0) {
				cout << "서버 연결 끊김 (본문 수신 실패)" << endl;
				networkmgr.StopRunning();
				break;
			}

			received_total += retval;
		}

		if (!networkmgr.IsRunning()) {
			break;
		}

		networkmgr.ProcessPacket(buf);
	}

	cout << "수신 스레드 종료." << endl;
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

	networkmgr.StartRunning();

	HANDLE hThread = CreateThread(NULL, 0, RecvThread, 0, 0, 0);
	if (hThread == NULL) {
		//closesocket(networkmgr.GetSocket()); // StopRunning이 대신 처리함
		networkmgr.StopRunning(); // 실패 시 플래그 원복 및 소켓 정리
	}

	glutMainLoop();

	networkmgr.StopRunning();
	if (hThread != NULL) {
		WaitForSingleObject(hThread, INFINITE); // 스레드 종료 대기
		CloseHandle(hThread);
	}

	DeleteCriticalSection(&CS);

	return 0;
}

GLvoid drawScene() {
	if (!networkmgr.IsRunning()) {
		cout << "네트워크 연결 끊김" << endl;
		glutLeaveMainLoop();
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MM.draw_model();
	MM.draw_bb();

	glutSwapBuffers();
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error: " << err << endl;
	}
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