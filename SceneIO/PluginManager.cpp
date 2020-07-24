#include "PluginManager.h"
#include "Utilities.h"
#include <codecvt>

typedef PluginDefinition* (*pluginApplicationRegister)();
typedef LoadedModel* (*modelImporterPlugin)(std::string filepath);
typedef bool (*modelExporterPlugin)(LoadedModel* model, std::string filepath);

/* Get all available plugins on startup */
PluginManager::PluginManager()
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	for (const auto& entry : std::filesystem::directory_iterator("plugins/")) {
		std::string thisFilepath = entry.path().u8string();
		if (thisFilepath.length() < 4) continue;
		if (thisFilepath.substr(thisFilepath.length() - 4) != ".dll") continue;
		HMODULE hModule = LoadLibraryW(conv.from_bytes(thisFilepath).c_str());
		if (hModule != NULL)
		{
			pluginApplicationRegister RegisterPlugin = (pluginApplicationRegister)GetProcAddress(hModule, "RegisterPlugin");
			if (RegisterPlugin != NULL)
			{
				PluginDefinition* thisPlugin = RegisterPlugin();
				thisPlugin->pluginPath = thisFilepath;
				plugins.push_back(thisPlugin);
			}
		}
		FreeLibrary(hModule);
	}
}

/* Load a model through a DLL */
LoadedModel* PluginManager::LoadModelWithPlugin(std::string filePath)
{
	//Find plugin to handle filetype
	std::string pluginName = "";
	std::string filePathExtension = Utilities::GetFileExtension(filePath);
	for (int i = 0; i < plugins.size(); i++) {
		if (plugins[i]->pluginType != PluginType::IMPORTER) continue;
		for (int x = 0; x < plugins[i]->supportedExtensions.size(); x++) {
			if (plugins[i]->supportedExtensions[x] == filePathExtension) {
				pluginName = plugins[i]->pluginPath;
				break;
			}
		}
	}
	if (pluginName == "") return nullptr;

	//Load with plugin
	LoadedModel* newModel = nullptr;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	HMODULE hModule = LoadLibraryW(conv.from_bytes(pluginName).c_str());
	if (hModule != NULL)
	{
		modelImporterPlugin LoadModel = (modelImporterPlugin)GetProcAddress(hModule, "LoadModel");
		if (LoadModel != NULL)
		{
			newModel = LoadModel(filePath);
		}
	}
	FreeLibrary(hModule);
	return newModel;
}

/* Export a model through a DLL */
bool PluginManager::SaveModelWithPlugin(LoadedModel* model, std::string filepath)
{
	//Find plugin to handle filetype
	std::string pluginName = "";
	std::string filePathExtension = Utilities::GetFileExtension(filepath);
	for (int i = 0; i < plugins.size(); i++) {
		if (plugins[i]->pluginType != PluginType::EXPORTER) continue;
		for (int x = 0; x < plugins[i]->supportedExtensions.size(); x++) {
			if (plugins[i]->supportedExtensions[x] == filePathExtension) {
				pluginName = plugins[i]->pluginPath;
				break;
			}
		}
	}
	if (pluginName == "") return false;

	//Save with plugin
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	bool result = false;
	HMODULE hModule = LoadLibraryW(conv.from_bytes(pluginName).c_str());
	if (hModule != NULL)
	{
		modelExporterPlugin SaveModel = (modelExporterPlugin)GetProcAddress(hModule, "SaveModel");
		if (SaveModel != NULL)
		{
			result = SaveModel(model, filepath);
		}
	}
	FreeLibrary(hModule);
	return result;
}

/* Get a vector of all plugin definitions */
std::vector<PluginDefinition*> PluginManager::GetPlugins()
{
	return plugins;
}

/* Get a vector of all importer plugin definitions */
std::vector<PluginDefinition*> PluginManager::GetImporterPlugins()
{
	std::vector<PluginDefinition*> importers = std::vector<PluginDefinition*>();
	for (int i = 0; i < plugins.size(); i++) {
		if (plugins[i]->pluginType == PluginType::IMPORTER) importers.push_back(plugins[i]);
	}
	return importers;
}

/* Get a vector of all exporter plugin definitions */
std::vector<PluginDefinition*> PluginManager::GetExporterPlugins()
{
	std::vector<PluginDefinition*> exporters = std::vector<PluginDefinition*>();
	for (int i = 0; i < plugins.size(); i++) {
		if (plugins[i]->pluginType == PluginType::EXPORTER) exporters.push_back(plugins[i]);
	}
	return exporters;
}