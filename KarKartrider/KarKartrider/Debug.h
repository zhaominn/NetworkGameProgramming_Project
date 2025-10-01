#pragma once

#include <iostream>
#include <unordered_map>
#include "Model.h"

// --- 선언부만 남김 ---
void debug_model(const Model* model);
void debug_materials(const std::unordered_map<std::string, Material>& materials);
