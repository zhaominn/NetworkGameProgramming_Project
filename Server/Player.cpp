#include "Player.h"

Player::~Player()
{
	delete[] m_name;
	m_name = nullptr;
}

void Player::recv_packet()
{
	int size;
	char buf[BUF_SIZE];

	int retval = recv(m_socket, (char*)&size, sizeof(int), MSG_WAITALL);
	if (retval <= 0) {
		printf("[Player %d] disconnected (len recv fail)\n", m_id);
		disconnect();
	}

	retval = recv(m_socket, buf, size, MSG_WAITALL);
	if (retval <= 0) {
		printf("[Player %d] disconnected (data recv fail)\n", GetID());
		disconnect();
	}

	process_packet(buf);
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
		SetOnline(true);
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
		C2S_Change_Ready_Packet* change_ready_packet = reinterpret_cast<C2S_Change_Ready_Packet*>(p);

		SetIsReady(change_ready_packet->is_ready);
		std::cout << "[Player : " << m_name << "]" << " ready status? : " << isReady << std::endl;

	}
	break;
	case C2S_MOVE:
	{
		C2S_Move_Packet* login_packet = reinterpret_cast<C2S_Move_Packet*>(p);
		switch (login_packet->direction) {
		case UP: {
			std::cout << "UP" << std::endl;
			if (m_speed < MAX_SPEED) {
				m_speed += ACCELERATION;
				if (m_speed > MAX_SPEED) m_speed = MAX_SPEED;
			}
		}
			   break;
		case DOWN: {
			std::cout << "DOWN" << std::endl;
			if (m_speed > -MAX_SPEED / 2.0f) {
				m_speed -= ACCELERATION;
				if (m_speed < -MAX_SPEED / 2.0f) m_speed = -MAX_SPEED / 2.0f;
			}
		}
				 break;
		case LEFT: {
			std::cout << "LEFT" << std::endl;
			if (m_speed != 0)
				m_yaw += TURN_ANGLE;
		}
				 break;
		case RIGHT: {
			std::cout << "RIGHT" << std::endl;
			if (m_speed != 0)
				m_yaw -= TURN_ANGLE;

		}
				  break;
		}
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
	case C2S_ENTER_ROOM:
	{
		std::cout << "[Player : " << m_name << " enter room]" << std::endl;

		// TEMP
		bool entered = false;
		for (int i = 0; i < 2; ++i)
		{
			Player* roomPlayer = g_room[i].inRoomPlayers[m_id];
			if (roomPlayer != nullptr && roomPlayer->GetID() != -1) {
				entered = true;
				break;
			}
		}
		if (!entered)
			g_room[0].roomManagerID = m_id;

		g_room[0].inRoomPlayers[m_id] = this;
	}
	break;
	default:
		std::cout << "Error Invalid Packet Type\n";
	}
}


void Player::disconnect()
{
	if (m_id != -1)
	{
		printf("[Thread %d] Disconnect", m_id);

		EnterCriticalSection(&g_CS);

		// broadcast another users
		closesocket(m_socket);
		g_users[m_id].m_socket = INVALID_SOCKET;
		g_users[m_id].m_id = -1;

		LeaveCriticalSection(&g_CS);
	}
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

void Player::SetYaw(float yaw)
{
	m_yaw = yaw;
}

void Player::SetSpeed(float speed)
{
	m_speed = speed;
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

float Player::GetYaw() const
{
	return m_yaw;
}

float Player::GetSpeed() const
{
	return m_speed;
}

bool Player::GetReady() const
{
	return isReady;
}

bool Player::GetOnline() const
{
	return isOnline;
}

void Player::send_Game_Start_Packet()
{
	S2C_GameStart_Packet* game_start = new S2C_GameStart_Packet;
	game_start->type = S2C_GAME_START;

	send_packet(reinterpret_cast<char*>(game_start), sizeof(S2C_GameStart_Packet));

	delete game_start;
}
