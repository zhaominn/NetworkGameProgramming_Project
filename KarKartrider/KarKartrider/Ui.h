#pragma once

#include <vector>
#include <string>
#include <gl/glew.h>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Model.h"
#include "LoadObj.h"
#include "BulletPhysics.h"

using namespace std;

class Ui : public Model {
public:
    Ui() {}

    Ui(string name, string path, string obj_name, string obj_type, bool rigid_status, glm::mat4 start_matrix = glm::mat4(1.0f)) {
        read_obj_file(name, path, this, obj_name, obj_type);
        this->matrix = start_matrix * this->matrix;
        this->rigid_status = rigid_status;
    }

    ~Ui() {}

    void load_obj(string name, string path, string obj_name, string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) override {
        read_obj_file(name, path, this, obj_name, obj_type);
        this->matrix = start_matrix * this->matrix;
    }

    const void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) override {
        if (!this->draw_status) return;

        GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        GLint normalLoc = glGetUniformLocation(shaderProgramID, "normalMatrix");
        if (this->model_status) {
            // VAO ���ε�
            glBindVertexArray(this->vao);

            // **���� ��� ���**
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(this->matrix)));
            glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // **�� ��� ����**
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->matrix));

            GLuint lastBoundTextureID = 0; // ���� �ؽ�ó ID ����
            for (const auto& [materialName, ebo] : this->textureEBOs) {
                auto it = this->materials.find(materialName);
                if (it == this->materials.end()) {
#ifdef DEBUG_MODE
                    std::cerr << "No material found for: " << materialName << std::endl;
#endif
                    continue;
                }

                const Material& material = it->second;

                // �ؽ�ó ���ε�
                bool textureBound = false;
                for (const auto& texture : material.textures) {
                    if (texture.type == "diffuse") {
                        glActiveTexture(GL_TEXTURE0);
                        if (lastBoundTextureID != texture.id) {
                            glBindTexture(GL_TEXTURE_2D, texture.id);
                            lastBoundTextureID = texture.id;
                        }
                        glUniform1i(glGetUniformLocation(shaderProgramID, "map_Kd"), 0);
                        glUniform1i(glGetUniformLocation(shaderProgramID, "hasKdTexture"), 1);
                        textureBound = true;
                    }
                }

                if (!textureBound) {
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

                    GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
                    GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
                    GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
                    GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

                    glUniform3fv(KaLoc, 1, glm::value_ptr(material.Ka));
                    glUniform3fv(KdLoc, 1, glm::value_ptr(material.Kd));
                    glUniform3fv(KsLoc, 1, glm::value_ptr(material.Ks));
                    glUniform1f(NsLoc, material.Ns);
                }

                // **EBO ������**
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(textureGroups[materialName].size()), GL_UNSIGNED_INT, 0);
            }

            // **������ �� ���� ����**
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            // OpenGL ���� �ʱ�ȭ
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
        }
    }




    const void draw_rigidBody(GLuint shaderProgramID) override {
        if (this->rigidBody) {
            RenderCollisionBox(this, shaderProgramID); // �浹 �ڽ� �׸���
        }
    }

    void initBuffer() override {
        // Step 1: VAO ���� �� ���ε�
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        std::unordered_map<std::tuple<unsigned int, unsigned int, unsigned int>, unsigned int, TupleHash> uniqueVertices;
        std::vector<glm::vec3> vertexBuffer;   // ��ġ ������
        std::vector<glm::vec3> normalBuffer;   // ���� ������
        std::vector<glm::vec2> texCoordBuffer; // �ؽ�ó ��ǥ ������

        // Step 2: Face�� �ؽ�ó���� �׷�ȭ
        for (const Face& face : this->faces) {
            std::string materialName = face.materialName.empty() ? "default" : face.materialName;

            // ���ο� ������ ���� �ʱ�ȭ
            if (textureGroups.find(materialName) == textureGroups.end()) {
                textureGroups[materialName] = {};
            }

            for (int i = 0; i < 3; i++) { // �ﰢ���� �� ���� ó��
                unsigned int vertexIndex = (i == 0) ? face.v1 : (i == 1) ? face.v2 : face.v3;
                unsigned int texCoordIndex = (i == 0) ? face.t1 : (i == 1) ? face.t2 : face.t3;
                unsigned int normalIndex = (i == 0) ? face.n1 : (i == 1) ? face.n2 : face.n3;
                std::tuple<unsigned int, unsigned int, unsigned int> key = std::make_tuple(vertexIndex, texCoordIndex, normalIndex);

                // ������ �������� Ȯ��
                if (uniqueVertices.count(key) == 0) {
                    uniqueVertices[key] = static_cast<unsigned int>(vertexBuffer.size());

                    // ��ġ �߰�
                    vertexBuffer.push_back(glm::vec3(
                        this->vertices[vertexIndex].x,
                        this->vertices[vertexIndex].y,
                        this->vertices[vertexIndex].z
                    ));

                    // �ؽ�ó ��ǥ �߰�
                    if (texCoordIndex != static_cast<unsigned int>(-1)) {
                        texCoordBuffer.push_back(glm::vec2(
                            this->texCoords[texCoordIndex].u,
                            this->texCoords[texCoordIndex].v
                        ));
                    }
                    else {
                        texCoordBuffer.push_back(glm::vec2(0.0f, 0.0f));
                    }

                    // ���� �߰�
                    normalBuffer.push_back(glm::vec3(
                        this->normals[normalIndex].nx,
                        this->normals[normalIndex].ny,
                        this->normals[normalIndex].nz
                    ));
                }

                // �ε����� �ؽ�ó �׷쿡 �߰�
                textureGroups[materialName].push_back(uniqueVertices[key]);
            }
        }

        // Step 3: VBO ���� - ��ġ ������
        glGenBuffers(1, &vbos[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(glm::vec3), vertexBuffer.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Step 4: VBO ���� - ���� ������
        glGenBuffers(1, &vbos[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, normalBuffer.size() * sizeof(glm::vec3), normalBuffer.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(1);

        // Step 5: VBO ���� - �ؽ�ó ��ǥ ������
        glGenBuffers(1, &vbos[2]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
        glBufferData(GL_ARRAY_BUFFER, texCoordBuffer.size() * sizeof(glm::vec2), texCoordBuffer.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        glEnableVertexAttribArray(2);

        // Step 6: �ؽ�ó�� EBO ����
        for (auto& [materialName, indices] : textureGroups) {
            GLuint ebo;
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            textureEBOs[materialName] = ebo; // EBO ����
        }

        // Step 7: VAO ���ε� ����
        glBindVertexArray(0);
    }

};