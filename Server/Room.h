#pragma once
#include "global.h"

class Room
{
public:
	MAP_TYPE mapType;
	Player* inRoomPlayers[MAX_USER];
	int roomManagerID;
	bool gameStart;
	float elapsedTime;

public:
	Room() { reset(); } // 생성 시 기본 초기화
	void reset();
};