#pragma once

#include "Model.h"

// MTL ������ �о�ͼ� ���� ������ �Ľ��ϴ� �Լ� ����
void read_mtl_file(
    const std::string& filename,
    const std::string path,
    std::unordered_map<std::string, Material>& materials
);
