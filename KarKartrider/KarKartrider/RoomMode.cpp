#include "Pch.h"
#include "RoomMode.h"

RoomMode::RoomMode()
{
}

RoomMode::~RoomMode()
{
}

void RoomMode::init()
{
}

void RoomMode::mouseClick(int button, int state, int x, int y)
{
}

void RoomMode::keyboard(unsigned char key, int x, int y)
{
}

void RoomMode::specialKey(int key, int x, int y)
{
}

void RoomMode::specialKeyUp(int key, int x, int y)
{
}

void RoomMode::draw_model()
{

    // 여기서는 "그리기만" 하고,
    // 클리어와 스왑은 drawScene에서만 하도록.

    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(-0.1f, 0.0f);
    std::string text = "룸";

    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void RoomMode::draw_bb()
{
}

void RoomMode::finish()
{
}
