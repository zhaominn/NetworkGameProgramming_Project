#include "Pch.h"
#include "RoomMode.h"
#include "NetGlobal.h"

#include "include/stb_image.h"

RoomMode::RoomMode()
{
	player_tex = loadTexture("asset/room/player_img.png");
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, room_tex);
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();

		glDisable(GL_TEXTURE_2D);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
	
}

void RoomMode::draw_bb()
{
}

void RoomMode::finish()
{
}
