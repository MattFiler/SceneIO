#pragma once
#include "../ButtonPlugin.h"

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	return new PluginDefinition("Test Button", PluginType::UI_BUTTON);
}

/* Perform changes on button press */
extern "C" __declspec(dllexport) void ButtonPress(LoadedScene * scene)
{
	
}