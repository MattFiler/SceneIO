#pragma once;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include "../SceneIO_Common.h"

extern "C" __declspec(dllexport) LoadedModel* LoadModel(std::string filePath);
