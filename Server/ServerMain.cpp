#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa
#include "Player.h"
#include <array>

#include <process.h> // _beginthreadex

std::array<Player, MAX_USER> g_users;
enum game_state { READY, INGAME, END };

CRITICAL_SECTION g_CS;
float g_ElapsedTime;


unsigned int __stdcall ClientThread(void* pArguments) {
	int player_id = *(int*)pArguments;
	free(pArguments);

	SOCKET client_sock = g_users[player_id].socket;
	char player_name[NAME_SIZE];
	strcpy_s(player_name, g_users[player_id].name);

	printf("[Thread %d] Player %s (Socket: %llu) Thread Start.\n", player_id, player_name, client_sock);

	static char recvBuf[BUF_SIZE];
	int32_t len;

	while (true) {
		len = recv(client_sock, recvBuf, BUF_SIZE, 0);

		if (len <= 0) {
			printf("[Thread %d] Player %s Close\n", player_id, player_name);
			closesocket(client_sock);

			EnterCriticalSection(&g_CS);
			g_users[player_id].id = -1;
			g_users[player_id].socket = INVALID_SOCKET;
			strcpy_s(g_users[player_id].name, "0");
			LeaveCriticalSection(&g_CS);

			break;
		}

		
		// packet_handler(player_id, recvBuf);
		printf("[Thread %d] Packet received from Player %s (Type: %d, Size: %d)\n",
			player_id, player_name, recvBuf[1], recvBuf[0]);

	}

	return 0;
}

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
			std::cout << "accept error" << std::endl;
			continue;
		}

		static char recvBuf[BUF_SIZE];
		int32_t len = recv(client_sock, recvBuf, BUF_SIZE, 0);

		if (len <= 0) {
			printf("login packet receive fail. close socket.\n");
			closesocket(client_sock);
			continue;
		}

		unsigned char packet_type = recvBuf[1];

		if (packet_type != C2S_LOGIN) {

			S2C_Login_Fail_Packet fail_packet;
			fail_packet.size = sizeof(S2C_Login_Fail_Packet);
			fail_packet.type = S2C_LOGIN_FAIL;
			send(client_sock, (char*)&fail_packet, fail_packet.size, 0);

			closesocket(client_sock);
			continue;
		}

		int new_player_id = -1;
		EnterCriticalSection(&g_CS);
		for (int i = 0; i < MAX_USER; ++i) {
			if (g_users[i].id == -1) {
				new_player_id = i;
				break;
			}
		}

		if (new_player_id == -1) {
			LeaveCriticalSection(&g_CS);
			printf("Max User.\n");

			S2C_Login_Fail_Packet fail_packet;
			fail_packet.size = sizeof(S2C_Login_Fail_Packet);
			fail_packet.type = S2C_LOGIN_FAIL;
			send(client_sock, (char*)&fail_packet, fail_packet.size, 0);

			closesocket(client_sock);
			continue;
		}

		g_users[new_player_id].id = new_player_id;
		g_users[new_player_id].socket = client_sock;
		g_users[new_player_id].x = 0;
		g_users[new_player_id].y = 0;
		g_users[new_player_id].z = 0;

		C2S_Login_Packet* login_packet = (C2S_Login_Packet*)recvBuf;
		strcpy_s(g_users[new_player_id].name, login_packet->name);

		printf("\n[Main] Enter Client %s (ID: %d, %s:%d)\n",
			g_users[new_player_id].name,
			new_player_id,
			inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port));

		LeaveCriticalSection(&g_CS);

		S2C_PlayerInfo_Packet info_packet;
		info_packet.size = sizeof(S2C_PlayerInfo_Packet);
		info_packet.type = S2C_PLAYER_INFO;
		info_packet.id = (char)new_player_id;

		int send_result = send(client_sock, (char*)&info_packet, info_packet.size, 0);

		if (send_result == SOCKET_ERROR) {
			printf("Player info packet send error: %d\n", WSAGetLastError());
			EnterCriticalSection(&g_CS);
			g_users[new_player_id].id = -1;
			g_users[new_player_id].socket = INVALID_SOCKET;
			LeaveCriticalSection(&g_CS);
			closesocket(client_sock);
			continue;
		}

		// create thread
		int* pPlayerID = (int*)malloc(sizeof(int));
		*pPlayerID = new_player_id;

		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ClientThread, pPlayerID, 0, NULL);

		if (hThread != NULL) {
			CloseHandle(hThread);
		}
	}

	closesocket(listen_sock);
	WSACleanup();
	DeleteCriticalSection(&g_CS);
	return 0;
}