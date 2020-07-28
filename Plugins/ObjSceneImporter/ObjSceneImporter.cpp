#pragma once
#include "../SceneImporterPlugin.h"
#include <fstream>
#include <iostream>

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	supportedFormats.push_back(".obj");
	return new PluginDefinition("OBJ Scene Importer", supportedFormats, PluginType::SCENE_IMPORTER);
}

/* Load an entire scene */
extern "C" __declspec(dllexport) SceneDefinition* LoadScene(std::string filePath)
{
	/*
	SceneDefinition* thisScene = new SceneDefinition();
	thisScene->camera = new SceneCamera(Vector3(0, 0, 0), Vector3(0, 0, 0));
	thisScene->modelDefinitions.push_back()
	return thisScene;
	*/
	return nullptr;
}
