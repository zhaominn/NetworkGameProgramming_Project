#include "Player.h"
#include <iostream>


bool PlayerCollisionCheck(int a, int b, float& pushX, float& pushZ)
{
	float x1 = g_users[a].m_posX;
	float z1 = g_users[a].m_posZ;

	float x2 = g_users[b].m_posX;
	float z2 = g_users[b].m_posZ;

	float hx1 = g_users[a].m_colliderHalfX;
	float hz1 = g_users[a].m_colliderHalfZ;

	float hx2 = g_users[b].m_colliderHalfX;
	float hz2 = g_users[b].m_colliderHalfZ;

	float dx = x1 - x2;
	float dz = z1 - z2;

	float overlapX = (hx1 + hx2) - fabs(dx);
	float overlapZ = (hz1 + hz2) - fabs(dz);

	if (overlapX > 0 && overlapZ > 0)
	{
		if (overlapX < overlapZ)
		{
			pushX = (dx > 0 ? overlapX : -overlapX);
			pushZ = 0;
		}
		else
		{
			pushX = 0;
			pushZ = (dz > 0 ? overlapZ : -overlapZ);
		}
		return true;
	}

	return false;
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
		if (!g_GameStart) {
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
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (!g_users[i].GetOnline()) continue;

				float oldX = g_users[i].m_posX;
				float oldZ = g_users[i].m_posZ;

				float px = 0.f, pz = 0.f;

				for (int j = 0; j < MAX_USER; ++j)
				{
					if (i == j) continue;
					if (!g_users[j].GetOnline()) continue;

					float pushX, pushZ;
					if (PlayerCollisionCheck(i, j, pushX, pushZ))
					{
						px += pushX;
						pz += pushZ;
					}
				}

				if (px != 0 || pz != 0)
				{
					g_users[i].m_posX += px * 0.5f;
					g_users[i].m_posZ += pz * 0.5f;

					float nx = 0.f, nz = 0.f;
					if (px != 0) nx = (px > 0 ? 1.0f : -1.0f);
					if (pz != 0) nz = (pz > 0 ? 1.0f : -1.0f);

					float yaw = g_users[i].GetYaw();
					float speed = g_users[i].GetSpeed();

					// local speed vector
					float vx = cosf(yaw) * speed;
					float vz = sinf(yaw) * speed;

					float dot = vx * nx + vz * nz;

					float rvx = vx - 2 * dot * nx;
					float rvz = vz - 2 * dot * nz;

					const float bounce = 0.7f;

					rvx *= bounce;
					rvz *= bounce;

					float newSpeed = sqrtf(rvx * rvx + rvz * rvz);
					float newYaw = atan2f(rvz, rvx);

					g_users[i].SetSpeed(newSpeed);
					g_users[i].SetYaw(newYaw);
				}

				g_users[i].CheckBoosterState();
				g_users[i].send_move_Packet();
				g_users[i].checkIsFinished();
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

				g_users[new_player_id].SetId(new_player_id);
				g_usersNum++;

				hThread = CreateThread(NULL, 0, &ClientThread, (LPVOID)&g_users[new_player_id], 0, NULL);

				if (hThread != NULL) {
					CloseHandle(hThread);
				}

				if (g_usersNum == 1)
				//if (g_usersNum == MAX_USER)
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
				if (readyClient == 1) {
				//if (readyClient == MAX_USER) {
					std::cout << "게임에 입장합니다." << std::endl;
					for (int i = 0; i < MAX_USER; ++i) {
						g_users[i].send_Game_Start_Packet();
					}

					EnterCriticalSection(&g_CS);
					g_GameStart = true;
					LeaveCriticalSection(&g_CS);

					g_ElapsedTime = 0.f;
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