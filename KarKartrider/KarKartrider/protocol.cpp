#pragma once

// global definition
constexpr short NAME_SIZE = 256;
constexpr short BUF_SIZE = 4096;
constexpr short SERVER_PORT = 9000;

constexpr float TIME_PER_ACTION = 0.25f;
constexpr float ACTION_PER_TIME = 1.0f / TIME_PER_ACTION;

constexpr float PLAYER_SPEED = 1.0f;
constexpr float BOOSTER_SPEED = 2.0f;

constexpr unsigned short WINDOW_WIDTH = 980;
constexpr unsigned short WINDOW_HEIGHT = 780;

constexpr int  MAX_USER = 3;

// packet
// C2S
constexpr char CS_LOGIN = 0;
constexpr char CS_IS_READY = 1;
constexpr char CS_MOVE = 2;
constexpr char CS_BOOSTER = 3;
constexpr char CS_LOGOUT = 4;
// S2C
constexpr char SC_PLAYER_INFO = 5;
constexpr char SC_LOGIN_FAIL = 5;
constexpr char SC_ENTER_ROOM = 5;
constexpr char SC_IS_READY = 5;
constexpr char SC_GAME_START = 5;
constexpr char SC_MOVE = 5;
constexpr char SC_BOOSTER = 5;
constexpr char SC_WORLD_UPDATE = 5;
constexpr char SC_RANK = 5;
constexpr char SC_LOGOUT = 5;

// enum
enum MAP_TYPE { STRAIGHT, RECTANGLE };
enum DATA_TYPE { ACCEPT, SEND, RECV };

#pragma pack (push, 1)

// S2C
struct S2C_PlayerInfo_Packet
{
	unsigned char size;
	char type;
	char id;
};

struct S2C_Login_Fail_Packet
{
	unsigned char size;
	char type;
};

struct S2C_Ready_Packet
{
	unsigned char size;
	char type;
	char id;
	bool is_ready;
};

struct S2C_Booster_Packet
{
	unsigned char size;
	char type;
	char id;
};

struct S2C_Rank_Packet
{
	unsigned char size;
	char type;
	char rank;
};

struct S2C_EnterRoom_Packet
{
	unsigned char size;
	char type;
	char id;
};

struct S2C_GameStart_Packet
{
	unsigned char size;
	char type;
};

class btRigidBody {}; // 나중에 수정..
struct S2C_Move_Packet
{
	unsigned char size;
	char type;
	char id;
	float x;
	float y;
	float z;
	btRigidBody rigidBody;
};

struct S2C_Finish_Packet
{
	unsigned char size;
	char type;
	char rank[3];
};

// C2S
struct C2S_Login_Packet
{
	unsigned char size;
	char type;
	char name[NAME_SIZE];
};

struct C2S_Change_Ready_Packet
{
	unsigned char size;
	char type;
	bool is_ready;
};

struct C2S__Booster_Packet
{
	unsigned char size;
	char type;
};

struct C2S_Change_Map_Packet
{
	unsigned char size;
	char type;
	char map_info;
};

struct C2S_Enter_Room_Packet
{
	unsigned char size;
	char type;
	int room_key;
};

struct C2S_Move_Packet
{
	unsigned char size;
	char type;
	float x;
	float y;
	float z;
	btRigidBody rigidBody;
};

#pragma pack (pop)