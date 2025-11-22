#include "Player.h"

// - 212 ~ -304 Map1 EndLine

Player::~Player()
{
	delete[] m_name;
	m_name = nullptr;
}

bool Player::recv_packet()
{
	if (m_socket == INVALID_SOCKET) {
		return false;
	}

	int size;
	char buf[BUF_SIZE];

	// 먼저 길이 수신
	int retval = recv(m_socket, (char*)&size, sizeof(int), MSG_WAITALL);
	if (retval <= 0) {
		printf("[Player %d] disconnected (len recv fail)\n", m_id);
		disconnect();
		return false;
	}

	if (size <= 0 || size > BUF_SIZE) {
		printf("[Player %d] disconnected (invalid packet size: %d)\n", m_id, size);
		disconnect();
		return false;
	}

	int received_total = 0;
	while (received_total < size) {
		retval = recv(m_socket, buf + received_total, size - received_total, 0);
		if (retval <= 0) {
			printf("[Player %d] disconnected (data recv fail)\n", m_id);
			disconnect();
			return false;
		}
		received_total += retval;
	}

	process_packet(buf);
	return true;
}

void Player::send_packet(char* packet, int size)
{
	if (m_socket == INVALID_SOCKET) return;
	send(m_socket, (char*)&size, sizeof(int), 0);
	send(m_socket, packet, size, 0);
}

void Player::broadcast(char* packet, int len) // 자신을 제외한 모든 유저에게 패킷 전송
{
	for (int i = 0; i < MAX_USER; ++i) {
		if (g_users[i].GetSocket() != INVALID_SOCKET) {
			if (g_users[i].GetID() == this->GetID()) continue;
			g_users[i].send_packet(packet, len);
		}
	}
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
			send_Login_Fail_Packet();
			disconnect();
			return;
		}

		// success
		SetName(login_packet->name);
		SetOnline(true);
		std::cout << "[Player : " << m_name << "]" << std::endl;
		std::cout << "[id : " << m_id << "]" << std::endl;

		send_Player_Info_Packet();
	}
	break;
	case C2S_IS_READY:
	{
		C2S_Change_Ready_Packet* change_ready_packet = reinterpret_cast<C2S_Change_Ready_Packet*>(p);

		SetIsReady(change_ready_packet->is_ready);
		m_posX = change_ready_packet->x;
		m_posY = change_ready_packet->y;
		m_posZ = change_ready_packet->z;

		std::cout << "[Player : " << m_name << "]" << " ready status? : " << isReady << std::endl;

	}
	break;
	case C2S_MOVE:
	{
		std::lock_guard<std::mutex> lock1(g_UserMutex);

		C2S_Move_Packet* move_packet = reinterpret_cast<C2S_Move_Packet*>(p);
		m_up = move_packet->up;
		m_down = move_packet->down;
		m_left = move_packet->left;
		m_right = move_packet->right;

		if (m_up)
		{
			SetSpeed(m_speed + ACCELERATION);
		}
		if (m_down)
		{
			SetSpeed(m_speed - ACCELERATION);
		}
		if (m_left)
		{
			SetYaw(m_yaw + TURN_ANGLE);
			float f = GetFaceRotation();
			f -= RETURN_SPEED;
			SetFaceRotation(f);
			SetBodyRotation(TURN_ANGLE);
		}
		if (m_right)
		{
			SetYaw(m_yaw - TURN_ANGLE);
			float f = GetFaceRotation();
			f += RETURN_SPEED;
			SetFaceRotation(f);
			SetBodyRotation(-TURN_ANGLE);
		}


		if (!m_up && !m_down)
		{
			if (GetSpeed() > 0.0f) {
				SetSpeed(m_speed - DECELERATION);
				if (GetSpeed() < 0.0f) SetSpeed(0.0f);
			}
			else if (GetSpeed() < 0.0f) {
				SetSpeed(m_speed + DECELERATION);
				if (GetSpeed() > 0.0f) SetSpeed(0.0f);
			}
		}

		if (!m_left && !m_right)
		{
			float f = GetFaceRotation();
			if (f > 0.0f) {
				f -= RETURN_SPEED;
				if (f < 0.0f) f = 0.0f;
			}
			else if (f < 0.0f) {
				f += RETURN_SPEED;
				if (f > 0.0f) f = 0.0f;
			}
			SetFaceRotation(f);
			SetBodyRotation(0.0);
		}

		if (m_speed > MAX_SPEED) SetSpeed(MAX_SPEED);
		if (m_speed < -MAX_SPEED / 2.0f) SetSpeed(-MAX_SPEED / 2.0f);

		float f_limit = GetFaceRotation();
		if (f_limit > MAX_FACE_ROTATION) f_limit = MAX_FACE_ROTATION;
		if (f_limit < -MAX_FACE_ROTATION) f_limit = -MAX_FACE_ROTATION;
		SetFaceRotation(f_limit);

		// -------------- 일괄 이동 처리 --------------
		float rad = GetYaw() * PI / 180.0f;

		float dirX = -sinf(rad);
		float dirZ = -cosf(rad);

		m_posX += GetSpeed() * dirX;
		m_posZ += GetSpeed() * dirZ;
	}
	break;
	case C2S_BOOSTER:
	{
		//if (!isBoosterActive) {
		isBoosterActive = true;
		std::cout << "booster on!" << std::endl;
		/*		return;
			}*/

		if (isBoosterActive) {
			if (m_booster_head_tilt < MAX_HEAD_TILT) {
				m_booster_head_tilt += TILT_SPEED;
				if (m_booster_head_tilt > MAX_HEAD_TILT) {
					m_booster_head_tilt = MAX_HEAD_TILT;
				}
			}
		}
		else {
			if (m_booster_head_tilt > 0.0f) {
				m_booster_head_tilt -= TILT_SPEED;
				if (m_booster_head_tilt < 0.0f) {
					m_booster_head_tilt = 0.0f;
				}
			}
		}

		send_booster_packet();

	}
	break;
	case C2S_LOGOUT:
	{
		g_game_state = LOBBY;
		disconnect();
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
	if (m_socket == INVALID_SOCKET && m_id == -1) return;

	int old_id = m_id;

	EnterCriticalSection(&g_CS);

	if (m_socket != INVALID_SOCKET) {
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
	}

	if (old_id >= 0 && old_id < MAX_USER) {
		g_users[old_id].m_socket = INVALID_SOCKET;
		g_users[old_id].m_id = -1;
		g_users[old_id].isOnline = false;
		for (int r = 0; r < 2; ++r) {
			for (int i = 0; i < MAX_USER; ++i) {
				if (g_room[r].inRoomPlayers[i] == this) {
					g_room[r].inRoomPlayers[i] = nullptr;
				}
			}
			if (g_room[r].roomManagerID == old_id) g_room[r].roomManagerID = -1;
		}
		if (g_usersNum > 0) g_usersNum--;
		g_AllPlayerLogin = false;
	}

	LeaveCriticalSection(&g_CS);

	reset();

	// TEMP
	g_game_state = LOBBY;
	g_GameStart = false;
	g_room->reset();
}

void Player::send_Player_Info_Packet()
{
	S2C_PlayerInfo_Packet* info_packet = new S2C_PlayerInfo_Packet;
	info_packet->size = sizeof(S2C_PlayerInfo_Packet);
	info_packet->type = S2C_PLAYER_INFO;
	info_packet->id = (char)m_id;

	send_packet(reinterpret_cast<char*>(info_packet), sizeof(S2C_PlayerInfo_Packet));

	delete info_packet;
}

void Player::send_Login_Fail_Packet()
{
	printf("[Login Fail] empty name\n");
	S2C_Login_Fail_Packet fail_packet;
	fail_packet.size = sizeof(S2C_Login_Fail_Packet);
	fail_packet.type = S2C_LOGIN_FAIL;
	send_packet(reinterpret_cast<char*>(&fail_packet), sizeof(fail_packet));
}

void Player::send_Ready_Packet()
{
	S2C_Ready_Packet* ready_pkt = new S2C_Ready_Packet;
	ready_pkt->size = sizeof(S2C_Ready_Packet);
	ready_pkt->type = S2C_IS_READY;
	ready_pkt->id = GetID();
	ready_pkt->is_ready = GetReady();
	broadcast(reinterpret_cast<char*>(ready_pkt), sizeof(S2C_Ready_Packet));
	delete ready_pkt;
}

void Player::send_Game_Start_Packet()
{
	S2C_GameStart_Packet* game_start = new S2C_GameStart_Packet;
	game_start->size = sizeof(S2C_GameStart_Packet);
	game_start->type = S2C_GAME_START;

	send_packet(reinterpret_cast<char*>(game_start), sizeof(S2C_GameStart_Packet));

	delete game_start;
}

void Player::send_move_Packet()
{
	S2C_Move_Packet* move_pkt = new S2C_Move_Packet;
	move_pkt->size = sizeof(S2C_Move_Packet);
	move_pkt->type = S2C_MOVE;

	std::lock_guard<std::mutex> lock2(g_Sendmutex);
	move_pkt->id = GetID();
	move_pkt->booster_cnt = GetBodyRotation();
	move_pkt->speed = GetSpeed();
	move_pkt->yaw = GetYaw();
	move_pkt->x = m_posX;
	move_pkt->y = m_posY;
	move_pkt->z = m_posZ;
	move_pkt->face_rotation = GetFaceRotation();
	move_pkt->body_rotation = GetBodyRotation();

	send_packet(reinterpret_cast<char*>(move_pkt), sizeof(S2C_Move_Packet));
}

void Player::send_booster_packet()
{
	S2C_Booster_Packet* booster_pkt = new S2C_Booster_Packet;
	booster_pkt->id = GetID();
	booster_pkt->size = sizeof(S2C_Booster_Packet);
	booster_pkt->type = S2C_BOOSTER;
	booster_pkt->booster_head_tilt = GetHeadtilt();
	send_packet(reinterpret_cast<char*>(booster_pkt), sizeof(S2C_Booster_Packet));

	delete booster_pkt;
}

void Player::send_Rank_Packet()
{
	S2C_Rank_Packet* rank_pkt = new S2C_Rank_Packet;
	rank_pkt->size = sizeof(S2C_Rank_Packet);
	rank_pkt->type = S2C_RANK;
	rank_pkt->rank = g_rankCnt++;
	rank_pkt->finish_time = g_ElapsedTime;

	send_packet(reinterpret_cast<char*>(rank_pkt), sizeof(S2C_Rank_Packet));

	delete rank_pkt;
}

void Player::checkIsFinished()
{
	if (!isFinished && (g_room[0].mapType == STRAIGHT) && (m_posZ <= -212))
	{
		isFinished = true;
		send_Rank_Packet();
	}
}

void Player::reset()
{
	if (m_name) {
		delete[] m_name;
		m_name = nullptr;
	}

	m_name = new char[1];
	m_name[0] = '\0';

	m_id = -1;
	m_yaw = 0;
	m_speed = 0;
	m_face_rotation = 0;
	m_body_rotation = 0;
	m_booster_cnt = 2;
	isReady = false;
	isOnline = false;
	isBoosterActive = false;
	m_socket = INVALID_SOCKET;
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
	if (m_name) {
		delete[] m_name;
		m_name = nullptr;
	}
	m_name = new char[NAME_SIZE + 1];
	strncpy(m_name, name, NAME_SIZE);
	m_name[NAME_SIZE] = '\0';
}

void Player::SetBoosterCnt(int booster)
{
	m_booster_cnt = booster;
}

void Player::SetYaw(float yaw)
{
	m_yaw = yaw;
}

void Player::SetSpeed(float speed)
{
	m_speed = speed;
}

void Player::SetFaceRotation(float f_rotation)
{
	m_face_rotation = f_rotation;
}

void Player::SetBodyRotation(float b_rotation)
{
	m_body_rotation = b_rotation;
}

void Player::SetHeadTilt(float headtilt)
{
	m_booster_head_tilt = headtilt;
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

int Player::GetBoosterCnt() const
{
	return m_booster_cnt;
}

char* Player::GetName() const
{
	return m_name;
}

float Player::GetYaw() const
{
	return m_yaw;
}

float Player::GetSpeed() const
{
	return m_speed;
}

float Player::GetFaceRotation() const
{
	return m_face_rotation;
}

float Player::GetBodyRotation() const
{
	return m_body_rotation;
}

float Player::GetHeadtilt() const
{
	return m_booster_head_tilt;
}

bool Player::GetReady() const
{
	return isReady;
}

bool Player::GetOnline() const
{
	return isOnline;
}