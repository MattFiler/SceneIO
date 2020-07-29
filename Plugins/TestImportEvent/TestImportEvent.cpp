#pragma once
#include "../EventPlugin.h"

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	return new PluginDefinition("OBJ Mesh Importer", PluginType::IMPORT_EVENT);
}

/* Perform changes on import event */
extern "C" __declspec(dllexport) void ImportEvent(LoadedScene * scene)
{
	
}