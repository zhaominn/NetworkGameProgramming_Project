#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment (lib,"WS2_32.LIB")
#pragma comment (lib, "MSWSock.LIB")

#include "protocol.h"
#include "Player.h"
#include <array>

std::array<Player, MAX_USER> g_users;
enum game_state{READY,INGAME,END};

CRITICAL_SECTION g_CS;
float g_ElapsedTime;

int main()
{
	
}