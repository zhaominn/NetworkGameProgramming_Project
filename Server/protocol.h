#pragma once

// global definition
constexpr short NAME_SIZE = 256;
constexpr short BUF_SIZE = 4096;
constexpr short SERVER_PORT = 9000;

constexpr float TIME_PER_ACTION = 0.25f;
constexpr float ACTION_PER_TIME = 1.0f / TIME_PER_ACTION;

constexpr float PLAYER_SPEED = 1.0f;

constexpr float ACCELERATION = 0.004f;
constexpr float DECELERATION = 0.003f;
constexpr float LIMIT_SPEED = 1.0;
constexpr float BOOSTER_SPEED = 2.0;
constexpr float MAX_SPEED = 1.0;
constexpr float TURN_ANGLE = 1.0;

constexpr float MAX_FACE_ROTATION = 25.0f;
constexpr float ROTATION_SPEED = 5.0f;
constexpr float RETURN_SPEED = 2.0f;

constexpr float booster_head_tilt = 0.0f;
constexpr float MAX_HEAD_TILT = 20.0f;
constexpr float TILT_SPEED = 2.0f;

constexpr unsigned short WINDOW_WIDTH = 980;
constexpr unsigned short WINDOW_HEIGHT = 780;

constexpr int  MAX_USER = 3;

// packet
// C2S
constexpr char C2S_LOGIN = 0;
constexpr char C2S_IS_READY = 1;
constexpr char C2S_ENTER_ROOM = 2;
constexpr char C2S_MOVE = 3;
constexpr char C2S_BOOSTER = 4;
constexpr char C2S_LOGOUT = 5;
// S2C
constexpr char S2C_PLAYER_INFO = 6;
constexpr char S2C_LOGIN_FAIL = 7;
constexpr char S2C_ENTER_ROOM = 8;
constexpr char S2C_IS_READY = 9;
constexpr char S2C_GAME_START = 10;
constexpr char S2C_MOVE = 11;
constexpr char S2C_BOOSTER = 12;
// constexpr char S2C_WORLD_UPDATE = 13;
constexpr char S2C_RANK = 14;
constexpr char S2C_LOGOUT = 15;

// enum
enum MAP_TYPE { STRAIGHT, RECTANGLE };
enum DATA_TYPE { ACCEPT, SEND, RECV };
enum KEY_TYPE {
	UP, DOWN, LEFT, RIGHT, RELEASED,
	UP_RELEASED, DOWN_RELEASED, LEFT_RELEASED, RIGHT_RELEASED, 
};
enum GAME_STATE { LOBBY, ROOM, INGAME };

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

struct S2C_EnterRoom_Packet
{
	unsigned char size;
	char type;
	char id;
};

struct S2C_Ready_Packet
{
	unsigned char size;
	char type;
	char id;
	bool is_ready;
};

struct S2C_GameStart_Packet
{
	unsigned char size;
	char type;
};

struct S2C_Move_Packet
{
	unsigned char size;
	char type;
	char id;
	float speed;
	float yaw;
	float face_rotation;
	KEY_TYPE key;

	float x;
	float y;
	float z;

	//btRigidBody rigidBody;
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
	MAP_TYPE map;
};

struct C2S_Move_Packet
{
	unsigned char size;
	char type;
	KEY_TYPE key_type;
	//btRigidBody rigidBody;
};

#pragma pack (pop)