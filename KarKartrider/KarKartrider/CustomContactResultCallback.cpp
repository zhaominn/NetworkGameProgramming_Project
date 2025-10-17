#include "Pch.h"
#include "CustomContactResultCallback.h"


btScalar CustomContactResultCallback::addSingleResult(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0Wrap,
    int partId0, int index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int partId1, int index1)
{
    hitDetected = true;

 
    collisionPointA = cp.getPositionWorldOnA();
    collisionPointB = cp.getPositionWorldOnB();

 
    collisionNormal = cp.m_normalWorldOnB;
    penetrationDepth = cp.getDistance();

    return 0;
}


void CustomContactResultCallback::reset() {
    hitDetected = false;
    penetrationDepth = 0.0f;
}
