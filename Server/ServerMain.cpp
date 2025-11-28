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

		// 적어도 한 방이라도 시작되었는지 확인
		bool anyRoomStarted = false;
		for (int roomIdx = 0; roomIdx < 2; roomIdx++)
		{
			if (g_room[roomIdx].gameStart)
			{
				anyRoomStarted = true;
				break;
			}
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

		float dt = static_cast<float>(elapsed_time) / 1000.0f;

		if (elapsed_time >= (1000 / 60))
		{
			std::lock_guard<std::mutex> lock1(g_UserMutex);

			// check and update
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (!g_users[i].GetOnline()) continue;
				Player& p = g_users[i];

				int roomIdx = p.select_map;
				Room& room = g_room[roomIdx];

				if (!room.gameStart) continue;

				// ---- wall collision ----
				float wpx, wpz;
				ProcessWallCollision(p, wpx, wpz);

				if (wpx != 0 || wpz != 0)
				{
					p.m_posX += wpx;
					p.m_posZ += wpz;
					//ApplyBounceReflection(p, wpx, wpz, 0.0f);
					p.SetSpeed(max(0.0f, p.GetSpeed() - 0.005f));
				}

				// ---- player collision ----
				float ppx, ppz;
				ProcessPlayerCollision(i, ppx, ppz);

				if (ppx != 0 || ppz != 0)
				{
					p.m_posX += ppx * 0.5f;
					p.m_posZ += ppz * 0.5f;
					//ApplyBounceReflection(p, ppx, ppz, 0.7f);
				}

				p.CheckBoosterState();
				p.checkIsFinished();
			}

			// send
			for (int i = 0; i < MAX_USER; ++i)
			{
				g_users[i].send_move_Packet();
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

	while (true) {
		switch (g_game_state) {
		case LOBBY:
		{
			// Login
			// ------------------------------------------------------------------------------------------------------

			int new_player_id = -1;
			while (!g_AllPlayerLogin) {
				for (int i = 0; i < MAX_USER; ++i) {
					if (g_users[i].GetID() == -1) {
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

				BOOL optVal = TRUE;
				setsockopt(
					g_users[new_player_id].GetSocket(), 
					IPPROTO_TCP,
					TCP_NODELAY,
					(const char*)&optVal,
					sizeof(optVal)
				);

				g_users[new_player_id].SetId(new_player_id);
				g_usersNum++;

				hThread = CreateThread(NULL, 0, &ClientThread, (LPVOID)&g_users[new_player_id], 0, NULL);

				if (hThread != NULL) {
					CloseHandle(hThread);
				}

				if (g_usersNum >= MIN_PLAYERS_TO_START)
				{
					g_AllPlayerLogin = true;
					g_game_state = INGAME;
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
				for (int roomIdx = 0; roomIdx < 2; roomIdx++)
				{
					Room& room = g_room[roomIdx];

					// 이미 시작한 방은 패스
					if (room.gameStart)
						continue;

					// 방의 모든 유저가 레디인지 체크
					if (IsRoomReady(room))
					{
						std::cout << "Room " << roomIdx << " start!" << std::endl;

						// 그 방에 있는 플레이어들에게만 Start 패킷 보내기
						for (int i = 0; i < MAX_USER; i++)
						{
							Player* p = room.inRoomPlayers[i];
							if (p != nullptr)
							{
								p->send_Game_Start_Packet(room.mapType);
							}
						}

						room.gameStart = true;
						room.elapsedTime = 0.0f;
					}
				}
				break;
			}
			break;
		}
	}

	closesocket(listen_sock);
	WSACleanup();
	DeleteCriticalSection(&g_CS);
	return 0;
}