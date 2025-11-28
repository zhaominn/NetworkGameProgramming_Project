#pragma once

#include <glm/glm/gtc/matrix_transform.hpp>


using namespace std;

class MapModel : public Model {
public:
	MapModel() {}

	MapModel(string name, string path, string obj_name, string obj_type, bool rigid_status, glm::mat4 start_matrix = glm::mat4(1.0f));

	~MapModel() {}

	void load_obj(string name, string path, string obj_name, string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) override;

	const void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) override;

	const void draw_rigidBody(GLuint shaderProgramID) override;

	void initBuffer() override;

};
