#pragma once

#include "DynamicMaterial.h"
#include <vector>

/* Manages all defined material types from JSON, loaded in Shared persistently */
class DynamicMaterialManager {
public:
	DynamicMaterialManager();
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