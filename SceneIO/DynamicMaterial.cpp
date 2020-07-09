#include "DynamicMaterial.h"

DynamicMaterial::DynamicMaterial(json config)
{
	materialName = config["name"].get<std::string>();

	for (int i = 0; i < config["parameters"].size(); i++) {
		materialParameters.emplace_back(config["parameters"][i]);
	}
}