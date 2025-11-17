#include "Room.h"

void Room::reset()
{
	mapType = STRAIGHT;
	roomManagerID = -1;
	for (int i = 0; i < MAX_USER; ++i)
	{
		inRoomPlayers[i] = nullptr;
	}
}