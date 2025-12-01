#include "global.h"
#include "Room.h"
#include "Player.h"

Room g_room[2];
std::array<Player, MAX_USER> g_users;
GAME_STATE g_game_state{ LOBBY };

float g_ElapsedTime = 0.0f;
int g_usersNum = 0;
bool g_AllPlayerLogin = false;
bool g_AllPlayerReady = false;
bool g_GameStart = false;
bool g_GameEnd = false;
int g_rankCnt = 1;

const float PI = 3.1415926535f;

CRITICAL_SECTION g_CS;