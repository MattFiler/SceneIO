#pragma once

#include "DynamicMaterial.h"
#include <vector>
#include <fstream>

/* Manages all defined material types from JSON, loaded in Shared persistently */
class DynamicMaterialManager {
public:
	DynamicMaterialManager() {
		json config;
		std::string configPath = "data/materials/material_config.json";
#ifdef SCENEIO_PLUGIN
		//configPath = "../" + configPath;
#endif
		std::fstream cmd_js(configPath);
		cmd_js >> config;

		for (int i = 0; i < config["materials"].size(); i++) {
			materials.push_back(new DynamicMaterial(config["materials"][i], i));
		}

		if (materials.size() == 0) {
			throw std::out_of_range("ERROR! No materials provided in configuration JSON file. Fatal.");
		}
	}
	~DynamicMaterialManager() = default;

	DynamicMaterial* GetMaterial(std::string name) {
		for (int i = 0; i < materials.size(); i++) {
			if (materials[i]->GetName() == name) {
				return materials[i];
			}
		}
		throw std::out_of_range("ERROR! Incorrect material requested. Fatal.");
	}
	DynamicMaterial* GetMaterial(int index) {
		if (index >= 0 && index < materials.size()) return materials[index];
		throw std::out_of_range("ERROR! Incorrect material requested. Fatal.");
	}

	int GetMaterialCount() {
		return materials.size();
	}

private:
	std::vector<DynamicMaterial*> materials = std::vector<DynamicMaterial*>();
};