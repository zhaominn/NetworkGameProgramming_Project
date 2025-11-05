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

using namespace std;

void InitBuffer();
void initPhysics();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

struct PlayerKart {
	int id;
	char name[NAME_SIZE];
	float x, y, z;
	int booster_cnt;
	float speed;
};

NetworkMgr networkmgr;
std::array<PlayerKart, MAX_USER> g_players;
CRITICAL_SECTION CS;

DWORD WINAPI RecvThread(LPVOID lpParam)
{
	while (true)
	{
		int len = 0;
		char buf[BUF_SIZE];

		recv(networkmgr.GetSocket(), (char*)&len, sizeof(int), MSG_WAITALL);
		recv(networkmgr.GetSocket(), buf, len, MSG_WAITALL);

		networkmgr.ProcessPacket(buf, len);
	}
	return 0;
}

int main(int argc, char** argv) {
	InitializeCriticalSection(&CS);
	if (!networkmgr.Init()) {
		std::cout << "Init Socket error " << std::endl;
		return 0;
	}

	char name[NAME_SIZE];
	cout << "이름을 입력하세요 : ";
	cin >> name;

	C2S_Login_Packet* packet = new C2S_Login_Packet;
	packet->size = sizeof(C2S_Login_Packet);
	packet->type = C2S_LOGIN;
	strncpy(packet->name, name, NAME_SIZE);
	networkmgr.SendPacket(reinterpret_cast<char*>(packet), packet->size);
	delete packet;

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

	LogoMode* logoMode = new LogoMode();
	MM.SetMode(logoMode);

	//LoginMode* loginMode = new LoginMode(); 
	//MM.SetMode(loginMode);

	/*debug_model(models.back());
	debug_materials(models.back()->materials);*/


	InitBuffer();


	//drawScene(); 

	glutDisplayFunc(drawScene);
	glutIdleFunc(drawScene);
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