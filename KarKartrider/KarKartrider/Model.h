#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>

#include <glew.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include"include/btBulletDynamicsCommon.h"

struct Vertex {
    float x, y, z;
    Vertex() = default;
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}

    glm::vec3 toVec3() const { return glm::vec3(x, y, z); }
};

struct Texture {
    std::string type;  // �ؽ�ó ���� (e.g., diffuse, specular, normal, ambient)
    GLuint id;         // OpenGL �ؽ�ó ID
    std::string path;  // �ؽ�ó ���� ���
};

struct TextureCoord {
    float u, v, w;
};

struct Normal {
    float nx, ny, nz;
};

struct Face {
    unsigned int v1, v2, v3;  // ���� �ε���
    unsigned int t1, t2, t3;  // �ؽ�ó ��ǥ �ε��� (������)
    unsigned int n1, n2, n3;  // ���� ���� �ε���
    std::string materialName; // Face�� ����ϴ� ���� �̸� (usemtl)
};

struct Material {
    std::string name;
    glm::vec3 Ka;  // ȯ�汤 ���
    glm::vec3 Kd;  // ���ݻ籤 ���
    glm::vec3 Ks;  // �ݻ籤 ���
    float Ns;      // ��¦�� ����

    std::string map_Ka; // ȯ�汤 �ؽ�ó ���� ���
    std::string map_Kd; // ���ݻ籤 �ؽ�ó ���� ���
    std::string map_Ks; // �ݻ籤 �ؽ�ó ���� ���

    GLuint ambientTextureID = 0;  // ȯ�汤 �ؽ�ó ID
    GLuint diffuseTextureID = 0;  // ���ݻ籤 �ؽ�ó ID
    GLuint specularTextureID = 0; // �ݻ籤 �ؽ�ó ID

    std::vector<Texture> textures;  // ���� �ؽ�ó ����

    bool hasTexture_s = false;  // �ؽ�ó�� �ִ��� ����

    // �ؽ�ó�� �ִ��� Ȯ��
    bool hasTexture() const { return !textures.empty(); }
};

class Model {
public:
    std::vector<Vertex> vertices;  // ���� �迭
    std::vector<TextureCoord> texCoords;  // �ؽ�ó ��ǥ �迭 (�߰�)
    std::vector<Normal> normals;   // ���� ���� �迭
    std::vector<Face> faces;       // �� �迭

    glm::mat4 matrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix = glm::mat4(1.0f);  // ���� �̵��� ��ġ�� ������ ����
    glm::mat4 rotateMatrix = glm::mat4(1.0f);    // ���� �ʱ� ȸ�� ���

    std::unordered_map<std::string, Material> materials;

    std::string name;
    std::string type; // �� ���� Ÿ�� ex) box, sphere, cylinder

    GLuint textureID;  // �ؽ�ó ID �ʵ� �߰�

    bool model_status = true;
    btRigidBody* rigidBody = nullptr;  // �� ���� ��ü
    bool rigid_status = true;
    bool draw_status = true;

    GLuint vao;
    GLuint vbos[4];
    // �ؽ�ó���� Face�� �׷�ȭ
    std::unordered_map<std::string, std::vector<unsigned int>> textureGroups;
    std::unordered_map<std::string, GLuint> textureEBOs;

    // ������ �ؽ�ó ����
    std::unordered_map<std::string, Texture> materialTextureMap;

    virtual void load_obj(std::string name, std::string path, std::string obj_name, std::string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) = 0;
    virtual const void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) = 0;
    virtual const void draw_rigidBody(GLuint shaderProgramID) = 0;
    virtual void initBuffer() = 0;

    virtual ~Model() = default; // ���� �Ҹ���
};