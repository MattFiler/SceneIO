#pragma once;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include "../SceneIO/CommonMesh.h"
#include "../SceneIO/DynamicMaterialManager.h"
#include "../SceneIO/DynamicMaterial.h"

extern "C" __declspec(dllexport) LoadedModel* LoadModel(std::string filePath);