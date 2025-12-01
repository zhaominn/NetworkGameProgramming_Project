#pragma once
#include "protocol.h"
#include "NetworkMgr.h"

struct PlayerKart {
	int m_id;
	int m_booster_cnt = 2;
	float m_yaw;
	float m_speed;
	float m_face_rotation;
	float m_body_rotation;
	float m_booster_head_tilt;
	bool isReady;
	bool isOnline;
	bool isBoosterOn;
	SOCKET m_socket;
	char m_name[NAME_SIZE];

	int m_rank;
	float m_finish_time;

	float x;
	float y;
	float z;

	MAP_TYPE select_map = MAP_TYPE::STRAIGHT;
};

extern NetworkMgr networkmgr;
extern std::array<PlayerKart, MAX_USER> g_players;
extern CRITICAL_SECTION CS;

extern int g_myid;
extern bool g_gameStart;
extern bool g_GameEnd;
extern bool g_setItem;
extern bool g_loginOk;

extern std::vector<int> g_roomPlayers;
extern char g_myRoomId;





