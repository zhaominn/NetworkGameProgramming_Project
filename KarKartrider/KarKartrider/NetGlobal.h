#pragma once
#include "protocol.h"
#include "NetworkMgr.h"

struct PlayerKart {
	int m_id;
	char* m_name;
	float m_x, m_y, m_z;
	float m_yaw;
	float m_speed;
	float m_face_rotation;
	bool isReady;
	bool isOnline;
	KEY_TYPE m_key;
	SOCKET m_socket;
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




