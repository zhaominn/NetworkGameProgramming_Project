#pragma once
#include "protocol.h"
#include "NetworkMgr.h"

struct PlayerKart {
	int m_id;
	char* m_name;
	float m_yaw;
	float m_speed;
	float m_face_rotation;
	bool isReady;
	bool isOnline;
	KEY_TYPE m_key;
	SOCKET m_socket;

	int m_rank;
	float m_finish_time;

	float x;
	float y;
	float z;
};

extern NetworkMgr networkmgr;
extern std::array<PlayerKart, MAX_USER> g_players;
extern CRITICAL_SECTION CS;

extern int g_myid;
extern bool g_ready;
extern bool g_gameStart;
extern bool g_GameEnd;
extern bool g_setItem;
extern bool g_loginOk;




