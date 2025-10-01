#pragma once

#include <string>
#include "Model.h"

// OBJ 파일을 읽어와서 모델 데이터를 파싱하는 함수 (선언만)
void read_obj_file(
    const std::string& filename,
    const std::string path,
    Model* model,
    const std::string& name,
    const std::string& type
);
