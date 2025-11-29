#include "Pch.h"
#include "NetGlobal.h"

NetworkMgr networkmgr;
std::array<PlayerKart, MAX_USER> g_players;
CRITICAL_SECTION CS;

int g_myid;
bool g_ready;
bool g_gameStart = false;
bool g_GameEnd = false;
bool g_setItem = false;
bool g_loginOk = false;

std::vector<int> g_roomPlayers;
char g_myRoomId;
