#include "Pch.h"
#include "root.h"

std::vector<Model*> karts;
std::vector<Model*> road1;
std::vector<Model*> road1_barricate;
std::vector<Model*> road2;
std::vector<Model*> road2_barricate;
std::vector<Model*> selectMaps;
std::vector<Model*> character;
std::vector<Model*> countDown;
std::vector<Model*> pause;
std::vector<Model*> booster_uis;
std::vector<Model*> dashBoards;

ModeManager MM;

int window_width = 980;
int window_height = 780;
int location_x = 0;
int location_y = 0;
float volume = 0.5f;

bool isGameRunning = false;
bool isGameRunning2 = false;
bool bb_status = false;


GLvoid Reshape(int w, int h) {
    glViewport(location_x, location_y, w, h);
    width = w;
    height = h;
}