#pragma once

#include <string>
#include "Model.h"

void read_obj_file(
    const std::string& filename,
    const std::string path,
    Model* model,
    const std::string& name,
    const std::string& type
);
