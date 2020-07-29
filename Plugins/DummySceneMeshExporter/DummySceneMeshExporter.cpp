#pragma once
#include "../ExporterPlugin.h"
#include <fstream>

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	//supportedFormats.push_back(".obj");
	return new PluginDefinition("Dummy Exporter (SCENE AND MESH)", PluginType::MODEL_AND_SCENE_EXPORTER, supportedFormats);
}

/* Load a single model */
extern "C" __declspec(dllexport) bool SaveModel(LoadedModel * model, std::string filepath)
{
	return false;
}

/* Save the entire loaded scene data */
extern "C" __declspec(dllexport) bool SaveScene(LoadedScene* scene, std::string filepath)
{
	return false;
}