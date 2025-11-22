#include "Pch.h"
#include "NetworkMgr.h"
#include "root.h"
#include "Map1_Mode.h"
#include "Map2_Mode.h"

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

void NetworkMgr::SendChangeReadyPacket(bool status)
{
	C2S_Change_Ready_Packet* change_ready_packet = new C2S_Change_Ready_Packet;
	change_ready_packet->size = sizeof(C2S_Change_Ready_Packet);
	change_ready_packet->type = C2S_IS_READY;
	change_ready_packet->is_ready = status;

	glm::vec3 pos = glm::vec3(0.0, 2.6, 238.0);
	change_ready_packet->x = pos.x;
	change_ready_packet->y = pos.y;
	change_ready_packet->z = pos.z;

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

void NetworkMgr::ProcessPacket(char* buf)
{
	unsigned char type = buf[1];
	switch (type) {
	case S2C_PLAYER_INFO: {
		std::cout << "로그인 성공!" << std::endl;

		S2C_PlayerInfo_Packet* playerinfo_packet = reinterpret_cast<S2C_PlayerInfo_Packet*>(buf);
		g_myid = playerinfo_packet->id;
		g_players[g_myid].m_id = g_myid;
	}
						break;
	case S2C_LOGIN_FAIL: {

		// 서버에서 보낸 로그인 패킷
		std::cout << "로그인 실패!" << std::endl;
	}
					   break;
	case S2C_ENTER_ROOM:
		break;
	case S2C_IS_READY:
		break;
	case S2C_GAME_START: {
		std::cout << "Game Start@@@@@@@@@@@@@@@@@@@@" << std::endl;

		Map1_Mode* map1Mode = new Map1_Mode();
		//map1Mode->goSelectMode = [this]() { goSelectMode(); };
		MM.SetMode(map1Mode);

	}
					   break;
	case S2C_MOVE:
	{
		S2C_Move_Packet* p = reinterpret_cast<S2C_Move_Packet*>(buf);


		for (int i = 0; i < MAX_USER; ++i) {

			std::cout << p->arr[i].z << std::endl;

			g_players[i].m_speed = p->arr[i].speed;
			g_players[i].m_yaw = p->arr[i].yaw;
			g_players[i].m_face_rotation = p->arr[i].fase_rotation;
			g_players[i].m_body_rotation = p->arr[i].body_rotation;
			g_players[i].x = p->arr[i].x;
			g_players[i].y = p->arr[i].y;
			g_players[i].z = p->arr[i].z;
		}

	}
				 break;
	case S2C_BOOSTER:
	{
		S2C_Booster_Packet* p = reinterpret_cast<S2C_Booster_Packet*>(buf);
		g_players[p->id].m_booster_head_tilt = p->booster_head_tilt;
		std::cout << "get boooster packet" << std::endl;
	}
		break;
	case S2C_RANK:
	{
		S2C_Rank_Packet* p = reinterpret_cast<S2C_Rank_Packet*>(buf);
		g_players[g_myid].m_rank = p->rank;
		g_delta_time = p->finish_time;
		g_players[g_myid].m_finish_time = g_delta_time;
	}
		break;
	case S2C_LOGOUT:
		break;
	default:
		std::cout << "찾을 수 없는 패킷 : " << (int)type << std::endl;
		break;
	}
}