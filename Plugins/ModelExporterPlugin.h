#pragma once;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include "../SceneIO/CommonScene.h"
#include "../SceneIO/DynamicMaterial.h"

extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin();
extern "C" __declspec(dllexport) bool SaveModel(LoadedModel* model, std::string filepath);
