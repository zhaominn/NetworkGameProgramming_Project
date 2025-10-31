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
	send(m_sock, reinterpret_cast<char*>(&size), sizeof(int), 0);
	send(m_sock, packet, size, 0);
}

void NetworkMgr::RecvThread()
{
	char buf[512];
	while (m_running)
	{
		int ret = recv(m_sock, buf, sizeof(buf), 0);
		if (ret > 0) {
			ProcessPacket(buf, ret);
		}
		else if (ret == 0) {
			std::cout << "[서버 연결 종료]" << std::endl;
			m_running = false;
		}
		else {
			std::cout << "[recv error]" << std::endl;
			m_running = false;
		}
	}
	std::cout << "RecvThread 종료" << std::endl;
}

void NetworkMgr::ProcessPacket(char* buf, int len)
{
	unsigned char type = buf[0];
	switch (type) {
	case S2C_MOVE:
		// 서버에서 보낸 MOVE 패킷 처리
		break;
	case S2C_LOGIN_FAIL:
		// 서버에서 보낸 로그인 패킷
		break;
	default:
		std::cout << "찾을 수 없는 패킷 : " << (int)type << std::endl;
		break;
	}
}
