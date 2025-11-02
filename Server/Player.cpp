#include "Player.h"

#include <string.h>
#include <array>
#include <windows.h>
extern CRITICAL_SECTION g_CS;
extern std::array<Player, MAX_USER> g_users;

Player::~Player()
{

}

bool Player::recv_packet()
{

	static char recvBuf[BUF_SIZE];
	int32_t len;

	len = recv(this->socket, recvBuf, BUF_SIZE, 0);

	if (len <= 0) {
		printf("[Thread %d] Player %s Close\n", this->id, this->name);
		closesocket(this->socket);

		EnterCriticalSection(&g_CS);
		g_users[id].id = -1;
		g_users[id].socket = INVALID_SOCKET;
		strcpy_s(g_users[id].name, "0");
		LeaveCriticalSection(&g_CS);

		return false;
	}

	// packet_handler(this->id, recvBuf);
	printf("[Thread %d] Packet received from Player %s\n", id, this->name);

	return true;
}

void Player::send_packet(void* packet)
{
	int retval = send(socket, (char*)&packet, sizeof(packet), 0);

	if (retval == SOCKET_ERROR)
	{
		printf("[Player %d] send error\n", id);
		return;
	}
}
