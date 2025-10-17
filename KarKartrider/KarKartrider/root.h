#pragma once

#include "shaderMaker.h"
#include "ModeManager.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "CharacterModel.h"
#include "Ui.h"

// Models
extern std::vector<Model*> karts;
extern std::vector<Model*> road1;
extern std::vector<Model*> road1_barricate;
extern std::vector<Model*> road2;
extern std::vector<Model*> road2_barricate;
extern std::vector<Model*> selectMaps;
extern std::vector<Model*> character;
extern std::vector<Model*> countDown;
extern std::vector<Model*> pause;
extern std::vector<Model*> booster_uis;
extern std::vector<Model*> dashBoards;

extern ModeManager MM;

extern int window_width;
extern int window_height;
extern int location_x;
extern int location_y;
extern float volume;

extern bool isGameRunning;
extern bool isGameRunning2;
extern bool bb_status;

GLvoid Reshape(int w, int h);