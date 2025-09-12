class CustomContactResultCallback : public btCollisionWorld::ContactResultCallback {
public:
    bool hitDetected = false;                // �浹 ���� �÷���
    btVector3 collisionPointA;               // �浹 ���� A
    btVector3 collisionPointB;               // �浹 ���� B
    btVector3 collisionNormal;               // �浹 ���� ����
    float penetrationDepth = 0.0f;           // ħ�� ����

    // �浹�� �߻��� ������ ȣ��Ǵ� �Լ�
    virtual btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0, int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1, int index1) override
    {
        hitDetected = true;

        // �浹 ���� A�� B�� ���
        collisionPointA = cp.getPositionWorldOnA();
        collisionPointB = cp.getPositionWorldOnB();

        // �浹 ���� ���Ϳ� ħ�� ���� ����
        collisionNormal = cp.m_normalWorldOnB;
        penetrationDepth = cp.getDistance(); // m_distance1�� getDistance()�� ���� ����

        return 0;
    }

    // �浹 ���θ� �����ϴ� �Լ�
    void reset() {
        hitDetected = false;
        penetrationDepth = 0.0f;
    }
};