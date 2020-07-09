#pragma once

#include "DynamicMaterial.h"
#include <vector>

class DynamicMaterialManager {
public:
	DynamicMaterialManager();
	~DynamicMaterialManager() = default;

	DynamicMaterial* GetMaterial(std::string name) {
		for (int i = 0; i < materials.size(); i++) {
			if (materials[i].GetName() == name) {
				return &materials[i];
			}
		}
		return nullptr;
	}
	DynamicMaterial* GetMaterial(int index) {
		if (index >= 0 && index < materials.size()) return &materials[index];
		return nullptr;
	}

	int GetMaterialCount() {
		return materials.size();
	}

	std::vector<DynamicMaterial>* GetMaterials() {
		return &materials;
	}

private:
	std::vector<DynamicMaterial> materials = std::vector<DynamicMaterial>();
};