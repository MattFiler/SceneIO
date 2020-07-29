#pragma once
#include "../ExporterPlugin.h"
#include <fstream>

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	supportedFormats.push_back(".obj");
	return new PluginDefinition("OBJ Scene Exporter", PluginType::SCENE_EXPORTER, supportedFormats);
}

/* Save the entire loaded scene data */
extern "C" __declspec(dllexport) bool SaveScene(LoadedScene* scene, std::string filepath)
{
	
	return true;
}