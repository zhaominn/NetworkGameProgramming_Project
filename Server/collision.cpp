#include "collision.h"

void ApplyBounceReflection(Player& p, float pushX, float pushZ, float bounce)
{
	float nx = (pushX > 0 ? 1.0f : (pushX < 0 ? -1.0f : 0));
	float nz = (pushZ > 0 ? 1.0f : (pushZ < 0 ? -1.0f : 0));

	float yaw = p.GetYaw();
	float speed = p.GetSpeed();

	float vx = cosf(yaw) * speed;
	float vz = sinf(yaw) * speed;

	float dot = vx * nx + vz * nz;

	float rvx = vx - 2 * dot * nx;
	float rvz = vz - 2 * dot * nz;

	rvx *= bounce;
	rvz *= bounce;

	float newSpeed = sqrtf(rvx * rvx + rvz * rvz);
	float newYaw = atan2f(rvz, rvx);

	p.SetSpeed(newSpeed);
	p.SetYaw(newYaw);
}



bool WallCollisionCheck(const AABB& box, int id, float& pushX, float& pushZ)
{
	if (!box.rigid_status)
		return false;
	pushX = pushZ = 0;

	float px = g_users[id].m_posX;
	float pz = g_users[id].m_posZ;

	float hx = g_users[id].m_colliderHalfX;
	float hz = g_users[id].m_colliderHalfZ;

	float pMinX = px - hx;
	float pMaxX = px + hx;
	float pMinZ = pz - hz;
	float pMaxZ = pz + hz;

	if (pMaxX < box.minX || pMinX > box.maxX) return false;
	if (pMaxZ < box.minZ || pMinZ > box.maxZ) return false;

	float penLeft = box.maxX - pMinX;
	float penRight = pMaxX - box.minX;
	float penFront = box.maxZ - pMinZ;
	float penBack = pMaxZ - box.minZ;

	float pushXAmount, pushZAmount;

	pushXAmount = (penLeft < penRight ? penLeft : -penRight);
	pushZAmount = (penFront < penBack ? penFront : -penBack);

	if (fabs(pushXAmount) < fabs(pushZAmount))
	{
		pushX = pushXAmount;
	}
	else
	{
		pushZ = pushZAmount;
	}

	return true;
}


bool PlayerCollisionCheck(int a, int b, float& pushX, float& pushZ)
{
	float x1 = g_users[a].m_posX;
	float z1 = g_users[a].m_posZ;

	float x2 = g_users[b].m_posX;
	float z2 = g_users[b].m_posZ;

	float hx1 = g_users[a].m_colliderHalfX;
	float hz1 = g_users[a].m_colliderHalfZ;

	float hx2 = g_users[b].m_colliderHalfX;
	float hz2 = g_users[b].m_colliderHalfZ;

	float dx = x1 - x2;
	float dz = z1 - z2;

	float overlapX = (hx1 + hx2) - fabs(dx);
	float overlapZ = (hz1 + hz2) - fabs(dz);

	if (overlapX > 0 && overlapZ > 0)
	{
		if (overlapX < overlapZ)
		{
			pushX = (dx > 0 ? overlapX : -overlapX);
			pushZ = 0;
		}
		else
		{
			pushX = 0;
			pushZ = (dz > 0 ? overlapZ : -overlapZ);
		}
		return true;
	}

	return false;
}

void ProcessWallCollision(Player& p, float& outPushX, float& outPushZ)
{
	outPushX = outPushZ = 0.0f;
	
	if (p.select_map == MAP_TYPE::STRAIGHT) {
		for (auto& box : p.g_Map1Colliders)
		{
			float px, pz;
			if (WallCollisionCheck(box, p.GetID(), px, pz))
			{
				outPushX += px;
				outPushZ += pz;
			}
		}
	}

	if (p.select_map == MAP_TYPE::RECTANGLE) {
		for (auto& box : p.g_Map2Colliders)
		{
			float px, pz;
			if (WallCollisionCheck(box, p.GetID(), px, pz))
			{
				outPushX += px;
				outPushZ += pz;
			}
		}
	}
	
}

void ProcessPlayerCollisionRoom(Room& room, int myPlayerId, float& outX, float& outZ)
{
	outX = 0.0f;
	outZ = 0.0f;

	Player* me = g_users[myPlayerId].GetOnline() ? &g_users[myPlayerId] : nullptr;
	if (!me) return;

	for (int i = 0; i < MAX_USER; i++)
	{
		Player* other = room.inRoomPlayers[i];

		if (!other) continue;
		if (other->GetID() == -1) continue; // 안전
		if (other->GetID() == myPlayerId) continue; // 나 자신 제외

		float px = 0, pz = 0;

		// myPlayerId vs other->GetID()
		if (PlayerCollisionCheck(myPlayerId, other->GetID(), px, pz))
		{
			outX += px * 0.5f;
			outZ += pz * 0.5f;
		}
	}
}
