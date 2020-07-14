#include "DynamicMaterial.h"

/* Created organically */
DynamicMaterial::DynamicMaterial(json _config)
{
	config = _config;
	Setup();
}

/* Created from copy */
DynamicMaterial::DynamicMaterial(const DynamicMaterial& cpy)
{
	config = cpy.config;
	Setup();
}

/* Initialise the dynamic material */
void DynamicMaterial::Setup()
{
	name = config["name"].get<std::string>();
	
	std::string typeString = config["type"].get<std::string>();
	if (typeString == "SURFACE") type = MaterialSurfaceTypes::SURFACE;
	if (typeString == "VOLUME") type = MaterialSurfaceTypes::VOLUME;
	if (typeString == "ENVIRONMENT") type = MaterialSurfaceTypes::ENVIRONMENT;

	for (int i = 0; i < config["parameters"].size(); i++) {
		parameters.emplace_back(config["parameters"][i]);
	}
}
