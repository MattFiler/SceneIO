#include "DynamicMaterialManager.h"
#include <fstream>

/* Initialise materials */
DynamicMaterialManager::DynamicMaterialManager()
{
	json config;
	std::fstream cmd_js("data/material_config.json");
	cmd_js >> config;

	for (int i = 0; i < config["materials"].size(); i++) {
		materials.emplace_back(config["materials"][i]);
	}
}

/* Render material manager UI */
void DynamicMaterialManager::RenderUI()
{
	/*
	//Debug out loaded material types
	for (int i = 0; i < dxshared::materialManager.GetMaterialCount(); i++) {
		Debug::Log("Found material: " + dxshared::materialManager.GetMaterial(i)->GetName());
		for (int x = 0; x < dxshared::materialManager.GetMaterial(i)->GetParameterCount(); x++) {
			Debug::Log("    With parameter: " + dxshared::materialManager.GetMaterial(i)->GetParameter(x)->name);
			Debug::Log("        Of type: " + std::to_string((int)dxshared::materialManager.GetMaterial(i)->GetParameter(x)->value.type));
			Debug::Log("        Binding: " + dxshared::materialManager.GetMaterial(i)->GetParameter(x)->bind);
		}
		Debug::Log("---");
	}
	*/
}
