#include "Pch.h"
#include "NetworkMgr.h"
#include "root.h"
#include "Map1_Mode.h"
#include "Map2_Mode.h"
#include "RoomMode.h"

std::thread m_recvThread;
std::atomic<bool> m_running{ false };

NetworkMgr::NetworkMgr()
{
	m_sock = -1;	// < 0 이면 초기화가 안된거임
}

NetworkMgr::~NetworkMgr()
{
	StopRunning();

	// 윈속 종료
	WSACleanup();
}

bool NetworkMgr::Init()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	// 소켓 생성
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET) {
		std::cout << "socket error" << std::endl;
	}

	char SERVERIP[30]{};
	std::cout << "ip 주소를 입력하세요" << std::endl;
	std::cin >> SERVERIP;

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVER_PORT);
	int retval = connect(m_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		std::cout << "fail to connect" << std::endl;
		return false;
	}

	BOOL optVal = TRUE;
	setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&optVal, sizeof(optVal));

	std::cout << "success to connect! (server: " << SERVERIP << ")" << std::endl;
	return true;
}

ModeType NetworkMgr::GetCurrentModeType() const
{
	if (m_currentMode) {
		return m_currentMode->GetModeType();
	}
	return ModeType::NONE;
}

void NetworkMgr::StartRunning()
{
	m_running = true;
}

void NetworkMgr::StopRunning()
{
	m_running = false;

	if (m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}

void NetworkMgr::reset()
{
	m_currentMode = nullptr;
	g_players[g_myid].isReady = false;
	g_players[g_myid].isBoosterOn = false;
	g_gameStart = false;
	g_GameEnd = false;
	g_delta_time = 0;
}


bool NetworkMgr::IsRunning() const
{
	return m_running;
}

void NetworkMgr::SendPacket(char* packet, int size)
{
	send(m_sock, (char*)&size, sizeof(int), 0);
	send(m_sock, packet, size, 0);
}

void NetworkMgr::SendEnterRoomPacket(MAP_TYPE map)
{
	C2S_Enter_Room_Packet* enter_room_packet = new C2S_Enter_Room_Packet;
	enter_room_packet->size = sizeof(C2S_Enter_Room_Packet);
	enter_room_packet->type = C2S_ENTER_ROOM;
	enter_room_packet->map = map;

	SendPacket(reinterpret_cast<char*>(enter_room_packet), sizeof(C2S_Enter_Room_Packet));
	delete enter_room_packet;
}

void NetworkMgr::SendLeaveRoomPacket(MAP_TYPE map)
{
	C2S_Leave_Game_Packet* leave_room_packet = new C2S_Leave_Game_Packet;
	leave_room_packet->size = sizeof(C2S_Leave_Game_Packet);
	leave_room_packet->type = C2S_LEAVE_GAME;
	leave_room_packet->map = map;

	SendPacket(reinterpret_cast<char*>(leave_room_packet), sizeof(C2S_Leave_Game_Packet));
	delete leave_room_packet;
}

void NetworkMgr::SendLoginPacket(std::string name)
{
	C2S_Login_Packet* login_packet = new C2S_Login_Packet;
	login_packet->size = sizeof(C2S_Login_Packet);
	login_packet->type = C2S_LOGIN;
	strncpy(login_packet->name, name.c_str(), NAME_SIZE - 1);
	login_packet->name[NAME_SIZE - 1] = '\0';
	SendPacket(reinterpret_cast<char*>(login_packet), sizeof(C2S_Login_Packet));
	delete login_packet;
}

void NetworkMgr::SendChangeReadyPacket()
{
	C2S_Change_Ready_Packet* change_ready_packet = new C2S_Change_Ready_Packet;
	change_ready_packet->size = sizeof(C2S_Change_Ready_Packet);
	change_ready_packet->type = C2S_IS_READY;

	SendPacket(reinterpret_cast<char*>(change_ready_packet), sizeof(C2S_Change_Ready_Packet));
	delete change_ready_packet;
}

void NetworkMgr::SendMovePacket(bool up, bool down, bool left, bool right)
{
	C2S_Move_Packet* packet = new C2S_Move_Packet;
	packet->type = C2S_MOVE;
	packet->up = up;
	packet->down = down;
	packet->left = left;
	packet->right = right;

	SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Move_Packet));
	delete packet;
}

void NetworkMgr::SendBoosterPacket(bool boosterOn, int booster_cnt)
{
	C2S_Booster_Packet* packet = new C2S_Booster_Packet;
	packet->type = C2S_BOOSTER;
	packet->boosterOn = boosterOn;
	packet->booster_cnt = booster_cnt;

	SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Booster_Packet));
	delete packet;
}

void NetworkMgr::SendWallCollisionPacket_1(AABB aabb[5])
{
	C2S_Wall_Collision_1_Packet* packet = new C2S_Wall_Collision_1_Packet;
	packet->size = sizeof(C2S_Wall_Collision_1_Packet);
	packet->type = C2S_WALL_COLLISION_1;
	memcpy(packet->aabbs, aabb, sizeof(AABB) * 5);

	SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Wall_Collision_1_Packet));
	delete packet;
}

void NetworkMgr::SendWallCollisionPacket_2(AABB aabb[18])
{
	C2S_Wall_Collision_2_Packet* packet = new C2S_Wall_Collision_2_Packet;
	packet->size = sizeof(C2S_Wall_Collision_2_Packet);
	packet->type = C2S_WALL_COLLISION_2;
	memcpy(packet->aabbs, aabb, sizeof(AABB) * 18);

	SendPacket(reinterpret_cast<char*>(packet), sizeof(C2S_Wall_Collision_2_Packet));
	delete packet;
}

void NetworkMgr::ProcessPacket(char* buf)
{
	unsigned char type = buf[1];
	switch (type) {
	case S2C_PLAYER_INFO:
	{
		std::cout << "로그인 성공!" << std::endl;

		S2C_PlayerInfo_Packet* playerinfo_packet = reinterpret_cast<S2C_PlayerInfo_Packet*>(buf);
		g_myid = playerinfo_packet->id;
		g_players[g_myid].m_id = g_myid;
		SendEnterRoomPacket(STRAIGHT);
	}
	break;
	case S2C_LOGIN_FAIL:
	{

		// 서버에서 보낸 로그인 패킷
		std::cout << "로그인 실패!" << std::endl;
	}
	break;
	case S2C_ENTER_ROOM:
	{
		S2C_EnterRoom_Packet* packet = reinterpret_cast<S2C_EnterRoom_Packet*>(buf);

		g_players[packet->id].m_id = packet->id;
		g_players[packet->id].select_map = packet->map;
		strncpy(g_players[packet->id].m_name, packet->name, NAME_SIZE - 1);
		g_players[packet->id].m_name[NAME_SIZE - 1] = '\0';
		g_players[packet->id].isReady = false;
		g_players[packet->id].isOnline = true;

	}
	break;
	case S2C_LEAVE_ROOM:
	{
		S2C_LeaveRoom_Packet* packet = reinterpret_cast<S2C_LeaveRoom_Packet*>(buf);
		g_players[packet->id].isOnline = false;

		if (packet->id >= 0 && packet->id < MAX_USER)
			g_players[packet->id].isReady = false;
	}
	break;
	case S2C_IS_READY:
	{
		S2C_Ready_Packet* pkt = reinterpret_cast<S2C_Ready_Packet*>(buf);
		g_players[pkt->id].isReady = pkt->is_ready;
	}
	break;
	case S2C_GAME_START:
	{
		S2C_GameStart_Packet* pkt = reinterpret_cast<S2C_GameStart_Packet*>(buf);

		std::cout << "==== Game Start ====" << std::endl;
		std::cout << "roomId = " << (int)pkt->roomId << std::endl;
		std::cout << "playerCount = " << (int)pkt->playerCount << std::endl;

		// 1. 내가 속한 방 번호 저장
		g_myRoomId = pkt->roomId;

		// 2. 같은 방 플레이어 목록 저장
		g_roomPlayers.clear();

		for (int i = 0; i < pkt->playerCount; i++)
		{
			int id = pkt->players[i].id;
			MAP_TYPE mapType = (MAP_TYPE)pkt->players[i].mapType;

			g_players[id].select_map = mapType; // 각 플레이어의 맵 정보 저장
			g_roomPlayers.push_back(id);

			std::cout << "방 플레이어: " << id
				<< " , map = " << (int)mapType << std::endl;
		}

		// 3. 내 맵 타입 기준으로 맵 모드 선택
		MAP_TYPE myMap = g_players[g_myid].select_map;

		if (myMap == MAP_TYPE::STRAIGHT)
		{
			MM.SetMode(std::make_unique<Map1_Mode>());
		}
		else if (myMap == MAP_TYPE::RECTANGLE)
		{
			MM.SetMode(std::make_unique<Map2_Mode>());
		}
	}
	break;
	case S2C_MOVE:
	{
		S2C_Move_Packet* p = reinterpret_cast<S2C_Move_Packet*>(buf);


		for (int i = 0; i < MAX_USER; ++i) {
			g_players[i].m_speed = p->arr[i].speed;
			g_players[i].m_yaw = p->arr[i].yaw;
			g_players[i].m_face_rotation = p->arr[i].face_rotation;
			g_players[i].m_body_rotation = p->arr[i].body_rotation;
			g_players[i].m_booster_head_tilt = p->arr[i].booster_head_tilt;
			g_players[i].x = p->arr[i].x;
			g_players[i].y = p->arr[i].y;
			g_players[i].z = p->arr[i].z;
			g_players[i].isBoosterOn = p->arr[i].boosterOn;
		}

	}
	break;
	case S2C_BOOSTER:
	{
		S2C_Booster_Packet* p = reinterpret_cast<S2C_Booster_Packet*>(buf);
		g_players[g_myid].isBoosterOn = p->boosterOn;
		g_players[g_myid].m_booster_cnt = p->booster_cnt;
	}
	break;
	case S2C_RANK:
	{
		S2C_Rank_Packet* p = reinterpret_cast<S2C_Rank_Packet*>(buf);
		g_players[g_myid].m_rank = p->rank;
		g_delta_time = p->finish_time;
		g_players[g_myid].m_finish_time = g_delta_time;
		g_GameEnd = true;
	}
	break;
	case S2C_LEAVE_GAME:
	{
		std::cout << "Leave" << std::endl;
		reset();
		MM.SetMode(std::make_unique<RoomMode>());
		return;
	}
	break;
	case S2C_LOGOUT:
		break;
	default:
		std::cout << "찾을 수 없는 패킷 : " << type << std::endl;
		break;
	}
}