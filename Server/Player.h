#pragma once
#include "Room.h"

class Player
{
private:
	int m_id;
	char* m_name;
	float m_speed;
	float m_yaw;
	float m_face_rotation;
	float m_body_rotation;
	KEY_TYPE m_key;
	bool isReady;
	bool isOnline;
	bool isFinished;
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
	Player() : m_id(-1), m_name(), m_yaw(0), m_speed(0), m_face_rotation(0),
		m_key(RELEASED), isReady(false), isOnline(false), isFinished(false), m_socket(INVALID_SOCKET) {
	};
	~Player();

	bool recv_packet();
	void send_packet(char* packet, int len);
	void process_packet(char* p);
	void disconnect();

	void send_Game_Start_Packet();

	// ingame
	void send_Rank_Packet();
	void checkIsFinished();

private:
	void reset();


public:
	void SetSocket(SOCKET socket);
	void SetId(short id);
	void SetName(const char* name);
	void SetYaw(float yaw);
	void SetKey(KEY_TYPE key);
	void SetSpeed(float speed);
	void SetFaceRotation(float f_rotation);
	void SetBodyRotation(float b_rotation);
	void SetIsReady(bool ready);
	void SetOnline(bool online);

	SOCKET GetSocket() const { return m_socket; }
	int GetID() const;
	char* GetName() const;
	float GetYaw() const;
	KEY_TYPE GetKey() const;
	float GetSpeed() const;
	float GetFaceRotation() const;
	float GetBodyRotation() const;
	bool GetReady() const;
	bool GetOnline() const;
};

