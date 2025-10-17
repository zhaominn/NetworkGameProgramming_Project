#pragma once
#include <iostream>
#include <glew.h>

#include "filetobuf.h"

using namespace std; 

GLint width, height; 
GLuint shaderProgramID;  
GLuint vertexShader; 
GLuint fragmentShader;  

GLchar* vertexSource, * fragmentSource; 

void make_vertexShaders() {
    vertexSource = filetobuf("vertex.glsl");  
    vertexShader = glCreateShader(GL_VERTEX_SHADER); 
    std::cout << "Vertex Shader ID: " << vertexShader << std::endl;
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);  
    glCompileShader(vertexShader);

    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

}

void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.glsl"); 
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);  
    glCompileShader(fragmentShader);  

    GLint success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void make_shaderProgram() {
    make_vertexShaders();  
    make_fragmentShaders();  

    shaderProgramID = glCreateProgram();  
    glAttachShader(shaderProgramID, vertexShader);  
    glAttachShader(shaderProgramID, fragmentShader);  
    glLinkProgram(shaderProgramID);  

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
    glDeleteShader(vertexShader);  
    glDeleteShader(fragmentShader); 

    glUseProgram(shaderProgramID);  
}

GLuint shaderProgramID_UI; 
GLuint vertexShader_UI;  
GLuint fragmentShader_UI;  

GLchar* vertexSource_UI, * fragmentSource_UI; 

void make_vertexShaders_ui() {
    vertexSource_UI = filetobuf("vertex_ui.glsl");  
    vertexShader_UI = glCreateShader(GL_VERTEX_SHADER);  
    std::cout << "Vertex Shader ID: " << vertexShader_UI << std::endl;
    glShaderSource(vertexShader_UI, 1, (const GLchar**)&vertexSource_UI, 0);  
    glCompileShader(vertexShader_UI);  

    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader_UI, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader_UI, 512, NULL, infoLog);
        std::cout << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

}

void make_fragmentShaders_ui() {
    fragmentSource_UI = filetobuf("fragment_ui.glsl");  
    fragmentShader_UI = glCreateShader(GL_FRAGMENT_SHADER);  
    glShaderSource(fragmentShader_UI, 1, (const GLchar**)&fragmentSource_UI, 0); 
    glCompileShader(fragmentShader_UI);  

    GLint success;
    char infoLog[512];

    glGetShaderiv(fragmentShader_UI, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader_UI, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void make_shaderProgram_ui() {
    make_vertexShaders_ui();  
    make_fragmentShaders_ui();  

    shaderProgramID_UI = glCreateProgram();  
    glAttachShader(shaderProgramID_UI, vertexShader_UI);  
    glAttachShader(shaderProgramID_UI, fragmentShader_UI);
    glLinkProgram(shaderProgramID_UI); 
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
    glDeleteShader(vertexShader_UI);  
    glDeleteShader(fragmentShader_UI); 

    glUseProgram(shaderProgramID_UI);  
}
