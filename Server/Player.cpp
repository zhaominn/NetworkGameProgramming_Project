#include "Player.h"

#include <string.h>
#include <array>
#include <windows.h>
extern CRITICAL_SECTION g_CS;
extern std::array<Player, MAX_USER> g_users;

Player::~Player()
{
	delete[] m_name;
	m_name = nullptr;
}

bool Player::recv_packet()
{
	char recvBuf[BUF_SIZE];
	int32_t len;

	len = recv(m_socket, recvBuf, BUF_SIZE, 0);

	if (len <= 0) {
		disconnect();

		return false;
	}

	process_packet(recvBuf);
	printf("[Thread %d] Packet received from Player %s (Type: %d)\n",
		m_id, m_name, (unsigned char)recvBuf[1]);

	return true;
}

void Player::send_packet(void* packet)
{
	int retval = send(m_socket, (char*)&packet, sizeof(packet), 0);

	if (retval == SOCKET_ERROR)
	{
		printf("[Player %d] send error\n", m_id);
		return;
	}
}

void Player::process_packet(char* p)
{
	const unsigned char packet_type = p[1];
	switch (packet_type) {
	case C2S_LOGIN:
	{
		C2S_Login_Packet* login_packet = reinterpret_cast<C2S_Login_Packet*>(p);
		SetName(login_packet->name);
		std::cout << "[Player : " << m_name << "]" << std::endl;
		std::cout << "[id : " << m_id << "]" << std::endl;
	}
	break;
	case C2S_IS_READY:
	{

	}
	break;
	case C2S_MOVE:
	{

	}
	break;
	case C2S_BOOSTER:
	{

	}
	break;
	case C2S_LOGOUT:
	{

	}
	break;
	default:
		std::cout << "Error Invalid Packet Type\n";
	}
}


void Player::disconnect()
{
	printf("[Thread %d] Disconnect", m_id);

	EnterCriticalSection(&g_CS);

	// broadcast another users
	closesocket(m_socket);
	g_users[m_id].m_socket = INVALID_SOCKET;
	g_users[m_id].m_id = -1;

	LeaveCriticalSection(&g_CS);
}

void Player::SetSocket(SOCKET socket)
{
	m_socket = socket;
}

void Player::SetId(short id)
{
	m_id = id;
}

void Player::SetName(const char* name)
{
	m_name = new char[NAME_SIZE + 1];
	strncpy(m_name, name, NAME_SIZE);
	m_name[NAME_SIZE] = '\0';
}

void Player::SetX(float x)
{
	m_x = x;
}

void Player::SetY(float y)
{
	m_y = y;
}

void Player::SetZ(float z)
{
	m_z = z;
}

void Player::SetIsReady(bool ready)
{
	isReady = ready;
}

void Player::SetOnline(bool online)
{
	isOnline = online;
}

int Player::GetID() const
{
	return m_id;
}

char* Player::GetName() const
{
	return m_name;
}

float Player::GetX() const
{
	return m_x;
}

float Player::GetY() const
{
	return m_y;
}

float Player::GetZ() const
{
	return m_z;
}

bool Player::GetReady() const
{
	return isReady;
}

bool Player::GetOnline() const
{
	return isOnline;
}
