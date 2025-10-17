#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <glm/glm/glm.hpp>
#include <glew.h>

#include "include/stb_image.h"

/// �ؽ�ó �ε� �Լ�: OpenGL �ؽ�ó�� �����ϰ� ���Ͽ��� �����͸� �ε�
/// @param path �ؽ�ó ���� ���
/// @return OpenGL �ؽ�ó ID
Texture load_texture(const std::string& path, const std::string& type) {
    Texture texture;
    texture.type = type;
    texture.path = path;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "[ERROR] Failed to load texture: " << path << std::endl;
        throw std::runtime_error("Failed to load texture");
    }

    // OpenGL �ؽ�ó ID ����
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // �ؽ�ó ������ ���ε� �� ����
    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // �ؽ�ó ���� �� ���� �ɼ� ����
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // CPU �޸𸮿��� �ؽ�ó ������ ����
    stbi_image_free(data);

    // �ؽ�ó ID�� Texture ����ü�� ����
    texture.id = textureID;

    //std::cout << "[INFO] Texture loaded: " << path << " (ID: " << textureID << ")" << std::endl;

    return texture;  // Texture ����ü ��ȯ
}