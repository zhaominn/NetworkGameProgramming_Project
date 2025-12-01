#include "Room.h"

void Room::reset()
{
    // roomManagerID = -1;
    gameStart = false;
    elapsedTime = 0.0f;
    mapType = STRAIGHT;
    rank = 0;
    room_player_cnt = 0;

    for (int i = 0; i < MAX_USER; i++)
        inRoomPlayers[i] = nullptr;
}