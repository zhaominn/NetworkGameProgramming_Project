#include "Pch.h"
#include "RoomMode.h"
#include "NetGlobal.h"

RoomMode::RoomMode()
{
}

RoomMode::~RoomMode()
{
}

void RoomMode::init()
{
}

void RoomMode::mouseClick(int button, int state, int x, int y)
{
}

void RoomMode::keyboard(unsigned char key, int x, int y)
{
	if (key == 'r'|| key == 'R') {
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

}

void RoomMode::draw_bb()
{
}

void RoomMode::finish()
{
}
