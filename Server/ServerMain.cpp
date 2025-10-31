#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa
#include "Player.h"
#include <array>

#include <process.h> // _beginthreadex

std::array<Player, MAX_USER> g_users;
enum game_state { READY, INGAME, END };

CRITICAL_SECTION g_CS;
float g_ElapsedTime;

int main()
{
	InitializeCriticalSection(&g_CS);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) std::cout << "listen socket error" << std::endl;

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	if (bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
		std::cout << "bind error" << std::endl;

	if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR)
		std::cout << "listen error" << std::endl;

	while (1) {
		struct sockaddr_in clientaddr;
		int addrlen = sizeof(clientaddr);
		SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			std::cout << "client socket error" << std::endl;
			continue;
		}

		EnterCriticalSection(&g_CS);
		printf("\nenter client: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		LeaveCriticalSection(&g_CS);

		SOCKET* pClientSocket = (SOCKET*)malloc(sizeof(SOCKET));
		*pClientSocket = client_sock;

		/*HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ProcessClient, pClientSocket, 0, NULL);

		if (hThread != NULL) {
			CloseHandle(hThread);
		}*/
	}

	closesocket(listen_sock);
	WSACleanup();
	DeleteCriticalSection(&g_CS);
	return 0;
}