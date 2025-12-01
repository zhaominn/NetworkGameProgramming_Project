#pragma once

enum class ModeType {
	NONE,
	LOGO,
	LOGIN,
	SELECT_MAP,
	ROOM,
	INGAME,
};

class Mode {
public:
	static Mode* currentInstance;

	virtual void init() = 0;
	virtual void keyboard(unsigned char key, int x, int y) = 0;
	virtual void specialKey(int key, int x, int y) = 0;
	virtual void specialKeyUp(int key, int x, int y) = 0;
	virtual void mouseClick(int button, int state, int x, int y) = 0;
	virtual void passiveMotion(int x, int y) = 0;
	virtual void draw_model() = 0;
	virtual void draw_bb() = 0;
	virtual void finish() = 0;

	virtual ModeType GetModeType() const = 0;

	virtual ~Mode() {
		if (currentInstance == this) {
			currentInstance = nullptr;
		}
	}
};