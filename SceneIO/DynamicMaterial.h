#pragma once

#include "MaterialParameter.h"

class DynamicMaterial {
public:
	DynamicMaterial(json config);
	~DynamicMaterial() = default;

	std::string GetName() {
		return materialName;
	}

	MaterialParameter* GetParameter(std::string name) {
		for (int i = 0; i < materialParameters.size(); i++) {
			if (materialParameters[i].name == name) {
				return &materialParameters[i];
			}
		}
		return nullptr;
	}
	MaterialParameter* GetParameter(int index) {
		if (index >= 0 && index < materialParameters.size()) return &materialParameters[index];
		return nullptr;
	}

private:
	std::string materialName;
	std::vector<MaterialParameter> materialParameters = std::vector<MaterialParameter>();
};