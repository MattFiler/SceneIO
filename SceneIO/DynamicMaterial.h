#pragma once

#include "MaterialParameter.h"
#include "Utilities.h"

enum class MaterialSurfaceTypes {
	SURFACE,
	VOLUME,
	ENVIRONMENT,
};

/* A material which can be defined by JSON */
class DynamicMaterial {
public:
	DynamicMaterial(json _config);
	DynamicMaterial(const DynamicMaterial& cpy);
	~DynamicMaterial() = default;

	std::string GetName() {
		return name;
	}

	MaterialSurfaceTypes GetSurfaceType() {
		return type;
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
	void Setup();

	std::string name = "";
	MaterialSurfaceTypes type = MaterialSurfaceTypes::SURFACE;
	std::vector<MaterialParameter> parameters = std::vector<MaterialParameter>();
	json config;
};