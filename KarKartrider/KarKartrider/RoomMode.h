#pragma once
#include "Mode.h" 

class RoomMode : public Mode {
public:

    RoomMode();
    ~RoomMode();

    void init() override;

    void mouseClick(int button, int state, int x, int y) override;

    void keyboard(unsigned char key, int x, int y) override;

    void specialKey(int key, int x, int y) override;

    void specialKeyUp(int key, int x, int y) override;

    void draw_model() override;

    void draw_bb() override;

    void finish() override;

    virtual ModeType GetModeType() const override {
        return ModeType::ROOM;
    }

private:

    bool ready_status{ false };

};

