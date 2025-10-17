#pragma once

#include <vector>
#include <string>
#include <glew.h>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Model.h"
#include "LoadObj.h"
#include "BulletPhysics.h"

using namespace std;

class CharacterModel : public Model {
public:
    CharacterModel() {}

    CharacterModel(string name, string path, string obj_name, string obj_type, bool rigid_status, glm::mat4 start_matrix = glm::mat4(1.0f));

    ~CharacterModel() {}

    void load_obj(string name, string path, string obj_name, string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) override;

    const void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) override;

    const void draw_rigidBody(GLuint shaderProgramID) override;

    void initBuffer() override;


};
