#pragma once
#include "../ButtonPlugin.h"

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	return new PluginDefinition("Test Button (shift scene by 1,1,1)", PluginType::UI_BUTTON);
}

/* Perform changes on button press */
extern "C" __declspec(dllexport) void ButtonPress(LoadedScene* scene)
{
	//do some stuff with the scene
	for (int i = 0; i < scene->modelDefinitions.size(); i++) {
		scene->modelDefinitions[i].position = scene->modelDefinitions[i].position + Vector3(1, 1, 1);
	}
}