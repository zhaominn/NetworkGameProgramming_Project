#include "Player.h"


Player::~Player()
{

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
