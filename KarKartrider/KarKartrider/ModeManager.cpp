#include "Pch.h"
#include "ModeManager.h"

void ModeManager::SetMode(std::unique_ptr<Mode> m)
{
	std::cout << "set mode.." << std::endl;
	nextMode = std::move(m);
	needSwitch = true;
}

void ModeManager::draw_model()
{
	if (currentMode)
		currentMode->draw_model();

	ApplyModeChangeIfNeeded();
}

void ModeManager::draw_bb()
{
	if (currentMode)
		currentMode->draw_bb();
}

void ModeManager::finish()
{
	if (currentMode)
		currentMode->finish();

	ApplyModeChangeIfNeeded();
}
void ModeManager::keyboard(unsigned char key, int x, int y) {
	if (currentMode)
		currentMode->keyboard(key, x, y);

	ApplyModeChangeIfNeeded();
}

void ModeManager::specialKey(int key, int x, int y) {
	if (currentMode)
		currentMode->specialKey(key, x, y);

	ApplyModeChangeIfNeeded();
}

void ModeManager::specialKeyUp(int key, int x, int y) {
	if (currentMode)
		currentMode->specialKeyUp(key, x, y);

	ApplyModeChangeIfNeeded();
}

void ModeManager::mouseClick(int button, int state, int x, int y) {
	if (currentMode)
		currentMode->mouseClick(button, state, x, y);

	ApplyModeChangeIfNeeded();
}

void ModeManager::passiveMotion(int x, int y) {
	if (currentMode) {
		currentMode->passiveMotion(x, y);
	}

	ApplyModeChangeIfNeeded();
}

void ModeManager::ApplyModeChangeIfNeeded()
{
	if (!needSwitch)
		return;

	std::unique_ptr<Mode> oldMode = std::move(currentMode);

	currentMode = std::move(nextMode);

	if (currentMode) {
		Mode::currentInstance = currentMode.get();
	}
	else {
		Mode::currentInstance = nullptr;
	}

	if (currentMode) {
		std::cout << "Initializing New Mode..." << std::endl;
		currentMode->init();
	}

	oldMode.reset();

	needSwitch = false;
	std::cout << "Mode Switch Complete." << std::endl;
}
