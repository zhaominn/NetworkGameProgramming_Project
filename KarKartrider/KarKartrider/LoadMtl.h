#pragma once

#include <string>
#include <unordered_map>
#include "Model.h"

// MTL 파일을 읽어와서 재질 정보를 파싱하는 함수 선언
void read_mtl_file(
    const std::string& filename,
    const std::string path,
    std::unordered_map<std::string, Material>& materials
);
