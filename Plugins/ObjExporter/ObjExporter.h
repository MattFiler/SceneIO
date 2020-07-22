#pragma once;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include "../../SceneIO/CommonMesh.h"
#include "../../SceneIO/DynamicMaterial.h"

extern "C" __declspec(dllexport) void SaveModel(LoadedModel* model, std::string filepath);
