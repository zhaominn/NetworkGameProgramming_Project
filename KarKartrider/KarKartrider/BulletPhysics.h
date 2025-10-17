#pragma once
#include <vector>
#include <glm/glm/glm.hpp>

#ifndef GL_VERSION
using GLuint = unsigned int;
#endif

class Model;

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


extern btBroadphaseInterface* broadphase;
extern btDefaultCollisionConfiguration* collisionConfiguration;
extern btCollisionDispatcher* dispatcher;
extern btSequentialImpulseConstraintSolver* solver;
extern btDiscreteDynamicsWorld* dynamicsWorld;


void initPhysics();
glm::vec3 calculateModelSize(const Model* model);
void addModelToPhysicsWorld(Model* model);
void initializeModelsWithPhysics(std::vector<Model*>& models);
void cleanupPhysics();

void removeModelFromWorld(std::vector<Model*>& models, Model*& modelToDelete);

void removeRigidBodyFromModel(Model*& model);

void UpdateRigidBodyTransforms(std::vector<Model*> models);
void UpdateRigidBodyTransform(Model*& model);

void RenderCollisionBox(const Model* model, GLuint shaderProgram);