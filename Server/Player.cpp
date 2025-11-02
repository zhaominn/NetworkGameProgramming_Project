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

	len = recv(socket, recvBuf, BUF_SIZE, 0);

	if (len <= 0) {
		disconnect();

		return false;
	}

	// packet_handler(id, recvBuf);
	printf("[Thread %d] Packet received from Player %s (Type: %d)\n",
		id, name, (unsigned char)recvBuf[1]);

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

void Player::disconnect()
{
	// broadcast another users
	closesocket(socket);

	printf("[Thread %d] Disconnect", id);

	EnterCriticalSection(&g_CS);
	g_users[id].id = -1;
	g_users[id].socket = INVALID_SOCKET;
	strcpy_s(g_users[id].name, "0");
	LeaveCriticalSection(&g_CS);
}
