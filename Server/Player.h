#pragma once
#define _CRT_SECURE_NO_WARNINGS 
//
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment (lib,"WS2_32.LIB")
#pragma comment (lib, "MSWSock.LIB")
#include "protocol.h"
#include <iostream>
//

class Player
{
public:
	int m_id;
	char* m_name;
	float m_x, m_y, m_z;
	bool isReady;
	bool isOnline;
	SOCKET m_socket;

public:
	Player() : m_id(-1), m_name(), m_x(0), m_y(0), m_z(0), isReady(false), isOnline(false), m_socket(INVALID_SOCKET) {};
	~Player();

	void recv_packet();
	void send_packet(char* packet, int len);
	void process_packet(char* p);
	void disconnect();

	void SetSocket(SOCKET socket);
	void SetId(short id);
	void SetName(const char* name);
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void SetIsReady(bool ready);
	void SetOnline(bool online);

	SOCKET GetSocket() const { return m_socket; }
	int GetID() const;
	char* GetName() const;
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	bool GetReady() const;
	bool GetOnline() const;

	void send_Game_Start_Packet();


};

