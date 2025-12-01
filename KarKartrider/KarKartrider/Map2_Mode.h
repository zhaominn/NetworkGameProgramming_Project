#pragma once
#include <iostream>
#include <thread>

#include <glm/glm/gtc/quaternion.hpp>
#include <glm/glm/gtx/quaternion.hpp>
#include <functional> 


class Map2_Mode : public Mode {
public:

	std::function<void()> goSelectMode;

	glm::quat cameraRotationQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	float reducedRotationInfluence = 0.0f;

	GLfloat kart_speed = 0.0f;

	enum Move { NONE_M, UP, DOWN, LEFT, RIGHT, CTRL };
	float ACCELERATION = 0.004f;
	float DECELERATION = 0.003f;
	float LIMIT_SPEED = 1.0;
	float BOOSTER_SPEED = 2.0;
	float MAX_SPEED = 1.0;

	int finish_cnt = 0;

	int start_count;

	bool Pause;

	std::unordered_map<Move, bool> kart_keyState;

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool booster = true;
	bool ctrl = false;

	glm::vec3 cameraTargetPos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	glm::vec3 smoothedCarPos = glm::vec3(0.0f);
	glm::quat smoothedCarRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	float    smoothedSpeed = 0.0f;
	bool     bFirstCameraFrame = true;

	glm::vec3 g_kartRenderPos = glm::vec3(0.0, 0.0, 0.0);
	bool g_firstRenderFrame = true;

	float carPosLerp = 0.2f;
	float carRotLerp = 0.2f;
	float speedLerp = 0.3f;

	float yaw = -90.0f;
	float pitch = 0.0f;
	float TURN_ANGLE = 1.0f;

	//sound
	bool isBackgroundSound = false;
	std::thread backgroundSoundThread;
	bool isCountNSound = false;
	std::thread countNSoundThread;
	bool isCountGoSound = false;
	std::thread countGoSoundThread;
	bool isMotorSound = false;
	std::thread motorSoundThread;
	bool isCrashSound = false;
	std::thread crashSoundThread;
	bool isBoosterSound = false;
	std::thread boosterSoundThread;
	bool isWinSound = false;
	std::thread winSoundThread;
	bool isLoseSound = false;
	std::thread loseSoundThread;

	// ----- game ------
	int booster_cnt = 2;
	const int MAX_BOOSTER_CNT = 2;
	bool isBoosterRegenActive = true;
	std::thread boosterRegenThread;
	bool isBoosterActive = false;
	bool isGameOver = false;

	float character_face_rotation = 0.0f;
	const float MAX_FACE_ROTATION = 25.0f;
	const float ROTATION_SPEED = 5.0f;
	const float RETURN_SPEED = 2.0f;

	float booster_head_tilt = 0.0f;
	const float MAX_HEAD_TILT = 20.0f;
	const float TILT_SPEED = 2.0f;

	// 내 카트 위치
	glm::mat4 myKartMatrix = glm::mat4(1.0f);

	Map2_Mode();
	~Map2_Mode();

	void startBoosterRegen();

	void draw_dashBoard();

	void draw_speed();

	void draw_ui();

	void draw_timer(float deltaTime);

	void init() override;

	void playCountdown(int count);

	void updateCameraDirection();

	void setCamera();

	void goSelectMode_();

	void finish_game();

	void draw_finish_time(float deltaTime);

	void lose_game();

	void checkCollisionKart();

	void checkEngineSound();

	void RenderPlayer();

	void timer();

	void mouseClick(int button, int state, int x, int y) override;

	void passiveMotion(int x, int y) override {}

	void keyboard(unsigned char key, int x, int y) override;

	void activateBoosterSound();

	void specialKey(int key, int x, int y) override;

	void specialKeyUp(int key, int x, int y) override;

	void draw_model() override;

	void draw_bb() override;

	void finish() override;

	virtual ModeType GetModeType() const override {
		return ModeType::INGAME;
	}
private:

	void updatePhysics(float deltaTime);

	static void timerHelper(int value);

	void backgroundSound();

	void count_go();
	void count_n();
	void engine_sound();
	void crash_sound();
	void booster_sound();
	void win_sound();
	void lose_sound();
};