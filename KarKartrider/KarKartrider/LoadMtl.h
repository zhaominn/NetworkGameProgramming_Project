#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map> // �߰�: ������ ���� ����

#include "Model.h"
#include "LoadTexture.h"

// MTL ������ �о�ͼ� ���� ������ �Ľ��ϴ� �Լ�
void read_mtl_file(const std::string& filename, const std::string path, std::unordered_map<std::string, Material>& materials) {
    std::ifstream file(path + filename);  // MTL ���� ����
    if (!file.is_open()) {  // ���� ���� ���� �� ���� ó��
        std::cerr << "[ERROR] Failed to open MTL file: " << filename << std::endl;
        return;
    }

    std::string line, currentMaterial;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "newmtl") {
            // ���ο� ���� ����
            ss >> currentMaterial;
            materials[currentMaterial] = Material(); // �� ���� �߰�
            materials[currentMaterial].name = currentMaterial;
        }
        else if (prefix == "Ka") {  // ȯ�汤 ����
            ss >> materials[currentMaterial].Ka.x >> materials[currentMaterial].Ka.y >> materials[currentMaterial].Ka.z;
        }
        else if (prefix == "Kd") {  // ���ݻ籤 ����
            ss >> materials[currentMaterial].Kd.x >> materials[currentMaterial].Kd.y >> materials[currentMaterial].Kd.z;
        }
        else if (prefix == "Ks") {  // �ݻ籤 ����
            ss >> materials[currentMaterial].Ks.x >> materials[currentMaterial].Ks.y >> materials[currentMaterial].Ks.z;
        }
        else if (prefix == "Ns") {  // ��¦�� ����
            ss >> materials[currentMaterial].Ns;
        }
        else if (prefix == "map_Ka") {  // ȯ�汤 �ؽ�ó ��� (Ambient Texture)
            std::string texturePath;
            ss >> texturePath;
            Texture ambientTexture = load_texture(path + texturePath, "ambient");
            if (ambientTexture.id != 0) {
                materials[currentMaterial].textures.push_back(ambientTexture);
                materials[currentMaterial].map_Ka = texturePath;
                /*std::cout << "[SUCCESS] Ambient texture loaded: " << texturePath
                    << " (ID: " << ambientTexture.id << ")" << std::endl;*/
            }
            else {
                std::cerr << "[ERROR] Failed to load Ambient texture: " << texturePath << std::endl;
            }
        }
        else if (prefix == "map_Kd") {  // ���ݻ籤 �ؽ�ó ��� (Diffuse Texture)
            std::string texturePath;
            ss >> texturePath;
            cout << texturePath << endl;
            Texture diffuseTexture = load_texture(path + texturePath, "diffuse");
            if (diffuseTexture.id != 0) {
                materials[currentMaterial].textures.push_back(diffuseTexture);
                materials[currentMaterial].map_Kd = texturePath;
               /* std::cout << "[SUCCESS] Diffuse texture loaded: " << texturePath
                    << " (ID: " << diffuseTexture.id << ")" << std::endl;*/
            }
            else {
                std::cerr << "[ERROR] Failed to load Diffuse texture: " << texturePath << std::endl;
            }
        }
        else if (prefix == "map_Ks") {  // �ݻ籤 �ؽ�ó ��� (Specular Texture)
            std::string texturePath;
            ss >> texturePath;

            Texture specularTexture = load_texture(path + texturePath, "specular");
            if (specularTexture.id != 0) {
                materials[currentMaterial].textures.push_back(specularTexture);
                materials[currentMaterial].map_Ks = texturePath;
                /*std::cout << "[SUCCESS] Specular texture loaded: " << texturePath
                    << " (ID: " << specularTexture.id << ")" << std::endl;*/
            }
            else {
                std::cerr << "[ERROR] Failed to load Specular texture: " << texturePath << std::endl;
            }
        }
    }

    // ��� ������ ���� �ؽ�ó ���� Ȯ��
    for (const auto& [name, material] : materials) {
        materials.at(name).hasTexture_s = !material.textures.empty();
    }

    file.close();  // ���� �ݱ�
}