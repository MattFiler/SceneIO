#pragma once
#include "../ImporterPlugin.h"
#include <fstream>
#include <iostream>

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	supportedFormats.push_back(".obj");
	return new PluginDefinition("OBJ Scene Importer", PluginType::SCENE_IMPORTER, supportedFormats);
}

/* Load an entire scene */
extern "C" __declspec(dllexport) LoadedScene* LoadScene(std::string filePath)
{
	DynamicMaterialManager* materialManager = new DynamicMaterialManager();

	LoadedScene* thisScene = new LoadedScene();
	thisScene->camera = SceneCamera(Vector3(0, 0, 0), Vector3(0, 0, 0), 1.5f);
	thisScene->environmentMat = materialManager->GetEnvironmentMaterial(); //Tweak properties as appropriate
	//thisScene->modelDefinitions.push_back()
	return thisScene;
}
