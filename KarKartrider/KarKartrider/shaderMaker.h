#pragma once
#include <iostream>
#include <glew.h>

#include "filetobuf.h"

using namespace std;  // ���ӽ����̽� std ������� �ڵ� ������ std:: ���� ����

// ���� ���� �����
GLint width, height;  // â�� �ʺ�� ���̸� �����ϴ� ����
GLuint shaderProgramID;  // ���̴� ���α׷��� ID�� �����ϴ� ����
GLuint vertexShader;  // ���ؽ� ���̴� ID�� �����ϴ� ����
GLuint fragmentShader;  // �����׸�Ʈ ���̴� ID�� �����ϴ� ����

GLchar* vertexSource, * fragmentSource;  // ���̴� �ҽ� �ڵ带 ������ ������

// ���ؽ� ���̴��� ����� �Լ�
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.glsl");  // ���ؽ� ���̴� ���� �б�
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // ���ؽ� ���̴� ����
    std::cout << "Vertex Shader ID: " << vertexShader << std::endl;
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(vertexShader);  // ���̴� ������

    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

}

// �����׸�Ʈ ���̴��� ����� �Լ�
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.glsl");  // �����׸�Ʈ ���̴� ���� �б�
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  // �����׸�Ʈ ���̴� ����
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(fragmentShader);  // ���̴� ������

    GLint success;
    char infoLog[512];
    // ������ ���� Ȯ��
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// ���̴� ���α׷��� ����� �Լ�
void make_shaderProgram() {
    make_vertexShaders();  // ���ؽ� ���̴� ����
    make_fragmentShaders();  // �����׸�Ʈ ���̴� ����

    shaderProgramID = glCreateProgram();  // ���̴� ���α׷� ����
    glAttachShader(shaderProgramID, vertexShader);  // ���ؽ� ���̴� ���α׷��� ÷��
    glAttachShader(shaderProgramID, fragmentShader);  // �����׸�Ʈ ���̴� ���α׷��� ÷��
    glLinkProgram(shaderProgramID);  // ���̴� ���α׷� ��ũ
    // ���α׷� ��ũ ���� Ȯ��
    GLint success;
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    else {
        std::cout << "Shader Program linked successfully.\n";
    }
    glDeleteShader(vertexShader);  // ���ؽ� ���̴� ���� (�̹� ���α׷��� ���ԵǾ����Ƿ�)
    glDeleteShader(fragmentShader);  // �����׸�Ʈ ���̴� ����

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���
}

GLuint shaderProgramID_UI;  // ���̴� ���α׷��� ID�� �����ϴ� ����
GLuint vertexShader_UI;  // ���ؽ� ���̴� ID�� �����ϴ� ����
GLuint fragmentShader_UI;  // �����׸�Ʈ ���̴� ID�� �����ϴ� ����

GLchar* vertexSource_UI, * fragmentSource_UI;  // ���̴� �ҽ� �ڵ带 ������ ������

// ���ؽ� ���̴��� ����� �Լ�
void make_vertexShaders_ui() {
    vertexSource_UI = filetobuf("vertex_ui.glsl");  // ���ؽ� ���̴� ���� �б�
    vertexShader_UI = glCreateShader(GL_VERTEX_SHADER);  // ���ؽ� ���̴� ����
    std::cout << "Vertex Shader ID: " << vertexShader_UI << std::endl;
    glShaderSource(vertexShader_UI, 1, (const GLchar**)&vertexSource_UI, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(vertexShader_UI);  // ���̴� ������

    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader_UI, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader_UI, 512, NULL, infoLog);
        std::cout << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

}

// �����׸�Ʈ ���̴��� ����� �Լ�
void make_fragmentShaders_ui() {
    fragmentSource_UI = filetobuf("fragment_ui.glsl");  // �����׸�Ʈ ���̴� ���� �б�
    fragmentShader_UI = glCreateShader(GL_FRAGMENT_SHADER);  // �����׸�Ʈ ���̴� ����
    glShaderSource(fragmentShader_UI, 1, (const GLchar**)&fragmentSource_UI, 0);  // ���̴� �ҽ� �ڵ� ����
    glCompileShader(fragmentShader_UI);  // ���̴� ������

    GLint success;
    char infoLog[512];
    // ������ ���� Ȯ��
    glGetShaderiv(fragmentShader_UI, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader_UI, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// ���̴� ���α׷��� ����� �Լ�
void make_shaderProgram_ui() {
    make_vertexShaders_ui();  // ���ؽ� ���̴� ����
    make_fragmentShaders_ui();  // �����׸�Ʈ ���̴� ����

    shaderProgramID_UI = glCreateProgram();  // ���̴� ���α׷� ����
    glAttachShader(shaderProgramID_UI, vertexShader_UI);  // ���ؽ� ���̴� ���α׷��� ÷��
    glAttachShader(shaderProgramID_UI, fragmentShader_UI);  // �����׸�Ʈ ���̴� ���α׷��� ÷��
    glLinkProgram(shaderProgramID_UI);  // ���̴� ���α׷� ��ũ
    // ���α׷� ��ũ ���� Ȯ��
    GLint success;
    glGetProgramiv(shaderProgramID_UI, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgramID_UI, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    else {
        std::cout << "Shader Program linked successfully.\n";
    }
    glDeleteShader(vertexShader_UI);  // ���ؽ� ���̴� ���� (�̹� ���α׷��� ���ԵǾ����Ƿ�)
    glDeleteShader(fragmentShader_UI);  // �����׸�Ʈ ���̴� ����

    glUseProgram(shaderProgramID_UI);  // ���̴� ���α׷� ���
}
