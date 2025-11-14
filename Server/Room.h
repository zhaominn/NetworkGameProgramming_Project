#pragma once
#include "global.h"

class Room
{
public:
	MAP_TYPE mapType;
	Player* inRoomPlayers[MAX_USER];
	char roomManagerID;
};