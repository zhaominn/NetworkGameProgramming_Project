#pragma once
#include<iostream>
#include <gl/glm/glm/glm.hpp>

#include "DefaultModel.h"
#include "Model.h"

void printProgressBar(int progress, int total) {
    const int barWidth = 50; // �ε��� �ʺ�
    float percentage = static_cast<float>(progress) / total;

    std::cout << "\r[";
    int pos = static_cast<int>(barWidth * percentage);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(percentage * 100.0) << "%";
    std::cout.flush();
}

template <typename T, typename = std::enable_if<std::is_base_of<Model, T>::value>>
void loadModelWithProgress(const std::string& modelPath, const std::string path, const std::string& modelName, const std::string& modelType, glm::mat4 scale, std::vector<Model*>& models, bool rigid_status, bool draw_status) {
    std::cout << "Loading model: " << modelName << std::endl;

    const int totalSteps = 4; // �� �۾� �ܰ� ��
    int currentStep = 0;

    // Step 1: OBJ ���� �ε�
    std::cout << "Step 1/4: Reading OBJ file..." << std::endl;
    T* model = new T(modelPath, path, modelName, modelType, rigid_status, scale);  // ���ø����� �� ����
    currentStep++;
    printProgressBar(currentStep, totalSteps);

    // Step 2: MTL ���� ó��
    std::cout << "\nStep 2/4: Processing Materials (MTL file)..." << std::endl;
    for (const auto& [materialName, material] : model->materials) {
        std::cout << "Processing material: " << materialName << std::endl;
        if (material.hasTexture()) {
            std::cout << "  Material has textures: " << material.textures.size() << std::endl;
        }
        else {
            std::cout << "  No textures found for material: " << materialName << std::endl;
        }
    }
    currentStep++;
    printProgressBar(currentStep, totalSteps);

    // Step 3: �ؽ�ó �ε�
    std::cout << "\nStep 3/4: Loading textures..." << std::endl;
    for (const auto& [materialName, material] : model->materials) {
        for (const auto& texture : material.textures) {
            std::cout << "Loading texture: " << texture.path << std::endl;
            if (texture.id != 0) {
                std::cout << "  Texture loaded successfully (ID: " << texture.id << ")" << std::endl;
            }
            else {
                std::cerr << "  Failed to load texture: " << texture.path << std::endl;
            }
        }
    }
    currentStep++;
    printProgressBar(currentStep, totalSteps);

    // Step 4: ���� �ʱ�ȭ
    std::cout << "\nStep 4/4: Initializing buffers..." << std::endl;
    model->initBuffer();
    currentStep++;
    printProgressBar(currentStep, totalSteps);

    // �� �ε� �Ϸ�
    std::cout << "\nModel loaded successfully!" << std::endl;

    if (rigid_status) {
        addModelToPhysicsWorld(model);

        //�浹 ���� �߰��Ϸ�
        std::cout << "\nModel add PhysicsWorld add!" << std::endl;
    }

    model->draw_status = draw_status;

    models.push_back(model);
}
