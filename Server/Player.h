#pragma onc
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
	int id;
	char name[NAME_SIZE];
	short x, y, z;
	bool isReady;
	SOCKET socket;

public:
	Player() : id(-1), name("0"), x(0), y(0), z(0), isReady(false),socket(INVALID_SOCKET) {};
	~Player();


	bool recv_packet();
	void send_packet(void* packet);
	void process_packet(unsigned char* p);
	void disconnect();
};

