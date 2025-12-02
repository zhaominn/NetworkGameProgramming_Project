#include "Player.h"
#include "collision.h"

#include <iostream>

bool IsRoomReady(Room& room)
{
	int playerCount = 0;
	int readyCount = 0;

	for (int i = 0; i < MAX_USER; i++)
	{
		Player* p = room.inRoomPlayers[i];
		if (p != nullptr && p->GetID() != -1)
		{
			playerCount++;

			if (p->GetReady())
			{
				readyCount++;
			}

		}
	}

	// 플레이어가 최소 1명 이상 있어야 함
	if (playerCount == 0)
		return false;

	// 방에 있는 모든 플레이어가 READY면 true
	return (readyCount == playerCount);
}

DWORD WINAPI ClientThread(LPVOID socket)
{
	Player* player = reinterpret_cast<Player*>(socket);
	SOCKET client_socket = player->GetSocket();
	int my_id = player->GetID();

	printf("Player[%d] ClientThread Start.\n", my_id);

	while (true) {
		player->recv_packet();
		if (player->GetID() == -1)
			break;
	}

	g_users[my_id].disconnect();
	printf("[Thread %d] Thread End.\n", my_id);
	return 0;
}

DWORD WINAPI UpdatePosition(LPVOID lpParam)
{
	auto last_tick_time = std::chrono::steady_clock::now();

	while (true)
	{
		auto current_time = std::chrono::steady_clock::now();
		std::chrono::duration<float> delta_duration = current_time - last_tick_time;
		float dt = delta_duration.count();

		if (dt < (1.0f / 60.0f))
		{
			Sleep(1);
			continue;
		}

		last_tick_time = current_time;

		EnterCriticalSection(&g_CS);


		for (int roomIdx = 0; roomIdx < 2; roomIdx++)
		{
			Room& room = g_room[roomIdx];
			if (!room.gameStart) continue;

			room.elapsedTime += dt;
			g_ElapsedTime = room.elapsedTime;

			for (int i = 0; i < MAX_USER; i++)
			{
				Player* p = room.inRoomPlayers[i];
				if (!p) continue;
				if (!p->GetOnline()) continue;

				float wpx = 0, wpz = 0;
				ProcessWallCollision(*p, wpx, wpz);

				if (wpx != 0 || wpz != 0)
				{
					p->m_posX += wpx;
					p->m_posZ += wpz;
					p->SetSpeed(max(0.0f, p->GetSpeed() - 0.005f));
				}

				float ppx = 0, ppz = 0;
				ProcessPlayerCollisionRoom(room, p->GetID(), ppx, ppz);

				if (ppx != 0 || ppz != 0)
				{
					p->m_posX += ppx * 0.5f;
					p->m_posZ += ppz * 0.5f;
				}

				p->CheckBoosterState();
				// p->checkIsFinished(); // 필요 시 주석 해제
			}

			for (int i = 0; i < MAX_USER; i++)
			{
				Player* p = room.inRoomPlayers[i];
				if (!p) continue;
				p->send_move_Packet();
			}
		}
		LeaveCriticalSection(&g_CS);
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

	u_long on = 1;
	ioctlsocket(listen_sock, FIONBIO, &on);

	HANDLE SendThread;
	SendThread = CreateThread(NULL, 0, UpdatePosition, 0, 0, 0);
	if (SendThread == NULL) {
		std::cout << "closesocket()" << std::endl;
		closesocket(listen_sock);
	}
	else {
		CloseHandle(SendThread);
	}

	struct sockaddr_in clientaddr;
	int addrlen{};
	HANDLE hThread;

	while (true)
	{
		// 1) 넌블로킹 accept 시도
		addrlen = sizeof clientaddr;
		SOCKET clientSock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);


		if (clientSock != INVALID_SOCKET)
		{

			u_long off = 0;
			ioctlsocket(clientSock, FIONBIO, &off);

			EnterCriticalSection(&g_CS);

			int new_player_id = -1;

			for (int i = 0; i < MAX_USER; ++i) {
				if (g_users[i].GetID() == -1) {
					new_player_id = i;
					break;
				}
			}

			if (new_player_id != -1)
			{
				g_users[new_player_id].SetSocket(clientSock);
				g_users[new_player_id].SetId(new_player_id);
				g_usersNum++;

				CreateThread(NULL, 0, ClientThread, (LPVOID)&g_users[new_player_id], 0, NULL);
			}
			else
			{
				closesocket(clientSock); // 자리 없으면 거절
			}

			LeaveCriticalSection(&g_CS);
		}
		else
		{
			int err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
			{
				std::cout << "accept error: " << err << std::endl;
				// 필요하면 break; 로 서버 종료
			}
		}

		// 2) Room 별로 READY 검사 → 시작 여부 판단
		EnterCriticalSection(&g_CS);
		for (int roomIdx = 0; roomIdx < 2; roomIdx++)
		{
			Room& room = g_room[roomIdx];

			if (!room.gameStart && IsRoomReady(room))
			{
				std::cout << "Room " << roomIdx << "Game start!" << std::endl;

				for (int i = 0; i < MAX_USER; i++)
				{
					Player* p = room.inRoomPlayers[i];
					if (p != nullptr)
					{
						p->send_Game_Start_Packet(g_room);
					}
				}

				room.gameStart = true;
				room.elapsedTime = 0.0f;
			}
			else if (room.gameStart)
			{
				int readyCount = 0;
				int playerCount = 0;
				for (int i = 0; i < MAX_USER; ++i) {
					Player* p = room.inRoomPlayers[i];
					if (p != nullptr && p->GetID() != -1) {
						playerCount++;
						if (p->GetReady()) readyCount++;
					}
				}

				if (playerCount > 0 && readyCount == 0)
				{
					std::cout << "Room " << roomIdx << " Reset (Waiting for players)" << std::endl;
					room.gameStart = false;
					room.rank = 0;
				}
				else if (playerCount == 0)
				{
					room.gameStart = false;
					room.elapsedTime = 0.0f;
					room.rank = 0;
				}
			}
		}
		LeaveCriticalSection(&g_CS);
		Sleep(1); // CPU 너무 안 태우게 살짝 쉼
	}

	closesocket(listen_sock);
	WSACleanup();
	DeleteCriticalSection(&g_CS);
	return 0;
}