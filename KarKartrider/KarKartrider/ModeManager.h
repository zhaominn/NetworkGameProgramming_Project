#pragma once

//#include "Mode.h"
#include "Mode.h"

class ModeManager {
private:
	Mode* mode;

public:
	ModeManager() : mode(nullptr) {}
	~ModeManager(){}
	void SetMode(Mode* m);

	Mode* GetMode() {
		return mode;
	}

	void draw_model();

	void draw_bb();

	void finish();

	void keyboard(unsigned char key, int x, int y);

	void specialKey(int key, int x, int y);

	void specialKeyUp(int key, int x, int y);

	void mouseClick(int button, int state, int x, int y);

	void passiveMotion(int x, int y);
};