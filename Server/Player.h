#pragma once
#include "Room.h"

class Player
{
private:
	int m_id;
	int m_booster_cnt;
	char* m_name;
	float m_speed;
	float m_yaw;
	float m_face_rotation;
	float m_body_rotation;
	float m_booster_head_tilt;

	bool isReady;
	bool isOnline;
	bool isBoosterActive;

	SOCKET m_socket;

	std::chrono::steady_clock::time_point m_boosterEndTime;

public:
	bool m_up = false;
	bool m_down = false;
	bool m_left = false;
	bool m_right = false;
	bool m_release = false;

	// player position
	float m_posX = 0;
	float m_posY = 0;
	float m_posZ = 0;

	// collider size (player)
	float m_colliderHalfX = 0.933135f;
	float m_colliderHalfY = 0.533785f;
	float m_colliderHalfZ = 1.526595f;

	float collisionCH = false;
	bool isFinished;

	AABB g_Map1Colliders[5]{};
	AABB g_Map2Colliders[18]{};

	MAP_TYPE select_map = STRAIGHT; // default
public:
	Player() : m_id(-1), m_name(), m_booster_cnt(2), m_yaw(0), m_speed(0),
		m_face_rotation(0), m_booster_head_tilt(0), m_body_rotation(0),
		isBoosterActive(false), isReady(false), isOnline(false), isFinished(false), m_socket(INVALID_SOCKET) {
	};
	~Player();

	bool recv_packet();
	void send_packet(char* packet, int len);
	void broadcast(char* packet, int len);
	void process_packet(char* p);
	void disconnect();

	void send_Player_Info_Packet();
	void send_Login_Fail_Packet();
	void send_Enter_Room_Packet(MAP_TYPE map);
	void send_Leave_Room_Packet(int roomIdx, int leaverID);
	void send_Ready_Packet();
	void send_Game_Start_Packet(Room rooms[2]);
	void send_move_Packet();
	void send_booster_packet();

	void ActiveBooster();
	void CheckBoosterState();

	void CheckCollision();
	bool PlayerCollisionCheck(int a, int b, float& pushX, float& pushZ);

	// ingame
	void send_Rank_Packet();
	void checkIsFinished();

private:
	void reset();


public:
	void SetSocket(SOCKET socket);
	void SetId(short id);
	void SetName(const char* name);
	void SetBoosterCnt(int booster);
	void SetBoosterStatus(bool booster_status);
	void SetYaw(float yaw);
	void SetSpeed(float speed);
	void SetFaceRotation(float f_rotation);
	void SetBodyRotation(float b_rotation);
	void SetHeadTilt(float headtilt);
	void SetIsReady(bool ready);
	void SetOnline(bool online);

	SOCKET GetSocket() const { return m_socket; }
	int GetID() const;
	int GetBoosterCnt() const;
	bool GetBoosterStatus() const;
	char* GetName() const;
	float GetYaw() const;
	float GetSpeed() const;
	float GetFaceRotation() const;
	float GetBodyRotation() const;
	float GetHeadtilt() const;
	bool GetReady() const;
	bool GetOnline() const;
};

