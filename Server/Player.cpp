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

		SetIsReady(!GetReady());

		if (select_map == MAP_TYPE::STRAIGHT) {
			m_posX = 0;
			m_posY = 2.6;
			m_posZ = 238.0;
		}
		else if (select_map == MAP_TYPE::RECTANGLE) {
			m_posX = 165.0;
			m_posY = 1.0;
			m_posZ = 30.0;
		}

		switch (m_id)
		{
		case 0:
			m_posX += -5;
			break;
		case 1:
			m_posX += 0;
			break;
		case 2:
			m_posX += 5;
			break;
		default:
			break;
		}

		std::cout << "[Player : " << m_name << "]" << " ready status? : " << isReady << std::endl;

	}
	break;
	case C2S_MOVE:
	{
		C2S_Move_Packet* move_packet = reinterpret_cast<C2S_Move_Packet*>(p);
		m_up = move_packet->up;
		m_down = move_packet->down;
		m_left = move_packet->left;
		m_right = move_packet->right;

		float max_speed = MAX_SPEED;
		float acceleration = ACCELERATION;

		if (isBoosterActive) {
			max_speed = BOOSTER_SPEED;
			acceleration = ACCELERATION * 1.5f;
		}

		if (m_up)
		{
			SetSpeed(m_speed + acceleration);
		}
		if (m_down)
		{
			SetSpeed(m_speed - acceleration);
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

		if (m_speed > max_speed) SetSpeed(max_speed);
		if (m_speed < -max_speed / 2.0f) SetSpeed(-max_speed / 2.0f);

		float f_limit = GetFaceRotation();
		if (f_limit > MAX_FACE_ROTATION) f_limit = MAX_FACE_ROTATION;
		if (f_limit < -MAX_FACE_ROTATION) f_limit = -MAX_FACE_ROTATION;
		SetFaceRotation(f_limit);

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
		C2S_Booster_Packet* booster_packet = reinterpret_cast<C2S_Booster_Packet*>(p);
		SetBoosterCnt(booster_packet->booster_cnt);
		ActiveBooster();
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
		C2S_Enter_Room_Packet* packet = reinterpret_cast<C2S_Enter_Room_Packet*>(p);

		int newRoomIdx = packet->map; // 이동하려는 새 방 인덱스

		int oldRoomIdx = select_map;

		if (oldRoomIdx >= 0 && oldRoomIdx < 2 && g_room[oldRoomIdx].inRoomPlayers[m_id] == this)
		{
			Room& oldRoom = g_room[oldRoomIdx];

			oldRoom.inRoomPlayers[m_id] = nullptr;

			// 만약 내가 그 방의 방장(Manager)이었다면?
			if (oldRoom.roomManagerID == m_id)
			{
				oldRoom.roomManagerID = -1;

				// 새로운 방장 선출
				for (int i = 0; i < MAX_USER; ++i)
				{
					if (oldRoom.inRoomPlayers[i] != nullptr)
					{
						oldRoom.roomManagerID = oldRoom.inRoomPlayers[i]->GetID();
						
						break; // 한 명 찾았으면 종료
					}
				}

				// 만약 아무도 없다면 roomManagerID는 -1로 유지
			}
		}

		
		Room& newRoom = g_room[newRoomIdx];

		std::cout << "[Player : " << m_name << " enter room " << newRoomIdx << "]" << std::endl;

		
		if (newRoom.inRoomPlayers[m_id] != nullptr)
		{
			// 이미 들어온 유저
			std::cout << m_name << " already in target room" << std::endl;
			break;
		}

		// 새 방에 방장이 없는 경우 내가 방장
		if (newRoom.roomManagerID == -1)
			newRoom.roomManagerID = m_id;

		this->select_map = packet->map;
		newRoom.inRoomPlayers[m_id] = this;
		newRoom.mapType = packet->map;

		send_Enter_Room_Packet(packet->map, (newRoom.roomManagerID == m_id));
	}
	break;
	case C2S_WALL_COLLISION_1:
	{
		C2S_Wall_Collision_1_Packet* packet = reinterpret_cast<C2S_Wall_Collision_1_Packet*>(p);

		for (int i = 0; i < 5; ++i) {
			g_Map1Colliders[i].minX = packet->aabbs[i].minX;
			g_Map1Colliders[i].minY = packet->aabbs[i].minY;
			g_Map1Colliders[i].minZ = packet->aabbs[i].minZ;

			g_Map1Colliders[i].maxX = packet->aabbs[i].maxX;
			g_Map1Colliders[i].maxY = packet->aabbs[i].maxY;
			g_Map1Colliders[i].maxZ = packet->aabbs[i].maxZ;
			g_Map1Colliders[i].rigid_status = packet->aabbs[i].rigid_status;
		}

		/*for (int i = 0; i < 5; ++i) {
			std::cout << "AABB Min: " << g_Map1Colliders[i].minX << ", "
				<< g_Map1Colliders[i].minY << ", "
				<< g_Map1Colliders[i].minZ << std::endl;

			std::cout << "AABB Max: " << g_Map1Colliders[i].maxX << ", " << g_Map1Colliders[i].maxY << ", "
				<< g_Map1Colliders[i].maxZ << std::endl;
		}

		for (int i = 0; i < 5; ++i) {
			std::cout << "충돌 상태 : " << g_Map1Colliders[i].rigid_status << std::endl;
		}*/

		break;
	}
	case C2S_WALL_COLLISION_2:
	{
		C2S_Wall_Collision_2_Packet* packet = reinterpret_cast<C2S_Wall_Collision_2_Packet*>(p);

		for (int i = 0; i < 18; ++i) {
			g_Map2Colliders[i].minX = packet->aabbs[i].minX;
			g_Map2Colliders[i].minY = packet->aabbs[i].minY;
			g_Map2Colliders[i].minZ = packet->aabbs[i].minZ;

			g_Map2Colliders[i].maxX = packet->aabbs[i].maxX;
			g_Map2Colliders[i].maxY = packet->aabbs[i].maxY;
			g_Map2Colliders[i].maxZ = packet->aabbs[i].maxZ;
			g_Map2Colliders[i].rigid_status = packet->aabbs[i].rigid_status;
		}

		/*for (int i = 0; i < 5; ++i) {
			std::cout << "AABB Min: " << g_Map1Colliders[i].minX << ", "
				<< g_Map1Colliders[i].minY << ", "
				<< g_Map1Colliders[i].minZ << std::endl;

			std::cout << "AABB Max: " << g_Map1Colliders[i].maxX << ", " << g_Map1Colliders[i].maxY << ", "
				<< g_Map1Colliders[i].maxZ << std::endl;
		}

		for (int i = 0; i < 5; ++i) {
			std::cout << "충돌 상태 : " << g_Map1Colliders[i].rigid_status << std::endl;
		}*/

		break;
	}
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

void Player::send_Enter_Room_Packet(MAP_TYPE map, bool isRoomMaster)
{
	S2C_EnterRoom_Packet enter_room_packet;
	enter_room_packet.size = sizeof(S2C_EnterRoom_Packet);
	enter_room_packet.type = S2C_ENTER_ROOM;
	enter_room_packet.id = m_id;
	enter_room_packet.map = map;
	strncpy(enter_room_packet.name, m_name, NAME_SIZE - 1);
	enter_room_packet.name[NAME_SIZE - 1] = '\0';
	enter_room_packet.isRoomMaster = isRoomMaster;
	send_packet(reinterpret_cast<char*>(&enter_room_packet), sizeof(enter_room_packet));
	for (int i = 0; i < MAX_USER; i++) // 같은 방에만 브로드캐스팅
	{
		Player* p = g_room[map].inRoomPlayers[i];
		if (!p) continue;

		p->send_packet(reinterpret_cast<char*>(&enter_room_packet), sizeof(enter_room_packet));
	}
}

void Player::send_Ready_Packet()
{
	S2C_Ready_Packet* ready_pkt = new S2C_Ready_Packet;
	ready_pkt->size = sizeof(S2C_Ready_Packet);
	ready_pkt->type = S2C_IS_READY;
	ready_pkt->id = GetID();
	ready_pkt->is_ready = GetReady();
	send_packet(reinterpret_cast<char*>(ready_pkt), sizeof(S2C_Ready_Packet));
	broadcast(reinterpret_cast<char*>(ready_pkt), sizeof(S2C_Ready_Packet));
	delete ready_pkt;
}

void Player::send_Game_Start_Packet(Room rooms[2])
{
	int roomId = this->select_map;
	Room& room = rooms[roomId];

	S2C_GameStart_Packet pkt;
	pkt.type = S2C_GAME_START;

	pkt.roomId = roomId;

	int count = 0;
	for (int i = 0; i < MAX_USER; i++)
	{
		Player* p = room.inRoomPlayers[i];
		if (!p) continue;

		pkt.players[count].id = p->GetID();
		pkt.players[count].mapType = p->select_map;
		count++;
	}

	pkt.playerCount = count;

	pkt.size = sizeof(S2C_GameStart_Packet) - (sizeof(RoomPlayer) * (MAX_USER - count));

	send_packet(reinterpret_cast<char*>(&pkt), pkt.size);
}

void Player::send_move_Packet()
{

	S2C_Move_Packet* move_pkt = new S2C_Move_Packet;
	move_pkt->size = sizeof(S2C_Move_Packet);
	move_pkt->type = S2C_MOVE;

	for (int i = 0; i < MAX_USER; ++i) {

		move_pkt->arr[i].id = g_users[i].GetID();
		move_pkt->arr[i].speed = g_users[i].GetSpeed();
		move_pkt->arr[i].yaw = g_users[i].GetYaw();
		move_pkt->arr[i].face_rotation = g_users[i].GetFaceRotation();
		move_pkt->arr[i].body_rotation = g_users[i].GetBodyRotation();
		move_pkt->arr[i].booster_head_tilt = g_users[i].GetHeadtilt();
		move_pkt->arr[i].x = g_users[i].m_posX;
		move_pkt->arr[i].y = g_users[i].m_posY;
		move_pkt->arr[i].z = g_users[i].m_posZ;
		move_pkt->arr[i].z = g_users[i].m_posZ;
		move_pkt->arr[i].boosterOn = g_users[i].isBoosterActive;
	}

	move_pkt->id = GetID();
	move_pkt->booster_cnt = GetBodyRotation();
	move_pkt->speed = GetSpeed();
	move_pkt->yaw = GetYaw();
	move_pkt->x = m_posX;
	move_pkt->y = m_posY;
	move_pkt->z = m_posZ;
	move_pkt->face_rotation = GetFaceRotation();
	move_pkt->body_rotation = GetBodyRotation();
	move_pkt->booster_head_tilt = GetHeadtilt();

	send_packet(reinterpret_cast<char*>(move_pkt), sizeof(S2C_Move_Packet));
	//broadcast(reinterpret_cast<char*>(move_pkt), sizeof(S2C_Move_Packet));
}

void Player::send_booster_packet()
{
	S2C_Booster_Packet* booster_pkt = new S2C_Booster_Packet;
	booster_pkt->id = GetID();
	booster_pkt->size = sizeof(S2C_Booster_Packet);
	booster_pkt->type = S2C_BOOSTER;
	booster_pkt->boosterOn = isBoosterActive;
	booster_pkt->booster_cnt = m_booster_cnt;
	send_packet(reinterpret_cast<char*>(booster_pkt), sizeof(S2C_Booster_Packet));

	delete booster_pkt;
}

void Player::ActiveBooster()
{
	if (isBoosterActive) {
		std::cout << "Booster is already active!" << std::endl;
		return;
	}

	if (m_booster_cnt > 0) {
		--m_booster_cnt;
		isBoosterActive = true;
		m_boosterEndTime = std::chrono::steady_clock::now() + std::chrono::seconds(3);
		std::cout << "Booster activated! Remaining: " << m_booster_cnt << std::endl;
	}

	send_booster_packet();
}

void Player::CheckBoosterState()
{
	if (!isBoosterActive) return;

	if (std::chrono::steady_clock::now() >= m_boosterEndTime)
	{
		++m_booster_cnt;
		isBoosterActive = false;
		std::cout << "Booster Deactivated!" << std::endl;
	}

	send_booster_packet();
}

void Player::CheckCollision()
{
	float oldX = m_posX;
	float oldZ = m_posZ;

	float px = 0.f, pz = 0.f;

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (i == m_id) continue;
		if (!g_users[i].GetOnline()) continue;

		float pushX, pushZ;
		if (PlayerCollisionCheck(m_id, i, pushX, pushZ))
		{
			px += pushX;
			pz += pushZ;
		}
	}

	if (px != 0 || pz != 0)
	{
		m_posX += px * 0.5f;
		m_posZ += pz * 0.5f;

		float nx = 0.f, nz = 0.f;
		if (px != 0) nx = (px > 0 ? 1.0f : -1.0f);
		if (pz != 0) nz = (pz > 0 ? 1.0f : -1.0f);

		float yaw = GetYaw();
		float speed = GetSpeed();

		// local speed vector
		float vx = cosf(yaw) * speed;
		float vz = sinf(yaw) * speed;

		float dot = vx * nx + vz * nz;

		float rvx = vx - 2 * dot * nx;
		float rvz = vz - 2 * dot * nz;

		const float bounce = 0.7f;

		rvx *= bounce;
		rvz *= bounce;

		float newSpeed = sqrtf(rvx * rvx + rvz * rvz);
		float newYaw = atan2f(rvz, rvx);

		SetSpeed(newSpeed);
		SetYaw(newYaw);
	}
}

bool Player::PlayerCollisionCheck(int a, int b, float& pushX, float& pushZ)
{
	float x1 = g_users[a].m_posX;
	float z1 = g_users[a].m_posZ;

	float x2 = g_users[b].m_posX;
	float z2 = g_users[b].m_posZ;

	float hx1 = g_users[a].m_colliderHalfX;
	float hz1 = g_users[a].m_colliderHalfZ;

	float hx2 = g_users[b].m_colliderHalfX;
	float hz2 = g_users[b].m_colliderHalfZ;

	float dx = x1 - x2;
	float dz = z1 - z2;

	float overlapX = (hx1 + hx2) - fabs(dx);
	float overlapZ = (hz1 + hz2) - fabs(dz);

	if (overlapX > 0 && overlapZ > 0)
	{
		if (overlapX < overlapZ)
		{
			pushX = (dx > 0 ? overlapX : -overlapX);
			pushZ = 0;
		}
		else
		{
			pushX = 0;
			pushZ = (dz > 0 ? overlapZ : -overlapZ);
		}
		return true;
	}

	return false;
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

void Player::SetBoosterStatus(bool booster_status)
{
	isBoosterActive = booster_status;
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

bool Player::GetBoosterStatus() const
{
	return isBoosterActive;
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