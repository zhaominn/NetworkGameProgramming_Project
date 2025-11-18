#include "Player.h"
#include <iostream>

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

bool PlayerCollisionCheck(int myId)
{
	// 내 위치/크기 가져오기
	float x1 = g_users[myId].m_posX;
	float z1 = g_users[myId].m_posZ;

	float halfX1 = g_users[myId].m_colliderHalfX;
	float halfZ1 = g_users[myId].m_colliderHalfZ;

	// 다른 플레이어와 비교
	for (int i = 0; i < MAX_USER; i++)
	{
		if (i == myId) continue;                  // 자기 자신 제외
		if (!g_users[i].GetOnline()) continue;    // 접속 안한 플레이어 제외

		float x2 = g_users[i].m_posX;
		float z2 = g_users[i].m_posZ;

		float halfX2 = g_users[i].m_colliderHalfX;
		float halfZ2 = g_users[i].m_colliderHalfZ;

		// AABB 충돌 체크
		bool overlapX = fabs(x1 - x2) < (halfX1 + halfX2);
		bool overlapZ = fabs(z1 - z2) < (halfZ1 + halfZ2);

		if (overlapX && overlapZ)
		{
			return true; // 충돌 발생
		}
	}

	return false; // 충돌 없음
}

//DWORD WINAPI UpdatePositon(LPVOID lpParam)
//{
//	S2C_Move_Packet scpacket{};
//	scpacket.size = sizeof(S2C_Move_Packet);
//	scpacket.type = S2C_MOVE;
//
//	auto last_send_time = std::chrono::steady_clock::now();
//
//	while (true)
//	{
//		if (!g_GameStart) {
//			Sleep(1);
//			continue;
//		}
//
//		auto now = std::chrono::steady_clock::now();
//		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_send_time).count();
//
//		// 약 33fps 주기
//		if (elapsed_ms >= (1000 / 33))
//		{
//			float deltaTime = elapsed_ms / 1000.0f;
//			last_send_time = now;
//
//			for (int i = 0; i < MAX_USER; i++)
//			{
//				if (!g_users[i].GetOnline())
//					continue;
//
//				{
//					std::lock_guard<std::mutex> lock(g_UserMutex);
//
//					Player& pl = g_users[i];
//
//					// ----------- 1) 입력 기반 속도/회전 갱신 ----------------
//					float speed = pl.GetSpeed();
//					float yaw = pl.GetYaw();      // 도 단위
//					float turn = 0.0f;             // yaw 변화량 (도/s)
//
//					// 전진/후진 입력
//					if (pl.m_up)
//						speed += ACCELERATION * deltaTime;
//					if (pl.m_down)
//						speed -= ACCELERATION * deltaTime;
//
//					// 회전 입력
//					if (pl.m_left)
//						turn += TURN_ANGLE;
//					if (pl.m_right)
//						turn -= TURN_ANGLE;
//
//					// 감속 처리
//					if (!pl.m_up && !pl.m_down)
//					{
//						if (speed > 0.0f) {
//							speed -= DECELERATION * deltaTime;
//							if (speed < 0.0f) speed = 0.0f;
//						}
//						else if (speed < 0.0f) {
//							speed += DECELERATION * deltaTime;
//							if (speed > 0.0f) speed = 0.0f;
//						}
//					}
//
//					// 속도 제한
//					if (speed > MAX_SPEED) speed = MAX_SPEED;
//					if (speed < -MAX_SPEED / 2.0f) speed = -MAX_SPEED / 2.0f;
//
//					// yaw 회전 적용
//					yaw += turn * deltaTime;
//
//					// yaw 정규화
//					if (yaw > 180.f)  yaw -= 360.f;
//					if (yaw < -180.f) yaw += 360.f;
//
//					pl.SetSpeed(speed);
//					pl.SetYaw(yaw);
//
//					// ----------- 2) XYZ 위치 갱신 ----------------
//
//					float oldX = pl.m_posX;
//					float oldZ = pl.m_posZ;
//
//					// yaw → 라디안
//					float yawRad = yaw * (3.141592f / 180.0f);
//
//					// 네 클라 모델 기준: 카트의 전진 방향은 -Z
//					float dirX = -sinf(yawRad);
//					float dirZ = -cosf(yawRad);
//
//					pl.m_posX += dirX * speed * deltaTime;
//					pl.m_posZ += dirZ * speed * deltaTime;
//					// pl.m_posY 서버에서는 고정 유지 (필요하면 수정)
//
//					// ----------- 3) 플레이어 간 충돌 처리 -------------
//					if (PlayerCollisionCheck(i))
//					{
//						// 충돌 발생 → 롤백
//						pl.m_posX = oldX;
//						pl.m_posZ = oldZ;
//
//						// 충돌 시 감속 효과 (원하면)
//						pl.SetSpeed(speed * 0.5f);
//					}
//
//					// ----------- 4) 패킷 구성 -------------------------
//					scpacket.id = pl.GetID();
//					scpacket.speed = pl.GetSpeed();
//					scpacket.yaw = pl.GetYaw();
//					scpacket.key = pl.GetKey();
//					scpacket.face_rotation = pl.GetFaceRotation();
//
//					scpacket.x = pl.m_posX;
//					scpacket.y = pl.m_posY;
//					scpacket.z = pl.m_posZ;
//				}
//
//				// ----------- 5) 패킷 전송 -------------------------
//				{
//					std::lock_guard<std::mutex> lock(g_Sendmutex);
//					g_users[i].send_packet(reinterpret_cast<char*>(&scpacket), sizeof(scpacket));
//				}
//			}
//		}
//	}
//}


DWORD WINAPI UpdatePositon(LPVOID lpParam)
{
	S2C_Move_Packet scpacket{};
	scpacket.size = sizeof(S2C_Move_Packet);
	scpacket.type = S2C_MOVE;

	auto last = std::chrono::steady_clock::now();

	while (true)
	{
		if (!g_GameStart) {
			Sleep(1);
			continue;
		}

		auto now = std::chrono::steady_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();

		if (ms >= 1000 / 33)
		{
			float dt = ms / 1000.0f;
			last = now;

			for (int i = 0; i < MAX_USER; i++)
			{
				if (!g_users[i].GetOnline()) continue;

				{
					std::lock_guard<std::mutex> lock(g_UserMutex);

					Player& pl = g_users[i];

					// ===== 1) X축으로만 이동하는 테스트 =====
					float speed = pl.GetSpeed();

					if (pl.m_up)
						speed += ACCELERATION;   // 누르면 가속
					else
						speed *= 0.90f;          // 안 누르면 자동 감속

					pl.SetSpeed(speed);

					float oldX = pl.m_posX;

					// ===== 2) X축으로만 이동! =====
					pl.m_posX += speed * dt;

					// ===== 3) 충돌 체크 (옵션) =====
					if (PlayerCollisionCheck(i))
						pl.m_posX = oldX;

					// ===== 4) 패킷 채우기 =====
					scpacket.id = pl.GetID();
					scpacket.speed = pl.GetSpeed();
					scpacket.yaw = 0; // 테스트라서 회전 X
					scpacket.key = pl.GetKey();
					scpacket.face_rotation = pl.GetFaceRotation();

					scpacket.x = pl.m_posX;
					scpacket.y = pl.m_posY;
					scpacket.z = pl.m_posZ;
				}

				// ===== 5) 패킷 전송 =====
				{
					std::lock_guard<std::mutex> lock(g_Sendmutex);
					g_users[i].send_packet(reinterpret_cast<char*>(&scpacket), sizeof(scpacket));
				}
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