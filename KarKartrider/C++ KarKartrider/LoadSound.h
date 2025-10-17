#pragma once
#include <string>

// 선언만: 구현은 LoadSound.cpp로 이동
void play_sound2D(const std::string& name, const std::string& path, bool repeat, bool* isRunning);