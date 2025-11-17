#pragma once
#include "Room.h"

class Player
{
private:
	int m_id;
	char* m_name;
	float m_x, m_y, m_z;
	float m_speed;
	float m_yaw;
	KEY_TYPE m_key;
	bool isReady;
	bool isOnline;
	SOCKET m_socket;


public:
	Player() : m_id(-1), m_name(), m_x(0), m_y(0), m_z(0), m_yaw(0), m_speed(0), m_key(RELEASED),
		isReady(false), isOnline(false), m_socket(INVALID_SOCKET) {};
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
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void SetYaw(float yaw);
	void SetKey(KEY_TYPE key);
	void SetSpeed(float speed);
	void SetIsReady(bool ready);
	void SetOnline(bool online);

	SOCKET GetSocket() const { return m_socket; }
	int GetID() const;
	char* GetName() const;
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	float GetYaw() const;
	KEY_TYPE GetKey() const;
	float GetSpeed() const;
	bool GetReady() const;
	bool GetOnline() const;
};

