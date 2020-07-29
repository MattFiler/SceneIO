#pragma once
#include "../ImporterPlugin.h"
#include <fstream>
#include <iostream>

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	//supportedFormats.push_back(".obj");
	return new PluginDefinition("Dummy Importer (SCENE AND MESH)", PluginType::MODEL_AND_SCENE_IMPORTER, supportedFormats);
}

/* Load an entire scene */
extern "C" __declspec(dllexport) LoadedModel* LoadModel(std::string filepath)
{
	return nullptr;
}

/* Load an entire scene */
extern "C" __declspec(dllexport) LoadedScene* LoadScene(std::string filePath)
{
	return nullptr;
}
