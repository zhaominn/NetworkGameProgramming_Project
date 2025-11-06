#include "Pch.h"
#include "LogoMode.h"
#include "LoadProgress.h"
#include "LoadObj.h"
#include "BulletPhysics.h"
#include "LoadVideo.h"
#include "SelectMapMode.h"
#include "MapModel.h"
#include "BarricateMap1Model.h"
#include "Ui_v2.h"
#include "KartModel.h"
#include "RoadModel.h"


void LogoMode::init()
{
    //std::thread videoThread(&LogoMode::runVideo, this);
    //std::thread soundThread(&LogoMode::runSound, this);

    //videoThread.join();
    //soundThread.join();
}

void LogoMode::runVideo()
{
    loadVideo("kartrider_intro.mp4", "./asset/select_mode/", &isRunning);
}

void LogoMode::runSound()
{
    play_sound2D("kartrider_intro.WAV", "./asset/select_mode/", false, &isRunning);
}
