#include "Pch.h"
#include "Model.h"
#include "BulletPhysics.h"
#include "CustomContactResultCallback.h"

// Bullet
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"


btBroadphaseInterface* broadphase = nullptr;
btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
btCollisionDispatcher* dispatcher = nullptr;
btSequentialImpulseConstraintSolver* solver = nullptr;
btDiscreteDynamicsWorld* dynamicsWorld = nullptr;

// Bullet Physics
void initPhysics() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

// Model
glm::vec3 calculateModelSize(const Model* model) {
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::lowest());

    for (const Vertex& vertex : model->vertices) {
        glm::vec4 transformedVertex = model->matrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);

        min.x = std::min(min.x, transformedVertex.x);
        min.y = std::min(min.y, transformedVertex.y);
        min.z = std::min(min.z, transformedVertex.z);

        max.x = std::max(max.x, transformedVertex.x);
        max.y = std::max(max.y, transformedVertex.y);
        max.z = std::max(max.z, transformedVertex.z);
    }

    return max - min;
}

void addModelToPhysicsWorld(Model* model) {
    glm::vec3 size = calculateModelSize(model);

    btCollisionShape* shape = nullptr;
    if (model->type == "box") {
        shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    else if (model->type == "sphere") {
        float radius = std::max(std::max(size.x, size.y), size.z) * 0.5f;
        shape = new btSphereShape(radius);
    }
    else if (model->type == "cylinder") {
        shape = new btCylinderShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    else if (model->type == "capsule") {
        float radius = std::min(size.x, size.z) * 0.5f;
        float height = size.y - 2 * radius;
        shape = new btCapsuleShape(radius, height);
    }
    else if (model->type == "cone") {
        float radius = size.x * 0.5f;
        float height = size.y;
        shape = new btConeShape(radius, height);
    }
    else if (model->type == "plane") {
        float radius = size.x * 0.5f;
        float height = size.y;
        shape = new btConeShape(radius, height); 
    }
    else {
        shape = new btBoxShape(btVector3(size.x * 0.5f, 0.01f, size.z * 0.5f));
    }

    if (!shape) {
        std::cerr << "Failed to create collision shape for model: " << model->name << std::endl;
        return;
    }

    // OpenGL model matrix
    glm::vec3 translation, scale, skew;
    glm::vec4 perspective;
    glm::quat rotation;
    glm::decompose(model->matrix, scale, rotation, translation, skew, perspective);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(translation.x, translation.y, translation.z));

    btScalar mass = 1.0f;
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
    model->rigidBody = body;
}

//
void initializeModelsWithPhysics(std::vector<Model*>& models) {
    for (auto& model : models) {
        addModelToPhysicsWorld(model);
    }
}

// Bullet
void cleanupPhysics() {
    if (!dynamicsWorld) return;

    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    delete dynamicsWorld; dynamicsWorld = nullptr;
    delete solver; solver = nullptr;
    delete dispatcher; dispatcher = nullptr;
    delete collisionConfiguration; collisionConfiguration = nullptr;
    delete broadphase; broadphase = nullptr;
}


void removeModelFromWorld(std::vector<Model*>& models, Model*& modelToDelete) {
    if (modelToDelete && modelToDelete->rigidBody) {
        dynamicsWorld->removeRigidBody(modelToDelete->rigidBody);
        delete modelToDelete->rigidBody->getMotionState();
        delete modelToDelete->rigidBody;
        modelToDelete->rigidBody = nullptr;
    }

    auto it = std::find_if(models.begin(), models.end(),
        [&](const Model* m) { return m == modelToDelete; });

    if (it != models.end()) {
        models.erase(it);
    }
}


void removeRigidBodyFromModel(Model*& model) {
    if (model && model->rigidBody) {
        dynamicsWorld->removeRigidBody(model->rigidBody);
        delete model->rigidBody->getMotionState();
        delete model->rigidBody;
        model->rigidBody = nullptr;
    }
}


void UpdateRigidBodyTransforms(std::vector<Model*> models) {
    for (auto& model : models) {
        if (!model || !model->rigidBody) continue;

        glm::mat4 modelMatrix = model->translateMatrix;
        btTransform transform;
        transform.setFromOpenGLMatrix(glm::value_ptr(modelMatrix));
        model->rigidBody->setWorldTransform(transform);
    }
}


void UpdateRigidBodyTransform(Model*& model) {
    if (!model || !model->rigidBody) return;

    glm::mat4 modelMatrix = model->matrix;
    btTransform transform;
    transform.setFromOpenGLMatrix(glm::value_ptr(modelMatrix));

    std::cout << "Updating RigidBody for Model: " << model->name
        << " | OpenGL Position: (" << modelMatrix[3][0] << ", "
        << modelMatrix[3][1] << ", " << modelMatrix[3][2] << ")"
        << std::endl;

    model->rigidBody->setWorldTransform(transform);
}


void RenderCollisionBox(const Model* model, GLuint shaderProgram) {
    if (!model || !model->rigidBody) return;

    btVector3 aabbMin, aabbMax;
    model->rigidBody->getCollisionShape()->getAabb(model->rigidBody->getWorldTransform(), aabbMin, aabbMax);

    glm::vec3 min = { aabbMin.getX(), aabbMin.getY(), aabbMin.getZ() };
    glm::vec3 max = { aabbMax.getX(), aabbMax.getY(), aabbMax.getZ() };

    glm::vec3 corners[8] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, max.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };

    GLuint indices[24] = {
        0,1, 1,2, 2,3, 3,0,
        4,5, 5,6, 6,7, 7,4,
        0,4, 1,5, 2,6, 3,7
    };

    // --- OpenGL (VAO/VBO/EBO) ---
    GLuint vao = 0, vbo = 0, ebo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(corners), corners, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);

    //
    glm::mat4 identityMatrix(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identityMatrix));
    if (viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(identityMatrix));
    if (projectionLoc != -1) glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(identityMatrix));

    //
    GLint isRigidBodyLoc = glGetUniformLocation(shaderProgram, "isRigidBody");
    if (isRigidBodyLoc != -1) glUniform1i(isRigidBodyLoc, 1);

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    if (isRigidBodyLoc != -1) glUniform1i(isRigidBodyLoc, 0);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}
