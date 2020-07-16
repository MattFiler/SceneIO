#include "DynamicMaterialManager.h"
#include <fstream>
#include "imgui/imgui.h"
#include "Shared.h"

/* Initialise materials */
DynamicMaterialManager::DynamicMaterialManager()
{
	json config;
	std::fstream cmd_js("data/materials/material_config.json");
	cmd_js >> config;

	for (int i = 0; i < config["materials"].size(); i++) {
		materials.push_back(new DynamicMaterial(config["materials"][i], i));
	}

	if (materials.size() == 0) {
		throw std::out_of_range("ERROR! No materials provided in configuration JSON file. Fatal.");
	}
}