#include "Pch.h"
#include "ModeManager.h"

void ModeManager::SetMode(Mode* m)
{
	if (mode)
		mode->finish();
	mode = m;
	mode->init();
}

void ModeManager::draw_model()
{
	if (mode)
		mode->draw_model();
}

void ModeManager::draw_bb()
{
	if (mode)
		mode->draw_bb();
}

void ModeManager::finish()
{
	if (mode)
		mode->finish();
}
void ModeManager::keyboard(unsigned char key, int x, int y) {
	if (mode)
		mode->keyboard(key, x, y);
}

void ModeManager::specialKey(int key, int x, int y) {
	if (mode)
		mode->specialKey(key, x, y);
}

void ModeManager::specialKeyUp(int key, int x, int y) {
	if (mode)
		mode->specialKeyUp(key, x, y);
}

void ModeManager::mouseClick(int button, int state, int x, int y) {
	if (mode)
		mode->mouseClick(button, state, x, y);
}

void ModeManager::passiveMotion(int x, int y) {
	if (mode) {
		mode->passiveMotion(x, y);
	}
}