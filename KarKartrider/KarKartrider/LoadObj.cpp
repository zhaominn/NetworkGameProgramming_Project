#include "Pch.h"
#include "loadobj.h"
#include "LoadMtl.h"
#include <vector>


// OBJ ������ �о�ͼ� �� �����͸� �Ľ��ϴ� �Լ� (����)
void read_obj_file(const std::string& filename,
    const std::string path,
    Model* model,
    const std::string& name,
    const std::string& type)
{
    std::ifstream file(path + filename);  // ���� �б� ���� ����
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::string mtlFilename;
    std::string currentMaterial;
    std::unordered_map<std::string, Material> materials;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vertex vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            model->vertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            TextureCoord texCoord;
            ss >> texCoord.u >> texCoord.v;
            if (!(ss >> texCoord.w)) {
                texCoord.w = 0.0f;
            }
            model->texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {
            Normal normal;
            ss >> normal.nx >> normal.ny >> normal.nz;
            model->normals.push_back(normal);
        }
        else if (prefix == "usemtl") {
            ss >> currentMaterial;
        }
        else if (prefix == "f") {
            std::vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;
            unsigned int v, t = 0, n;
            char slash;

            while (ss >> v) {
                vertexIndices.push_back(v - 1);
                if (ss.peek() == '/') {
                    ss >> slash;
                    if (ss.peek() != '/') {
                        ss >> t;
                        texCoordIndices.push_back(t - 1);
                    }
                    else {
                        texCoordIndices.push_back(-1);
                    }
                    if (ss.peek() == '/') {
                        ss >> slash >> n;
                        normalIndices.push_back(n - 1);
                    }
                }
            }

            if (vertexIndices.size() == 3) {
                Face face;
                face.v1 = vertexIndices[0];
                face.v2 = vertexIndices[1];
                face.v3 = vertexIndices[2];

                face.t1 = (texCoordIndices.size() > 0 && texCoordIndices[0] != -1) ? texCoordIndices[0] : 0;
                face.t2 = (texCoordIndices.size() > 1 && texCoordIndices[1] != -1) ? texCoordIndices[1] : 0;
                face.t3 = (texCoordIndices.size() > 2 && texCoordIndices[2] != -1) ? texCoordIndices[2] : 0;

                face.n1 = normalIndices.size() > 0 ? normalIndices[0] : 0;
                face.n2 = normalIndices.size() > 1 ? normalIndices[1] : 0;
                face.n3 = normalIndices.size() > 2 ? normalIndices[2] : 0;

                face.materialName = currentMaterial;
                model->faces.push_back(face);
            }
            else if (vertexIndices.size() == 4) {
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

                face1.materialName = currentMaterial;
                model->faces.push_back(face1);

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

                face2.materialName = currentMaterial;
                model->faces.push_back(face2);
            }
        }
        else if (prefix == "mtllib") {
            ss >> mtlFilename;
        }
    }

    model->name = name;
    model->type = type;

    if (!mtlFilename.empty()) {
        read_mtl_file(mtlFilename, path, materials);
        model->materials = materials;
    }

    file.close();
}
