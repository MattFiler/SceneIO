#pragma once

#include "MaterialParameter.h"

class DynamicMaterial {
public:
	DynamicMaterial(json config);
	~DynamicMaterial() = default;

	std::string GetName() {
		return name;
	}

	MaterialParameter* GetParameter(std::string name) {
		for (int i = 0; i < parameters.size(); i++) {
			if (parameters[i].name == name) {
				return &parameters[i];
			}
		}
		return nullptr;
	}
	MaterialParameter* GetParameter(int index) {
		if (index >= 0 && index < parameters.size()) return &parameters[index];
		return nullptr;
	}

	int GetParameterCount() {
		return parameters.size();
	}

	std::vector<MaterialParameter>* GetParameters() {
		return &parameters;
	}

private:
	std::string name;
	std::vector<MaterialParameter> parameters = std::vector<MaterialParameter>();
};