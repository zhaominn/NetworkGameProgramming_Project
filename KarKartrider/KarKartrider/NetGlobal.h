#pragma once
#include "protocol.h"
#include "NetworkMgr.h"

struct PlayerKart {
	int id;
	char name[NAME_SIZE];
	float x, y, z;
	int booster_cnt;
	float speed;
};

extern NetworkMgr networkmgr;
extern std::array<PlayerKart, MAX_USER> g_players;
extern CRITICAL_SECTION CS;

