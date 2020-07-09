#include "DynamicMaterialManager.h"
#include <fstream>

DynamicMaterialManager::DynamicMaterialManager()
{
	json config;
	std::fstream cmd_js("data/material_config.json");
	cmd_js >> config;

	for (int i = 0; i < config["materials"].size(); i++) {
		materials.emplace_back(config["materials"][i]);
	}
}
