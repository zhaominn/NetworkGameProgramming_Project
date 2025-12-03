#pragma once
#include "Mode.h" 

class RoomMode : public Mode {
public:

    RoomMode();
    ~RoomMode();

    //
    GLuint player_tex;
    GLuint ready_tex;
    GLuint ready_hovered_tex;
    GLuint road1_tex;
    GLuint road1_hovered_tex;
    GLuint road1_playing_tex;
    GLuint road2_tex;
    GLuint road2_hovered_tex;
    GLuint road2_playing_tex;
    GLuint room_tex;
    GLuint player_ready_tex;

    bool isRoad1Hovered = false;
    bool isRoad2Hovered = false;
    bool isReadyHovered = false;

    int m_slots[3];
    //

    void init() override;

    GLuint loadTexture(const char* filename);

    void mouseClick(int button, int state, int x, int y) override;

    void passiveMotion(int x, int y) override;

    void keyboard(unsigned char key, int x, int y) override;

    void specialKey(int key, int x, int y) override;

    void specialKeyUp(int key, int x, int y) override;

    void draw_model() override;

    void draw_bb() override;

    void finish() override;

    bool CheckPlayGame(MAP_TYPE map);

    virtual ModeType GetModeType() const override {
        return ModeType::ROOM;
    }

    void RefreshSlotData();
    static void timerHelper(int value);

private:

    // bool ready_status{ false };

};

