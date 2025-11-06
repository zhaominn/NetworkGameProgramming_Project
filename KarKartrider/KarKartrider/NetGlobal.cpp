#include "Pch.h"
#include "NetGlobal.h"

NetworkMgr networkmgr;
std::array<PlayerKart, MAX_USER> g_players;
CRITICAL_SECTION CS;