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

    loadModelWithProgress<KartModel>("bazzi_face2.obj", "obj/character/", "character_face", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
    loadModelWithProgress<KartModel>("bazzi_body.obj", "obj/character/", "character_body", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
    loadModelWithProgress<KartModel>("booster.obj", "obj/car/booster/", "booster", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
    loadModelWithProgress<KartModel>("kronos.obj", "obj/car/kronos/", "car", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), karts, true, true);

    loadModelWithProgress<Ui_v2>("booster_ui.obj", "obj/ui/", "booster_ui1", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.90, 0.75, 0.0)), glm::vec3(0.1, 0.1, 0.1)), booster_uis, false, true);
    loadModelWithProgress<Ui_v2>("booster_ui.obj", "obj/ui/", "booster_ui2", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.78, 0.75, 0.0)), glm::vec3(0.1, 0.1, 0.1)), booster_uis, false, true);

    loadModelWithProgress<Ui_v2>("dashboard.obj", "obj/ui/", "dashboard", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -0.92, 0.0)), glm::vec3(0.7, 0.6, 0.5)), dashBoards, false, true);
    loadModelWithProgress<KartModel>("pause.obj", "obj/ui/", "pause", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), pause, true, true);

    loadModelWithProgress<RoadModel>("road_all_01.obj", "obj/road/", "road_all_1", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(15.0, 15.0, 15.0)), road1, false, true);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(16.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 700.0)), road1_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-16.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 700.0)), road1_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 304.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, true);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -304.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, true);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -212.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, false);

    loadModelWithProgress<RoadModel>("road_all_02.obj", "obj/road/", "road_all_2", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(20.0, 20.0, 20.0)), road2, false, true);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(184.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 400.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-184.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 400.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 184.7)), glm::vec3(400.0, 15.0, 15.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -184.7)), glm::vec3(400.0, 15.0, 15.0)), road2_barricate, true, false);
    //loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)), glm::vec3(331.0, 331.0, 331.0)), road2_barricate, true, false); //��� ��ֹ�
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(145.3, 0.0, 0.0)), glm::vec3(15.0, 15.0, 315.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-145.3, 0.0, 0.0)), glm::vec3(15.0, 15.0, 315.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 145.3)), glm::vec3(315.0, 15.0, 15.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -145.3)), glm::vec3(315.0, 15.0, 15.0)), road2_barricate, true, false);

    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(165.0, 0.0, 18.0)), glm::vec3(40.0, 1.0, 1.0)), road2_barricate, true, false);
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish_ch", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(165.0, 0.0, 50.0)), glm::vec3(40.0, 1.0, 1.0)), road2_barricate, true, false);

    glm::mat4 m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(174.7, 0.0, -175.3));
    m1 = glm::rotate(m1, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

    m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(174.7, 0.0, 175.3));
    m1 = glm::rotate(m1, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

    m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(-174.7, 0.0, 175.3));
    m1 = glm::rotate(m1, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

    m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(-174.7, 0.0, -175.3));
    m1 = glm::rotate(m1, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, false);

    glm::mat4 m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(146.8, 0.0, -146.8));
    m2 = glm::rotate(m2, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    m2 = glm::scale(m2, glm::vec3(5.0, 15.0, 10.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

    m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(146.8, 0.0, 146.8));
    m2 = glm::rotate(m2, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    m2 = glm::scale(m2, glm::vec3(5.0, 15.0, 10.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

    m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(-146.8, 0.0, 146.8));
    m2 = glm::rotate(m2, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    m2 = glm::scale(m2, glm::vec3(5.0, 15.0, 10.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

    m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(-146.8, 0.0, -146.8));
    m2 = glm::rotate(m2, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    m2 = glm::scale(m2, glm::vec3(5.0, 15.0, 10.0));
    loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m2, road2_barricate, true, false);

    loadModelWithProgress<KartModel>("3.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
    loadModelWithProgress<KartModel>("2.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
    loadModelWithProgress<KartModel>("1.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
    loadModelWithProgress<KartModel>("start.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);

    //loadModelWithProgress<KartModel>("pause.obj", "obj/ui/", "pause", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), pause, true, true);

    loadModelWithProgress<MapModel>("village_road.obj", "asset/select_mode/", "village_road", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), selectMaps, false, true);

    glm::mat4 map2_matrix = glm::mat4(1.0f);
    map2_matrix = glm::translate(map2_matrix, glm::vec3(2.5, 0.0, 0.0));
    map2_matrix = glm::scale(map2_matrix, glm::vec3(1.0, 1.0, 1.0));
    loadModelWithProgress<MapModel>("villiage_unha.obj", "asset/select_mode/", "villiage_unha", "box", map2_matrix, selectMaps, false, true);

    glm::mat4 arrow_matrix = glm::mat4(1.0f);
    arrow_matrix = glm::translate(arrow_matrix, glm::vec3(-0.2, -1.0, 1.0));
    arrow_matrix = glm::scale(arrow_matrix, glm::vec3(0.5, 0.5, 0.5));
    arrow_matrix = glm::rotate(arrow_matrix, glm::radians(-80.0f), glm::vec3(1.0, 0.0, 0.0));
    loadModelWithProgress<MapModel>("arrow_key.obj", "asset/select_mode/", "arrow_key", "box", arrow_matrix, selectMaps, false, true);

    glm::mat4 enter_matrix = glm::mat4(1.0f);
    enter_matrix = glm::translate(enter_matrix, glm::vec3(0.6, -1.0, 1.0));
    enter_matrix = glm::scale(enter_matrix, glm::vec3(0.4, 0.3, 0.4));
    enter_matrix = glm::rotate(enter_matrix, glm::radians(-80.0f), glm::vec3(1.0, 0.0, 0.0));
    loadModelWithProgress<MapModel>("enter_key.obj", "asset/select_mode/", "enter_key", "box", enter_matrix, selectMaps, false, true);

    //videoThread.join();
    //soundThread.join();

    isRunning = false;
    SelectMapMode* selectMapMode = new SelectMapMode();
    MM.SetMode(selectMapMode);
}

void LogoMode::runVideo()
{
    loadVideo("kartrider_intro.mp4", "./asset/select_mode/", &isRunning);
}

void LogoMode::runSound()
{
    play_sound2D("kartrider_intro.WAV", "./asset/select_mode/", false, &isRunning);
}
