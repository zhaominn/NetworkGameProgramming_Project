#pragma once
#include "Player.h"

void ApplyBounceReflection(Player& p, float pushX, float pushZ, float bounce);
bool WallCollisionCheck(const AABB& box, int id, float& pushX, float& pushZ);
bool PlayerCollisionCheck(int a, int b, float& pushX, float& pushZ);
void ProcessWallCollision(Player& p, float& outPushX, float& outPushZ);
void ProcessPlayerCollision(int myId, float& outPushX, float& outPushZ);
