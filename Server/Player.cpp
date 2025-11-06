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

void Player::send_packet(char* packet, int size)
{
	send(m_socket, (char*)&size, sizeof(int), 0);
	send(m_socket, packet, size, 0);
}

void Player::process_packet(char* p)
{
	const unsigned char packet_type = p[1];
	switch (packet_type) {
	case C2S_LOGIN:
	{
		C2S_Login_Packet* login_packet = reinterpret_cast<C2S_Login_Packet*>(p);

		// if login fail
		if (strlen(login_packet->name) == 0) {
			printf("[Login Fail] empty name\n");
			S2C_Login_Fail_Packet fail_packet;
			fail_packet.size = sizeof(S2C_Login_Fail_Packet);
			fail_packet.type = S2C_LOGIN_FAIL;
			send_packet(reinterpret_cast<char*>(&fail_packet), sizeof(fail_packet));
			disconnect();
		}

		// success
		SetName(login_packet->name);
		std::cout << "[Player : " << m_name << "]" << std::endl;
		std::cout << "[id : " << m_id << "]" << std::endl;

		S2C_PlayerInfo_Packet* info_packet = new S2C_PlayerInfo_Packet;
		info_packet->size = sizeof(S2C_PlayerInfo_Packet);
		info_packet->type = S2C_PLAYER_INFO;
		info_packet->id = (char)m_id;

		send_packet(reinterpret_cast<char*>(info_packet), sizeof(S2C_PlayerInfo_Packet));

		delete info_packet;
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
