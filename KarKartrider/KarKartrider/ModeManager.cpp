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

	// 1. [핵심] 옛날 모드를 바로 죽이지 말고, 임시 변수로 옮겨서 '생명 연장'을 시킵니다.
		// 이렇게 하면 currentMode에 새 값이 들어갈 때 소멸자가 호출되지 않습니다.
	std::unique_ptr<Mode> oldMode = std::move(currentMode);

	// 2. 새 모드를 현재 모드 자리에 앉힙니다.
	currentMode = std::move(nextMode);

	// 3. [중요] 전역/정적 포인터(싱글톤 대용)를 '가장 먼저' 갱신합니다.
	// RoomMode::init()이 실행되기 전에 무조건 갱신되어 있어야 합니다.
	if (currentMode) {
		Mode::currentInstance = currentMode.get();
	}
	else {
		Mode::currentInstance = nullptr;
	}

	// 4. 새 모드를 초기화합니다.
	// 이때 Mode::currentInstance는 이미 RoomMode를 가리키고 있으므로 안전합니다.
	if (currentMode) {
		std::cout << "Initializing New Mode..." << std::endl;
		currentMode->init();
	}

	// 5. 모든 교체와 연결이 끝난 후, 옛날 모드(LoginMode)를 안전하게 파괴합니다.
	// 여기서 소멸자가 호출됩니다.
	oldMode.reset();

	needSwitch = false;
	std::cout << "Mode Switch Complete." << std::endl;
}
