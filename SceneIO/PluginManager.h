#pragma once
#include "CommonMisc.h"

class LoadedModel;
class SceneDefinition;

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

	SceneDefinition* LoadSceneWithPlugin(std::string filepath);
	bool SaveSceneWithPlugin(SceneDefinition* scene, std::string filepath);

	std::vector<PluginDefinition*> GetPlugins();
	std::vector<PluginDefinition*> GetModelImporterPlugins();
	std::vector<PluginDefinition*> GetModelExporterPlugins();
	std::vector<PluginDefinition*> GetSceneImporterPlugins();
	std::vector<PluginDefinition*> GetSceneExporterPlugins();
	
private:
	std::vector<PluginDefinition*> GetPluginsByType(PluginType _type);
	std::string GetPluginForExtension(PluginType type, std::string extension);
	std::vector<PluginDefinition*> plugins = std::vector<PluginDefinition*>();
};