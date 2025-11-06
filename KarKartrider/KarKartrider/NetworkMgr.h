#pragma once

#define _CRT_SECURE_NO_WARNINGS // ���� C �Լ� ��� �� ��� ���
#define _WINSOCK_DEPRECATED_NO_WARNINGS // ���� ���� API ��� �� ��� ���

#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���

#include <tchar.h> // _T(), ...
#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strncpy(), ...

#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ
#include "protocol.h"

class NetworkMgr
{
private:
	SOCKET m_sock;

public:
	NetworkMgr();
	~NetworkMgr();

	bool Init();

	SOCKET GetSocket() { return m_sock; }

public:
	void SendPacket(char* packet, int size);

public:
	void ProcessPacket(char* buf);
};

