#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa

#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment (lib,"WS2_32.LIB")
#pragma comment (lib, "MSWSock.LIB")

#include <string.h>
#include <windows.h>
#include <iostream>
#include <array>
#include <chrono>
#include <mutex>

#include "protocol.h"


class Room;
class Player;
extern Room g_room[2];
extern std::array<Player, MAX_USER> g_users;

extern CRITICAL_SECTION g_CS;
extern GAME_STATE g_game_state;

extern float g_ElapsedTime;

extern int g_usersNum;
extern bool g_AllPlayerLogin;
extern bool g_AllPlayerReady;
extern bool g_GameStart;
extern bool g_GameEnd;
int g_rankCnt;

extern std::mutex g_Sendmutex;
extern std::mutex g_UserMutex;