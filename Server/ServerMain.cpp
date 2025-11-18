#include "Player.h"
#include <iostream>

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


DWORD WINAPI UpdatePositon(LPVOID lpParam)
{
	S2C_Move_Packet* scpacket = new S2C_Move_Packet;
	scpacket->size = sizeof(S2C_Move_Packet);
	scpacket->type = S2C_MOVE;

	// Collect the state of all players and send it to every player
	std::chrono::steady_clock::time_point last_send_time = std::chrono::steady_clock::now();
	while (true)
	{
		if (!g_GameStart) {
			Sleep(1);
			continue;
		}

		auto current_time = std::chrono::steady_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_send_time).count();

		if (elapsed_time >= 1000 / 33) {
			for (int i = 0; i < MAX_USER; ++i) {

				// update
				{
					std::lock_guard<std::mutex> lock1(g_UserMutex);

					float speed = g_users[i].GetSpeed();
					float turnSpeed = 0.0f;

					if (g_users[i].m_up)
					{
						speed += ACCELERATION;

						float f = g_users[i].GetFaceRotation();

						if (f > 0) {
							f -= RETURN_SPEED;
							if (f < 0) f = 0;
						}
						else if (f < 0) {
							f += RETURN_SPEED;
							if (f > 0) f = 0;
						}

						g_users[i].SetFaceRotation(f);
					}

					if (g_users[i].m_down)
					{
						speed -= ACCELERATION;

						float f = g_users[i].GetFaceRotation();

						if (f > 0) {
							f -= RETURN_SPEED;
							if (f < 0) f = 0;
						}
						else if (f < 0) {
							f += RETURN_SPEED;
							if (f > 0) f = 0;
						}

						g_users[i].SetFaceRotation(f);
					}

					if (g_users[i].m_left)
					{
						turnSpeed = TURN_ANGLE;
						float f = g_users[i].GetFaceRotation();
						f -= RETURN_SPEED;
						g_users[i].SetFaceRotation(f);
					}

					if (g_users[i].m_right)
					{
						turnSpeed = -TURN_ANGLE;
						float f = g_users[i].GetFaceRotation();
						f += RETURN_SPEED;
						g_users[i].SetFaceRotation(f);
					}

					if (g_users[i].m_release)
					{
						if (speed > 0.0f) {
							speed -= DECELERATION;
							if (speed < 0.0f) speed = 0.0f;
						}
						else if (speed < 0.0f) {
							speed += DECELERATION;
							if (speed > 0.0f) speed = 0.0f;
						}

						turnSpeed = 0.0f;

						float f = g_users[i].GetFaceRotation();

						if (f > 0) {
							f -= RETURN_SPEED;
							if (f < 0) f = 0;
						}
						else if (f < 0) {
							f += RETURN_SPEED;
							if (f > 0) f = 0;
						}

						g_users[i].SetFaceRotation(f);
					}


					if (speed > MAX_SPEED)speed = MAX_SPEED;
					if (speed < -MAX_SPEED / 2.0f) speed = -MAX_SPEED / 2.0f;

					g_users[i].SetSpeed(speed);
					g_users[i].SetYaw(turnSpeed);

					float f = g_users[i].GetFaceRotation();
					if (f > MAX_FACE_ROTATION) f = MAX_FACE_ROTATION;
					if (f < -MAX_FACE_ROTATION) f = -MAX_FACE_ROTATION;
					g_users[i].SetFaceRotation(f);

					if (g_users[i].GetOnline()) {
						scpacket->id = g_users[i].GetID();
						scpacket->speed = g_users[i].GetSpeed();
						scpacket->yaw = g_users[i].GetYaw();
						scpacket->key = g_users[i].GetKey();
						scpacket->face_rotation = g_users[i].GetFaceRotation();
						scpacket->x = g_users[i].m_posX;
						scpacket->y = g_users[i].m_posY;
						scpacket->z = g_users[i].m_posZ;
					}
				}

				// send
				if (g_users[i].GetOnline()) {
					std::lock_guard<std::mutex> lock2(g_Sendmutex);
					g_users[i].send_packet(reinterpret_cast<char*>(scpacket), sizeof(S2C_Move_Packet));
				}


			}
			last_send_time = current_time;
		}
	}
}


//DWORD WINAPI UpdatePositon(LPVOID lpParam)
//{
//	S2C_Move_Packet scpacket{};
//	scpacket.size = sizeof(S2C_Move_Packet);
//	scpacket.type = S2C_MOVE;
//
//
//	auto last = std::chrono::steady_clock::now();
//	auto startTime = std::chrono::steady_clock::now();
//	bool isFirstFrame = true;
//
//	while (true)
//	{
//
//		if (!g_GameStart) {
//			std::this_thread::sleep_for(std::chrono::milliseconds(1));
//			isFirstFrame = true;
//			continue;
//		}
//
//		auto now = std::chrono::steady_clock::now();
//
//		if (isFirstFrame) {
//			last = now;
//			startTime = now;
//			g_ElapsedTime = 0.0f;
//			g_rankCnt = 1;
//			isFirstFrame = false;
//			std::cout << "=== Game Timer Started ===" << std::endl;
//		}
//
//		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
//
//		if (ms >= (1000 / 33))
//		{
//			float dt = ms / 1000.0f;
//			last = now;
//
//			std::chrono::duration<float> elapsed = now - startTime;
//			g_ElapsedTime = elapsed.count();
//
//
//			for (int i = 0; i < MAX_USER; i++)
//			{
//				if (!g_users[i].GetOnline()) continue;
//
//				{
//					std::lock_guard<std::mutex> lock(g_UserMutex);
//
//					Player& pl = g_users[i];
//
//					// =================================================
//					// 1) 속도 갱신: W 누르면 앞으로 전진, 안 누르면 감속
//					// =================================================
//					float speed = pl.GetSpeed();
//					if (pl.m_up) speed += ACCELERATION;   // 가속
//					else speed *= 0.90f;          // 자동 감속
//					pl.SetSpeed(speed);
//
//					// =================================================
//					// 2) z축으로만 전진(회전 무시)
//					// =================================================
//					float oldZ = pl.m_posZ;
//					pl.m_posZ -= speed * dt;   // ★ -Z 방향이 앞으로 가는 방향
//
//					// ===== 테스트 출력 =====
//					std::cout << "[Server Move]"
//						<< "  id=" << pl.GetID()
//						<< "  speed=" << speed
//						<< "  dt=" << dt
//						<< "  oldZ=" << oldZ
//						<< "  newZ=" << pl.m_posZ
//						<< std::endl;
//
//					// =================================================
//					// 3) 충돌 체크
//					// =================================================
//					/*if (PlayerCollisionCheck(i))
//						pl.m_posZ = oldZ;*/
//
//					// =================================================
//					// 4) 패킷 작성
//					// =================================================
//					scpacket.id = pl.GetID();
//					scpacket.speed = pl.GetSpeed();
//					scpacket.yaw = 0; // 테스트라 yaw 없음
//					scpacket.key = pl.GetKey();
//					scpacket.face_rotation = pl.GetFaceRotation();
//					scpacket.x = pl.m_posX;
//					scpacket.y = pl.m_posY;
//					scpacket.z = pl.m_posZ;
//				}
//
//				// 게임 종료했는지 확인
//				g_users[i].checkIsFinished();
//
//				// =================================================
//				// 5) 패킷 전송
//				// =================================================
//				{
//					std::lock_guard<std::mutex> lock(g_Sendmutex);
//					g_users[i].send_packet(reinterpret_cast<char*>(&scpacket), sizeof(scpacket));
//				}
//			}
//		}
//	}
//}

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