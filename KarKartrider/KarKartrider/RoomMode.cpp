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
	road2_hovered_tex = loadTexture("asset/room/road_2_hovered.png");
	room_tex = loadTexture("asset/room/room.png");
	player_ready_tex = loadTexture("asset/room/ready_player.png");
}

RoomMode::~RoomMode()
{
}

void RoomMode::init()
{
	std::cout << "Room Mode" << std::endl;
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
	if (state == GLUT_UP)
	{
		if(!g_players[g_myid].isReady)
		{
			if (isRoad1Hovered)
			{
				if (g_players[g_myid].select_map != STRAIGHT)
					networkmgr.SendEnterRoomPacket(STRAIGHT);
			}
			else if (isRoad2Hovered)
			{
				if (g_players[g_myid].select_map != RECTANGLE)
					networkmgr.SendEnterRoomPacket(RECTANGLE);
			}
		}

		if (isReadyHovered)
		{
			networkmgr.SendChangeReadyPacket();
		}
	}
}

void RoomMode::passiveMotion(int x, int y)
{
	float glX = ((float)x / 980.0f) * 2.0f - 1.0f;
	float glY = -(((float)y / 770.0f) * 2.0f - 1.0f);
	float px = 2.0f / 980.0f;
	float py = 2.0f / 770.0f;

	float rW = 300.0f * px;
	float rH = 250.0f * py * 0.9f;
	float r1_X = -0.65f;
	float rY = -0.65f;

	if (glX >= r1_X - (rW / 2) && glX <= r1_X + (rW / 2) &&
		glY >= rY - (rH / 2) && glY <= rY + (rH / 2)) {
		isRoad1Hovered = true;
	}
	else {
		isRoad1Hovered = false;
	}

	float r2_X = 0.0f;

	if (glX >= r2_X - (rW / 2) && glX <= r2_X + (rW / 2) &&
		glY >= rY - (rH / 2) && glY <= rY + (rH / 2)) {
		isRoad2Hovered = true;
	}
	else {
		isRoad2Hovered = false;
	}

	float readyW = 301.0f * px;
	float readyH = 100.0f * py * 0.9f;
	float readyX = 0.65f;
	float readyY = -0.48f;

	if (glX >= readyX - (readyW / 2) && glX <= readyX + (readyW / 2) &&
		glY >= readyY - (readyH / 2) && glY <= readyY + (readyH / 2)) {
		isReadyHovered = true;
	}
	else {
		isReadyHovered = false;
	}

	glutPostRedisplay();
}

void RoomMode::keyboard(unsigned char key, int x, int y)
{
	if (key == 'r' || key == 'R') {
		// ready_status = !ready_status;
		// networkmgr.SendChangeReadyPacket(ready_status);
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
	RefreshSlotData(); // 데이터 최신화

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
	// 플레이어 렌더링 Loop
	// -----------------------------------------------------------
	float pW = 300.0f * px;
	float pH = 458.0f * py;
	float pHW = pW / 2.0f;
	float pHH = pH / 2.0f;
	float pY = 0.35f;
	float slotXPos[3] = { -0.64f, 0.0f, 0.64f };

	for (int i = 0; i < 3; ++i)
	{
		int pid = m_slots[i];

		if (pid < 0 || pid >= MAX_USER) continue;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, player_tex);
		glColor3f(1.0f, 1.0f, 1.0f);

		float cX = slotXPos[i];

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(cX - pHW, pY - pHH, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(cX - pHW, pY + pHH, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(cX + pHW, pY + pHH, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(cX + pHW, pY - pHH, 0.0f);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glColor3f(0.0f, 0.0f, 0.0f);

		int nameLen = strlen(g_players[pid].m_name);
		if (nameLen > 0)
		{
			float textX = cX - (nameLen * 0.015f);
			float textY = pY + pHH - 0.1f;

			glRasterPos3f(textX, textY, 1.0f);

			for (int k = 0; k < nameLen; k++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, g_players[pid].m_name[k]);

			glRasterPos3f(textX + 0.002f, textY, 1.0f);
			for (int k = 0; k < nameLen; k++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, g_players[pid].m_name[k]);

			glRasterPos3f(textX, textY + 0.002f, 1.0f);
			for (int k = 0; k < nameLen; k++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, g_players[pid].m_name[k]);
		}

		if (g_players[pid].isReady)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, player_ready_tex);
			glColor3f(1.0f, 1.0f, 1.0f);

			float rW = 320.0f * px;
			float rH = 143.0f * py;

			float rHW = rW / 2.0f;
			float rHH = rH / 2.0f;

			float rX = cX;
			float rY = (pY - pHH) + 0.2f;

			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(rX - rHW, rY - rHH, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(rX - rHW, rY + rHH, 1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(rX + rHW, rY + rHH, 1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(rX + rHW, rY - rHH, 1.0f);
			glEnd();

			glDisable(GL_TEXTURE_2D);
		}
	}

	// -----------------------------------------------------------
	// Road & Ready 버튼
	// -----------------------------------------------------------
	glEnable(GL_TEXTURE_2D);
	float rW = 300.0f * px;
	float rH = 250.0f * py * 0.9f;
	float rHW = rW / 2.0f;
	float rHH = rH / 2.0f;
	float rY = -0.65f;

	// [Road 1]
	GLuint r1_tex = (isRoad1Hovered || g_players[g_myid].select_map == STRAIGHT) ? road1_hovered_tex : road1_tex;
	glBindTexture(GL_TEXTURE_2D, r1_tex);
	glColor3f(1.0f, 1.0f, 1.0f);

	float r1_X = -0.65f;
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(r1_X - rHW, rY - rHH, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(r1_X - rHW, rY + rHH, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(r1_X + rHW, rY + rHH, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(r1_X + rHW, rY - rHH, 0.0f);
	glEnd();

	// [Road 2]
	GLuint r2_tex = (isRoad2Hovered || g_players[g_myid].select_map == RECTANGLE) ? road2_hovered_tex : road2_tex;
	glBindTexture(GL_TEXTURE_2D, r2_tex);

	float r2_X = 0.0f;
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(r2_X - rHW, rY - rHH, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(r2_X - rHW, rY + rHH, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(r2_X + rHW, rY + rHH, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(r2_X + rHW, rY - rHH, 0.0f);
	glEnd();

	// [Ready 버튼]
	GLuint ready_current_tex = (g_players[g_myid].isReady || isReadyHovered) ? ready_hovered_tex : ready_tex;
	glBindTexture(GL_TEXTURE_2D, ready_current_tex);

	float readyW = 301.0f * px;
	float readyH = 100.0f * py * 0.9f;
	float readyHW = readyW / 2.0f;
	float readyHH = readyH / 2.0f;
	float readyY = -0.48f;
	float readyX = 0.65f;

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
	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void RoomMode::draw_bb()
{
}

void RoomMode::finish()
{
}

void RoomMode::RefreshSlotData()
{
	for (int i = 0; i < 3; ++i) {
		m_slots[i] = -1;
	}

	int myRoomMapType = g_players[g_myid].select_map;

	int masterID = -1;
	std::vector<int> normalPlayers;

	for (int i = 0; i < MAX_USER; ++i)
	{
		const auto& p = g_players[i];

		if (!p.isOnline) continue;
		if (p.select_map != myRoomMapType) continue;

		
			normalPlayers.push_back(p.m_id);
	}

	if (masterID != -1)
	{
		m_slots[0] = masterID;

		for (size_t i = 0; i < normalPlayers.size(); ++i) {
			if (i + 1 < 3) m_slots[i + 1] = normalPlayers[i];
		}
	}
	else
	{
		for (size_t i = 0; i < normalPlayers.size(); ++i) {
			if (i < 3) m_slots[i] = normalPlayers[i];
		}
	}
}