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

class btRigidBody;

struct Vertex {
    float x, y, z;
    Vertex() = default;
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}

    glm::vec3 toVec3() const { return glm::vec3(x, y, z); }
};

struct Texture {
    std::string type;
    GLuint id;         
    std::string path;  
};

struct TextureCoord {
    float u, v, w;
};

struct Normal {
    float nx, ny, nz;
};

struct Face {
    unsigned int v1, v2, v3;  
    unsigned int t1, t2, t3;  
    unsigned int n1, n2, n3;  
    std::string materialName; 
};

struct Material {
    std::string name;
    glm::vec3 Ka;  
    glm::vec3 Kd;  
    glm::vec3 Ks;  
    float Ns;      

    std::string map_Ka; 
    std::string map_Kd; 
    std::string map_Ks; 

    GLuint ambientTextureID = 0;  
    GLuint diffuseTextureID = 0;  
    GLuint specularTextureID = 0; 

    std::vector<Texture> textures;  

    bool hasTexture_s = false;  

    bool hasTexture() const { return !textures.empty(); }
};

class Model {
public:
    std::vector<Vertex> vertices;  
    std::vector<TextureCoord> texCoords;  
    std::vector<Normal> normals;  
    std::vector<Face> faces;       

    glm::mat4 matrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix = glm::mat4(1.0f);  
    glm::mat4 rotateMatrix = glm::mat4(1.0f);   

    std::unordered_map<std::string, Material> materials;

    std::string name;
    std::string type;

    GLuint textureID; 

    bool model_status = true;
    btRigidBody* rigidBody = nullptr;  
    bool rigid_status = true;
    bool draw_status = true;

    GLuint vao;
    GLuint vbos[4];
    std::unordered_map<std::string, std::vector<unsigned int>> textureGroups;
    std::unordered_map<std::string, GLuint> textureEBOs;

    std::unordered_map<std::string, Texture> materialTextureMap;

    virtual void load_obj(std::string name, std::string path, std::string obj_name, std::string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) = 0;
    virtual const void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) = 0;
    virtual const void draw_rigidBody(GLuint shaderProgramID) = 0;
    virtual void initBuffer() = 0;

    virtual ~Model() = default; 

    glm::vec3 getPosition() const {
        return glm::vec3(translateMatrix[3]);
    }

    void setPosition(const glm::vec3& pos) {
        translateMatrix[3] = glm::vec4(pos, 1.0f);
    }

    void setRotation(float yaw)
    {
        rotateMatrix = glm::mat4(1.0f);
        rotateMatrix = glm::rotate(
            rotateMatrix,
            glm::radians(yaw),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }
};