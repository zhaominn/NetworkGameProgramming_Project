#pragma once
#define _CRT_SECURE_NO_WARNINGS 

#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment (lib,"WS2_32.LIB")
#pragma comment (lib, "MSWSock.LIB")
#include "protocol.h"
#include <iostream>

class Player;

class Room
{
public:
	MAP_TYPE mapType;
	Player* inRoomPlayers[MAX_USER];
};

Room g_room[MAX_USER];