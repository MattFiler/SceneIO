#pragma once
#include "../SceneExporterPlugin.h"
#include <fstream>

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	supportedFormats.push_back(".obj");
	return new PluginDefinition("OBJ Scene Exporter", supportedFormats, PluginType::SCENE_EXPORTER);
}

/* Save the entire scene */
extern "C" __declspec(dllexport) bool SaveScene(SceneDefinition * scene, std::string filepath)
{
	
	return true;
}