#include "Pch.h"
#include "RoomMode.h"
#include "NetGlobal.h"

#include "include/stb_image.h"

RoomMode::RoomMode()
{
	player_tex = loadTexture("asset/room/player_image.png");
	ready_tex = loadTexture("asset/room/ready.png");
	ready_hovered_tex = loadTexture("asset/room/ready_hovered.png");
	road1_tex = loadTexture("asset/room/road_1.png");
	road1_hovered_tex = loadTexture("asset/room/road_1_hovered.png");
	road2_tex = loadTexture("asset/room/road_2.png");
	road3_hovered_tex = loadTexture("asset/room/road_2_hovered.png");
	room_tex = loadTexture("asset/room/room.png");
}

RoomMode::~RoomMode()
{
}

void RoomMode::init()
{
}

GLuint RoomMode::loadTexture(const char* filename) {
	int width, height, channel;
	unsigned char* data = stbi_load(filename, &width, &height, &channel, 4);

	if (!data) {
		printf("ERROR: Fail to load image %s\n", filename);
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	return texture;
}

void RoomMode::mouseClick(int button, int state, int x, int y)
{
}

void RoomMode::keyboard(unsigned char key, int x, int y)
{
	if (key == 'r' || key == 'R') {
		ready_status = !ready_status;
		networkmgr.SendChangeReadyPacket(ready_status);
	}
}

void RoomMode::specialKey(int key, int x, int y)
{

}

void RoomMode::specialKeyUp(int key, int x, int y)
{
}

void RoomMode::draw_model()
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

	float px = 2.0f / 980.0f;
	float py = 2.0f / 770.0f;

	// -----------------------------------------------------------
	// 배경 (Room)
	// -----------------------------------------------------------
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, room_tex);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	// -----------------------------------------------------------
	// 플레이어 캐릭터 3명 (300 * 458) - 상단 배치
	// -----------------------------------------------------------
	glBindTexture(GL_TEXTURE_2D, player_tex);

	float pW = 300.0f * px;
	float pH = 458.0f * py;
	float pHW = pW / 2.0f;
	float pHH = pH / 2.0f;

	float pY = 0.35f;

	// 좌측
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.64f - pHW, pY - pHH, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.64f - pHW, pY + pHH, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.64f + pHW, pY + pHH, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.64f + pHW, pY - pHH, 0.0f);
	glEnd();

	// 중앙
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f - pHW, pY - pHH, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f - pHW, pY + pHH, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f + pHW, pY + pHH, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f + pHW, pY - pHH, 0.0f);
	glEnd();

	// 우측
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.64f - pHW, pY - pHH, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.64f - pHW, pY + pHH, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.64f + pHW, pY + pHH, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.64f + pHW, pY - pHH, 0.0f);
	glEnd();

	// -----------------------------------------------------------
	// Road 버튼들 (299 * 250) - 좌측 하단 배치
	// -----------------------------------------------------------
	float rW = 300.0f * px;
	float rH = 250.0f * py * 0.9;
	float rHW = rW / 2.0f;
	float rHH = rH / 2.0f;

	float rY = -0.65f;

	// [Road 1]
	glBindTexture(GL_TEXTURE_2D, road1_tex);
	float r1_X = -0.65f;
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(r1_X - rHW, rY - rHH, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(r1_X - rHW, rY + rHH, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(r1_X + rHW, rY + rHH, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(r1_X + rHW, rY - rHH, 0.0f);
	glEnd();

	// [Road 2]
	glBindTexture(GL_TEXTURE_2D, road2_tex);
	float r2_X = 0.f;
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(r2_X - rHW, rY - rHH, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(r2_X - rHW, rY + rHH, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(r2_X + rHW, rY + rHH, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(r2_X + rHW, rY - rHH, 0.0f);
	glEnd();

	// -----------------------------------------------------------
	// Ready 버튼 (299 * 100) - 우측 하단 배치
	// -----------------------------------------------------------
	if (ready_status) glBindTexture(GL_TEXTURE_2D, ready_hovered_tex);
	else glBindTexture(GL_TEXTURE_2D, ready_tex);

	float readyW = 301.0f * px;
	float readyH = 100.0f * py * 0.9;
	float readyHW = readyW / 2.0f;
	float readyHH = readyH / 2.0f;

	// Y 위치: Road 버튼들보다 이미지가 얇으니 위치를 조금 조정 (-0.75)
	float readyY = -0.75f;
	float readyX = 0.65f; // 우측 하단

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(readyX - readyHW, readyY - readyHH, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(readyX - readyHW, readyY + readyHH, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(readyX + readyHW, readyY + readyHH, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(readyX + readyHW, readyY - readyHH, 0.0f);
	glEnd();

	// -----------------------------------------------------------
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void RoomMode::draw_bb()
{
}

void RoomMode::finish()
{
}
