#pragma once

#include "Model.h"

void read_mtl_file(
    const std::string& filename,
    const std::string path,
    std::unordered_map<std::string, Material>& materials
);
