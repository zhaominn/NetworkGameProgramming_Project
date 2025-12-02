#pragma once

//#include "Mode.h"
#include "Mode.h"

class ModeManager {
public:
	//Mode* mode;

	std::unique_ptr<Mode> currentMode;
	std::unique_ptr<Mode> nextMode;
	bool needSwitch = false;

public:
	ModeManager() : currentMode(nullptr), nextMode(nullptr), needSwitch(false) {}

	void SetMode(std::unique_ptr<Mode> m);

	Mode* GetMode() {
		return currentMode.get();
	}

	~ModeManager(){}

	void draw_model();

	void draw_bb();

	void finish();

	void keyboard(unsigned char key, int x, int y);

	void specialKey(int key, int x, int y);

	void specialKeyUp(int key, int x, int y);

	void mouseClick(int button, int state, int x, int y);

	void passiveMotion(int x, int y);

private:
	void ApplyModeChangeIfNeeded();
};