#include "Pch.h"
#include "NetworkMgr.h"

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
