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
				readyCount++;
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

	printf("[Player %d] ClientThread Start.\n", my_id);

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
	std::chrono::steady_clock::time_point last_send_time = std::chrono::steady_clock::now();
	auto startTime = std::chrono::steady_clock::now();

	while (true)
	{
		bool anyRoomStarted = false;
		for (int r = 0; r < 2; r++)
		{
			if (g_room[r].gameStart)
				anyRoomStarted = true;
		}

		if (!anyRoomStarted)
		{
			Sleep(1);
			continue;
		}

		auto current_time = std::chrono::steady_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
			current_time - last_send_time
		).count();

		g_ElapsedTime = std::chrono::duration<float>(current_time - startTime).count();

		if (elapsed_time >= (1000 / 60))
		{
			std::lock_guard<std::mutex> lock1(g_UserMutex);

			for (int roomIdx = 0; roomIdx < 2; roomIdx++)
			{
				Room& room = g_room[roomIdx];
				if (!room.gameStart) continue;

				for (int i = 0; i < MAX_USER; i++)
				{
					Player* p = room.inRoomPlayers[i];
					if (!p) continue;

					// ---- wall collision ----
					float wpx, wpz;
					ProcessWallCollision(*p, wpx, wpz);

					if (wpx != 0 || wpz != 0)
					{
						p->m_posX += wpx;
						p->m_posZ += wpz;
						p->SetSpeed(max(0.0f, p->GetSpeed() - 0.005f));
					}

					// ---- player collision ----
					float ppx, ppz;
					ProcessPlayerCollisionRoom(room, p->GetID(), ppx, ppz);

					if (ppx != 0 || ppz != 0)
					{
						p->m_posX += ppx * 0.5f;
						p->m_posZ += ppz * 0.5f;
					}

					p->CheckBoosterState();
					//p->checkIsFinished();
				}

				for (int i = 0; i < MAX_USER; i++)
				{
					Player* p = room.inRoomPlayers[i];
					if (!p) continue;

					p->send_move_Packet();
				}
			}

			last_send_time = current_time;
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
		for (int roomIdx = 0; roomIdx < 2; roomIdx++)
		{
			Room& room = g_room[roomIdx];

			if (!room.gameStart && IsRoomReady(room))
			{
				std::cout << "Room " << roomIdx << " start!" << std::endl;

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
		}

		Sleep(1); // CPU 너무 안 태우게 살짝 쉼
	}

	closesocket(listen_sock);
	WSACleanup();
	DeleteCriticalSection(&g_CS);
	return 0;
}