#pragma once;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include "../SceneIO/CommonScene.h"
#include "../SceneIO/DynamicMaterialManager.h"
#include "../SceneIO/DynamicMaterial.h"

extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin();
extern "C" __declspec(dllexport) LoadedModel* LoadModel(std::string filePath);
