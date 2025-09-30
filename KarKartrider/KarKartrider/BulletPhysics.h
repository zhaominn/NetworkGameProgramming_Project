#pragma once

// --- 최소 포함(선언에 필요한 것만) ---
#include <vector>
#include <glm/glm/glm.hpp> // glm::vec3 반환형 때문에 필요

// OpenGL의 GLuint가 다른 곳에서 이미 정의되어 있을 수도 있으므로
// 없을 때만 안전하게 별칭을 둡니다.
#ifndef GL_VERSION
using GLuint = unsigned int;
#endif

// 전방 선언들 (헤더 경량화)
class Model;

// Bullet 전방 선언
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionObject;
class btRigidBody;
class btCollisionShape;
class btTransform;
class btDefaultMotionState;

// --- 전역 Bullet 포인터: 정의는 .cpp에서, 여기서는 extern ---
extern btBroadphaseInterface* broadphase;
extern btDefaultCollisionConfiguration* collisionConfiguration;
extern btCollisionDispatcher* dispatcher;
extern btSequentialImpulseConstraintSolver* solver;
extern btDiscreteDynamicsWorld* dynamicsWorld;

// --- 함수 선언들 ---
void initPhysics();
glm::vec3 calculateModelSize(const Model* model);
void addModelToPhysicsWorld(Model* model);
void initializeModelsWithPhysics(std::vector<Model*>& models);
void cleanupPhysics();

void removeModelFromWorld(std::vector<Model*>& models, Model*& modelToDelete);
// 원본 코드에 선언/정의 시그니처 불일치가 있었음 → 실제 구현(Model*&)에 맞춰 통일
void removeRigidBodyFromModel(Model*& model);

void UpdateRigidBodyTransforms(std::vector<Model*> models);
void UpdateRigidBodyTransform(Model*& model);

void RenderCollisionBox(const Model* model, GLuint shaderProgram);