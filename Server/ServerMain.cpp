#include "Player.h"
#include <array>

std::array<Player, MAX_USER> g_users;
enum game_state{READY,INGAME,END};

CRITICAL_SECTION g_CS;
float g_ElapsedTime;

int main()
{
	
}