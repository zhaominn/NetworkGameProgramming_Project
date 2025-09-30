#include "CustomContactResultCallback.h"

// 충돌이 발생할 때마다 호출
btScalar CustomContactResultCallback::addSingleResult(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0Wrap,
    int partId0, int index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int partId1, int index1)
{
    hitDetected = true;

    // 충돌 지점 A와 B 기록
    collisionPointA = cp.getPositionWorldOnA();
    collisionPointB = cp.getPositionWorldOnB();

    // 충돌 법선 벡터와 침투 깊이 저장
    collisionNormal = cp.m_normalWorldOnB;
    penetrationDepth = cp.getDistance();

    return 0;
}

// 충돌 여부 초기화
void CustomContactResultCallback::reset() {
    hitDetected = false;
    penetrationDepth = 0.0f;
}
