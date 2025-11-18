#pragma once
#include "Room.h"

class btRigidBody;

class Player
{
private:
	int m_id;
	char* m_name;
	float m_speed;
	float m_yaw;
	float m_face_rotation;
	KEY_TYPE m_key;
	bool isReady;
	bool isOnline;
	SOCKET m_socket;
	btRigidBody* rigidBody = nullptr;

public:
	Player() : m_id(-1), m_name(), m_yaw(0), m_speed(0), m_face_rotation(0),
		m_key(RELEASED), isReady(false), isOnline(false), m_socket(INVALID_SOCKET) {};
	~Player();

	bool recv_packet();
	void send_packet(char* packet, int len);
	void process_packet(char* p);
	void disconnect();
	void send_Game_Start_Packet();

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
	void SetIsReady(bool ready);
	void SetOnline(bool online);

	SOCKET GetSocket() const { return m_socket; }
	int GetID() const;
	char* GetName() const;
	float GetYaw() const;
	KEY_TYPE GetKey() const;
	float GetSpeed() const;
	float GetFaceRotation() const;
	bool GetReady() const;
	bool GetOnline() const;
};

