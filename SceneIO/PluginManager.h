#pragma once
#include <string>
#include <vector>

class PluginDefinition;
class LoadedModel;

class PluginManager {
public:
	PluginManager();
	~PluginManager() {
		for (int i = 0; i < plugins.size(); i++) {
			delete plugins[i];
		}
		plugins.clear();
	}

	LoadedModel* LoadModelWithPlugin(std::string filePath);
	bool SaveModelWithPlugin(LoadedModel* model, std::string filepath);

	std::vector<PluginDefinition*> GetPlugins();
	std::vector<PluginDefinition*> GetImporterPlugins();
	std::vector<PluginDefinition*> GetExporterPlugins();
	
private:
	std::vector<PluginDefinition*> plugins = std::vector<PluginDefinition*>();
};