#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map> // �߰�: ������ ���� ����

#include "Model.h"
#include "LoadMtl.h"

// OBJ ������ �о�ͼ� �� �����͸� �Ľ��ϴ� �Լ�
void read_obj_file(const std::string& filename, const std::string path, Model* model, const std::string& name, const std::string& type) {
    std::ifstream file(path + filename);  // ���� �б� ���� ����
    if (!file.is_open()) {  // ������ ���� ���� ���
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::string mtlFilename;  // MTL ���� �̸�
    std::string currentMaterial;  // ���� ��� ���� ���� �̸�
    std::unordered_map<std::string, Material> materials;  // ������ �����͸� ������ ���� ����

    // ������ �� �پ� �о�� ����, �ؽ�ó ��ǥ, ���� ����, �� �����͸� ó���ϴ� ����
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {  // ���� �����͸� ���� ��
            Vertex vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            model->vertices.push_back(vertex);
        }
        else if (prefix == "vt") {  // �ؽ�ó ��ǥ �����͸� ���� ��
            TextureCoord texCoord;

            ss >> texCoord.u >> texCoord.v;

            // ���� w ���� ������ �߰��� ����
            if (!(ss >> texCoord.w)) {
                texCoord.w = 0.0f; // �⺻���� 0.0���� ����
            }

            model->texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {  // ���� ���� �����͸� ���� ��
            Normal normal;
            ss >> normal.nx >> normal.ny >> normal.nz;
            model->normals.push_back(normal);
        }
        else if (prefix == "usemtl") {  // ���� ����
            ss >> currentMaterial;  // ���� ���� �̸� ������Ʈ
        }
        else if (prefix == "f") {  // Face �����͸� ���� ��
            std::vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;
            unsigned int v, t = 0, n;
            char slash;

            // Face �����͸� ó��
            while (ss >> v) {
                vertexIndices.push_back(v - 1); // OBJ �ε����� 1���� �����ϹǷ� -1
                if (ss.peek() == '/') {
                    ss >> slash;
                    if (ss.peek() != '/') {
                        ss >> t;
                        texCoordIndices.push_back(t - 1); // �ؽ�ó �ε���
                    }
                    else {
                        texCoordIndices.push_back(-1);  // �ؽ�ó�� ���� ��� -1
                    }
                    if (ss.peek() == '/') {
                        ss >> slash >> n;
                        normalIndices.push_back(n - 1); // ���� �ε���
                    }
                }
            }

            // �ﰢ�� Face ó��
            if (vertexIndices.size() == 3) {
                Face face;

                // ���� ó��
                face.v1 = vertexIndices[0];
                face.v2 = vertexIndices[1];
                face.v3 = vertexIndices[2];

                // �ؽ�ó ó��
                face.t1 = (texCoordIndices.size() > 0 && texCoordIndices[0] != -1) ? texCoordIndices[0] : 0;
                face.t2 = (texCoordIndices.size() > 1 && texCoordIndices[1] != -1) ? texCoordIndices[1] : 0;
                face.t3 = (texCoordIndices.size() > 2 && texCoordIndices[2] != -1) ? texCoordIndices[2] : 0;

                // ���� ó��
                face.n1 = normalIndices.size() > 0 ? normalIndices[0] : 0;
                face.n2 = normalIndices.size() > 1 ? normalIndices[1] : 0;
                face.n3 = normalIndices.size() > 2 ? normalIndices[2] : 0;

                face.materialName = currentMaterial;  // ���� ���� �̸� ����
                // Face�� normalFaces �߰�
                model->faces.push_back(face);
            }
            // �簢�� Face ó�� (�� ���� �ﰢ������ ����)
            else if (vertexIndices.size() == 4) {
                // ù ��° �ﰢ�� (v1, v2, v3)
                Face face1;

                face1.v1 = vertexIndices[0];
                face1.v2 = vertexIndices[1];
                face1.v3 = vertexIndices[2];

                face1.t1 = (texCoordIndices.size() > 0 && texCoordIndices[0] != -1) ? texCoordIndices[0] : 0;
                face1.t2 = (texCoordIndices.size() > 1 && texCoordIndices[1] != -1) ? texCoordIndices[1] : 0;
                face1.t3 = (texCoordIndices.size() > 2 && texCoordIndices[2] != -1) ? texCoordIndices[2] : 0;

                face1.n1 = normalIndices.size() > 0 ? normalIndices[0] : 0;
                face1.n2 = normalIndices.size() > 1 ? normalIndices[1] : 0;
                face1.n3 = normalIndices.size() > 2 ? normalIndices[2] : 0;

                face1.materialName = currentMaterial;  // ���� ���� �̸� ����
                model->faces.push_back(face1);

                // �� ��° �ﰢ�� (v1, v3, v4)
                Face face2;

                face2.v1 = vertexIndices[0];
                face2.v2 = vertexIndices[2];
                face2.v3 = vertexIndices[3];

                face2.t1 = (texCoordIndices.size() > 0 && texCoordIndices[0] != -1) ? texCoordIndices[0] : 0;
                face2.t2 = (texCoordIndices.size() > 2 && texCoordIndices[2] != -1) ? texCoordIndices[2] : 0;
                face2.t3 = (texCoordIndices.size() > 3 && texCoordIndices[3] != -1) ? texCoordIndices[3] : 0;

                face2.n1 = normalIndices.size() > 0 ? normalIndices[0] : 0;
                face2.n2 = normalIndices.size() > 2 ? normalIndices[2] : 0;
                face2.n3 = normalIndices.size() > 3 ? normalIndices[3] : 0;

                face2.materialName = currentMaterial;  // ���� ���� �̸� ����
                model->faces.push_back(face2);
            }
        }
        else if (prefix == "mtllib") {  // MTL ���� ����
            ss >> mtlFilename;
        }
    }

    // �� �̸� �� Ÿ�� ����
    model->name = name;
    model->type = type;

    // MTL ���� ó��
    if (!mtlFilename.empty()) {
        read_mtl_file(mtlFilename, path, materials);  // MTL ���� �ε�
        model->materials = materials;          // �ε��� ���� ������ �𵨿� ����
    }

    file.close();  // ���� �ݱ�
}
