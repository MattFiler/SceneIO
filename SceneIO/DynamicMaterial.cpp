#include "DynamicMaterial.h"

/* Initialise the dynamic material */
DynamicMaterial::DynamicMaterial(json config)
{
	name = config["name"].get<std::string>();

	for (int i = 0; i < config["parameters"].size(); i++) {
		parameters.emplace_back(config["parameters"][i]);
	}
}