#include "Pch.h"
#include "NetworkMgr.h"

std::thread m_recvThread;
std::atomic<bool> m_running{ false };

NetworkMgr::NetworkMgr()
{
	m_sock = -1;	// < 0 이면 초기화가 안된거임
}

NetworkMgr::~NetworkMgr()
{
	// 소켓 닫기
	closesocket(m_sock);

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

void NetworkMgr::SendPacket(char* packet, int size)
{
	send(m_sock, (char*)&size, sizeof(int), 0);
	send(m_sock, packet, size, 0);
}

void NetworkMgr::SendSelectMapPacket(MAP_TYPE map)
{
	C2S_Enter_Room_Packet* enter_room_packet = new C2S_Enter_Room_Packet;
	enter_room_packet->size = sizeof(C2S_Enter_Room_Packet);
	enter_room_packet->type = C2S_ENTER_ROOM;
	enter_room_packet->map = map;
	SendPacket(reinterpret_cast<char*>(enter_room_packet), sizeof(enter_room_packet));
}

void NetworkMgr::ProcessPacket(char* buf)
{
	unsigned char type = buf[1];
	switch (type) {
	case S2C_PLAYER_INFO: {
		std::cout << "로그인 성공!" << std::endl;
		S2C_PlayerInfo_Packet* playerinfo_packet = reinterpret_cast<S2C_PlayerInfo_Packet*>(buf);
		m_id = playerinfo_packet->id;
	}
						break;
	case S2C_MOVE: {

		// 서버에서 보낸 MOVE 패킷 처리
	}
				 break;
	case S2C_LOGIN_FAIL: {

		// 서버에서 보낸 로그인 패킷
		std::cout << "로그인 실패!" << std::endl;
	}
					   break;
	default:
		std::cout << "찾을 수 없는 패킷 : " << (int)type << std::endl;
		break;
	}
}