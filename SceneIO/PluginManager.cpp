#include "PluginManager.h"
#include "Utilities.h"
#include <codecvt>

typedef PluginDefinition* (*pluginApplicationRegister)();
typedef LoadedModel* (*modelImporterPlugin)(std::string filepath);
typedef bool (*modelExporterPlugin)(LoadedModel* model, std::string filepath);
typedef SceneDefinition* (*sceneImporterPlugin)(std::string filepath);
typedef bool (*sceneExporterPlugin)(SceneDefinition* scene, std::string filepath);

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
LoadedModel* PluginManager::LoadModelWithPlugin(std::string filepath)
{
	//Find plugin to handle filetype
	std::string pluginName = GetPluginForExtension(PluginType::MODEL_IMPORTER, Utilities::GetFileExtension(filepath));
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
			newModel = LoadModel(filepath);
		}
	}
	FreeLibrary(hModule);
	return newModel;
}

/* Export a model through a DLL */
bool PluginManager::SaveModelWithPlugin(LoadedModel* model, std::string filepath)
{
	//Find plugin to handle filetype
	std::string pluginName = GetPluginForExtension(PluginType::MODEL_EXPORTER, Utilities::GetFileExtension(filepath));
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

/* Load a scene definition through a DLL */
SceneDefinition* PluginManager::LoadSceneWithPlugin(std::string filepath)
{
	//Find plugin to handle filetype
	std::string pluginName = GetPluginForExtension(PluginType::SCENE_IMPORTER, Utilities::GetFileExtension(filepath));
	if (pluginName == "") return nullptr;

	//Load with plugin
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	SceneDefinition* result = nullptr;
	HMODULE hModule = LoadLibraryW(conv.from_bytes(pluginName).c_str());
	if (hModule != NULL)
	{
		sceneImporterPlugin LoadScene = (sceneImporterPlugin)GetProcAddress(hModule, "LoadScene");
		if (LoadScene != NULL)
		{
			result = LoadScene(filepath);
		}
	}
	FreeLibrary(hModule);
	return result;
}

/* Export a scene definition through a DLL */
bool PluginManager::SaveSceneWithPlugin(SceneDefinition* scene, std::string filepath)
{
	//Find plugin to handle filetype
	std::string pluginName = GetPluginForExtension(PluginType::SCENE_EXPORTER, Utilities::GetFileExtension(filepath));
	if (pluginName == "") return false;

	//Save with plugin
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	bool result = false;
	HMODULE hModule = LoadLibraryW(conv.from_bytes(pluginName).c_str());
	if (hModule != NULL)
	{
		sceneExporterPlugin SaveScene = (sceneExporterPlugin)GetProcAddress(hModule, "SaveScene");
		if (SaveScene != NULL)
		{
			result = SaveScene(scene, filepath);
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
std::vector<PluginDefinition*> PluginManager::GetModelImporterPlugins()
{
	return GetPluginsByType(PluginType::MODEL_IMPORTER);
}

/* Get a vector of all exporter plugin definitions */
std::vector<PluginDefinition*> PluginManager::GetModelExporterPlugins()
{
	return GetPluginsByType(PluginType::MODEL_EXPORTER);
}

/* Get a vector of all importer plugin definitions that support scenes */
std::vector<PluginDefinition*> PluginManager::GetSceneImporterPlugins()
{
	return GetPluginsByType(PluginType::SCENE_IMPORTER);
}

/* Get a vector of all exporter plugin definitions that support scenes */
std::vector<PluginDefinition*> PluginManager::GetSceneExporterPlugins()
{
	return GetPluginsByType(PluginType::SCENE_EXPORTER);
}

/* Get all plugins that match the given type */
std::vector<PluginDefinition*> PluginManager::GetPluginsByType(PluginType _type)
{
	std::vector<PluginDefinition*> importers = std::vector<PluginDefinition*>();
	for (int i = 0; i < plugins.size(); i++) {
		if (plugins[i]->pluginType == _type) importers.push_back(plugins[i]);
	}
	return importers;
}

/* Get a plugin path that supports a given extension for a type (returns empty string if none exist) */
std::string PluginManager::GetPluginForExtension(PluginType type, std::string extension)
{
	std::string pluginName = "";
	for (int i = 0; i < plugins.size(); i++) {
		if (plugins[i]->pluginType != type) continue;
		for (int x = 0; x < plugins[i]->supportedExtensions.size(); x++) {
			if (plugins[i]->supportedExtensions[x] == extension) {
				pluginName = plugins[i]->pluginPath;
				break;
			}
		}
	}
	return pluginName;
}
