#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa
#include "Player.h"
#include <array>

#include <process.h> // _beginthreadex

std::array<Player, MAX_USER> g_users;
int g_usersNum;
bool g_AllPlayerLogin = false;
bool g_AllPlayerReady = false;
bool g_GameStart = false;
bool g_GameEnd = false;
enum game_state { READY, INGAME, END };
game_state g_game_state = READY;

CRITICAL_SECTION g_CS;
CRITICAL_SECTION g_CS_Send;
float g_ElapsedTime;

DWORD WINAPI ClientThread(LPVOID socket)
{
	Player* player = reinterpret_cast<Player*>(socket);
	SOCKET client_socket = player->GetSocket();
	int my_id = player->GetID();

	printf("[Player %d] ClientThread Start.\n",
		g_users[player->GetID()].m_id);

	while (true) {
		player->recv_packet();
		if (player->GetID() == -1)
			break;
	}

	printf("[Thread %d] Thread End.\n", my_id);
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

	/*HANDLE SendThread;
	SendThread = CreateThread(NULL, 0, UpdatePositon, 0, 0, 0);
	if (SendThread == NULL) {
		std::cout << "closesocket()" << std::endl;
		closesocket(listen_sock);
	}
	else {
		CloseHandle(SendThread);
	}*/

	struct sockaddr_in clientaddr;
	int addrlen{};
	HANDLE hThread;

	while (true) {
		switch (g_game_state) {
		case READY:
		{
			// Login
			// ------------------------------------------------------------------------------------------------------

			int new_player_id = -1;
			while (!g_AllPlayerLogin) {
				for (int i = 0; i < MAX_USER; ++i) {
					if (g_users[i].m_id == -1) {
						new_player_id = i;
						break;
					}
				}

				addrlen = sizeof clientaddr;
				g_users[new_player_id].SetSocket(accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen));
				if (g_users[new_player_id].GetSocket() == INVALID_SOCKET) {
					std::cout << "accept error" << std::endl;
					continue;
				}

				g_users[new_player_id].SetId(new_player_id);
				g_usersNum++;

				hThread = CreateThread(NULL, 0, &ClientThread, (LPVOID)&g_users[new_player_id], 0, NULL);

				if (hThread != NULL) {
					CloseHandle(hThread);
				}

				if (g_usersNum == 3)
				{
					g_AllPlayerLogin = true; 
					/*temp*/g_game_state = INGAME;
				}
			}

			// Lobby
			// ------------------------------------------------------------------------------------------------------
			while (!g_AllPlayerReady) {
				break;
			}
		}
		break;
		case INGAME:
			// in game
			// ------------------------------------------------------------------------------------------------------
			while (!g_GameStart) {
				int readyClient = 0;

				if (g_GameStart) break;

				for (int i = 0; i < MAX_USER; ++i) {
					if (g_users[i].GetReady()) {
						readyClient++;
					}
				}
				if (readyClient == MAX_USER) {
					std::cout << "게임에 입장합니다." << std::endl;
					for (int i = 0; i < MAX_USER; ++i) {
						g_users[i].send_Game_Start_Packet();
					}

					EnterCriticalSection(&g_CS);
					g_GameStart = true;
					LeaveCriticalSection(&g_CS);
				}
			}
			break;
		}
	}

	closesocket(listen_sock);
	WSACleanup();
	DeleteCriticalSection(&g_CS);
	return 0;
}