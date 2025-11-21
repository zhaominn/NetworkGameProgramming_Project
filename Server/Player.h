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
	bool isFinished;
	bool isBoosterActive;

	SOCKET m_socket;

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

public:
	Player() : m_id(-1), m_name(), m_booster_cnt(2), m_yaw(0), m_speed(0), m_face_rotation(0), m_booster_head_tilt(0),
		isBoosterActive(false), isReady(false), isOnline(false), isFinished(false), m_socket(INVALID_SOCKET) {
	};
	~Player();

	bool recv_packet();
	void send_packet(char* packet, int len);
	void process_packet(char* p);
	void disconnect();

	void send_Game_Start_Packet();
	void send_move_Packet();
	void send_booster_packet();

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
	char* GetName() const;
	float GetYaw() const;
	float GetSpeed() const;
	float GetFaceRotation() const;
	float GetBodyRotation() const;
	float GetHeadtilt() const;
	bool GetReady() const;
	bool GetOnline() const;
};

