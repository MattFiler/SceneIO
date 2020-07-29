#pragma once
#include "CommonMisc.h"

class LoadedModel;
class LoadedScene;

class PluginManager {
public:
	PluginManager();
	~PluginManager() {
		for (int i = 0; i < plugins.size(); i++) {
			delete plugins[i];
		}
		plugins.clear();
	}

	LoadedModel* LoadModelWithPlugin(std::string filepath);
	bool SaveModelWithPlugin(LoadedModel* model, std::string filepath);

	LoadedScene* LoadSceneWithPlugin(std::string filepath);
	bool SaveSceneWithPlugin(LoadedScene* scene, std::string filepath);

	void CallImportEventPlugin(LoadedScene* scene, PluginDefinition* plugin);
	void CallButtonPressPlugin(LoadedScene* scene, PluginDefinition* plugin);

	std::vector<PluginDefinition*> GetPlugins();
	std::vector<PluginDefinition*> GetPluginsOfType(PluginType _type);
	std::string GetPluginForExtension(PluginType type, std::string extension);
	
private:
	std::vector<PluginDefinition*> plugins = std::vector<PluginDefinition*>();
};