#pragma once

class Player
{
public:
	int id;
	char name[NAME_SIZE];
	short x, y;
	SOCKET socket;

public:
	Player();
	~Player();
};

