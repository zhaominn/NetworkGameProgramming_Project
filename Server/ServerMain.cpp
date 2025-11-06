#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa
#include "Player.h"
#include <array>

#include <process.h> // _beginthreadex

std::array<Player, MAX_USER> g_users;
int g_ClientNum;
bool g_AllPlayerLogin = false;
bool g_GameStart = false;
bool g_GameEnd = false;
enum game_state { READY, INGAME, END };

CRITICAL_SECTION g_CS;
CRITICAL_SECTION g_CS_Send;
float g_ElapsedTime;

DWORD WINAPI ClientThread(void* pArguments)
{
	int player_id = *(int*)pArguments;
	free(pArguments);

	printf("[Player %d] ClientThread Start.\n",
		g_users[player_id].m_id);

	while (true) {
		if (false == g_users[player_id].recv_packet())
			break;
	}

	printf("[Thread %d] Thread End.\n", player_id);
	return 0;
}

DWORD WINAPI UpdatePositon(LPVOID lpParam)
{
	S2C_Move_Packet scpacket;
	scpacket.size = sizeof(S2C_Move_Packet);
	scpacket.type = S2C_MOVE;

	std::cout << "update" << std::endl;

	// Collect the state of all players and send it to every player
	while (true)
	{
		for (int from = 0; from < MAX_USER; ++from) {
			if (!g_users[from].GetOnline()) continue;

			scpacket.id = g_users[from].GetID();
			scpacket.x = g_users[from].GetX();
			scpacket.y = g_users[from].GetY();
			scpacket.z = g_users[from].GetZ();

			for (int to = 0; to < MAX_USER; ++to) {
				if (!g_users[to].GetOnline()) continue;

				EnterCriticalSection(&g_CS_Send);
				send(g_users[to].GetSocket(), (char*)&scpacket, scpacket.size, 0);
				LeaveCriticalSection(&g_CS_Send);
			}
		}

	}

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

	HANDLE SendThread;
	SendThread = CreateThread(NULL, 0, UpdatePositon, 0, 0, 0);
	if (SendThread == NULL) {
		std::cout << "closesocket()" << std::endl;
		closesocket(listen_sock);
	}
	else {
		CloseHandle(SendThread);
	}

	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	HANDLE hThread;

	// Login
	while (!g_AllPlayerLogin) {
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
			if (g_users[i].m_id == -1) {
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

		g_users[new_player_id].m_id = new_player_id;
		g_users[new_player_id].m_socket = client_sock;
		g_users[new_player_id].m_x = 0;
		g_users[new_player_id].m_y = 0;
		g_users[new_player_id].m_z = 0;

	/*	C2S_Login_Packet* login_packet = (C2S_Login_Packet*)recvBuf;
		g_users[new_player_id].SetName(login_packet->name);

		printf("\n[Main] Enter Client %s (ID: %d, %s:%d)\n",
			g_users[new_player_id].GetName(),
			new_player_id,
			inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port));*/

		LeaveCriticalSection(&g_CS);

		S2C_PlayerInfo_Packet info_packet;
		info_packet.size = sizeof(S2C_PlayerInfo_Packet);
		info_packet.type = S2C_PLAYER_INFO;
		info_packet.id = (char)new_player_id;

		int send_result = send(client_sock, (char*)&info_packet, info_packet.size, 0);

		if (send_result == SOCKET_ERROR) {
			printf("Player info packet send error: %d\n", WSAGetLastError());
			EnterCriticalSection(&g_CS);
			g_users[new_player_id].m_id = -1;
			g_users[new_player_id].m_socket = INVALID_SOCKET;
			LeaveCriticalSection(&g_CS);
			closesocket(client_sock);
			continue;
		}

		// create thread
		int* pPlayerID = (int*)malloc(sizeof(int));
		*pPlayerID = new_player_id;

		hThread = CreateThread(NULL, 0, &ClientThread, pPlayerID, 0, NULL);

		if (hThread != NULL) {
			CloseHandle(hThread);
		}
	}

	closesocket(listen_sock);
	WSACleanup();
	DeleteCriticalSection(&g_CS);
	return 0;
}