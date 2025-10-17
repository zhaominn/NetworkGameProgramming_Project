#include "LoadSound.h"
#include <iostream>
#include <irrKlang.h>
#include <thread>
#include <chrono>

using namespace irrklang;

void play_sound2D(const std::string& name, const std::string& path, bool repeat, bool* isRunning) {
    ISoundEngine* engine = createIrrKlangDevice();
    if (!engine) {
        std::cerr << "Error: Could not initialize irrKlang engine!" << std::endl;
        return;
    }

    std::string fullPath = path + name;
    std::cout << "Attempting to play sound: " << fullPath << std::endl;

    ISound* sound = engine->play2D(fullPath.c_str(), repeat, false, true);
    if (!sound) {
        std::cerr << "Error: Could not play sound file!" << std::endl;
        engine->drop();
        return;
    }

    sound->setVolume(1.0f);

    while (*isRunning) {
        if (!repeat && sound->isFinished()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!*isRunning) {
        sound->stop();
    }
    sound->drop();
    engine->drop();
}