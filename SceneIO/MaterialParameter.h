#pragma once

#include "DataTypes.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

/* A parameter for a JSON define material */
class MaterialParameter {
public:
	MaterialParameter(json config) {
		name = config["name"].get<std::string>();
		switch (config["type"].get<int>()) {
			case (int)DataTypes::RGB: {
				value = DataTypeRGB();
				break;
			}
			case (int)DataTypes::STRING: {
				value = DataTypeString();
				break;
			}
			case (int)DataTypes::FLOAT: {
				value = DataTypeFloat();
				break;
			}
			case (int)DataTypes::INTEGER: {
				value = DataTypeInt();
				break;
			}
			case (int)DataTypes::UNSIGNED_INTEGER: {
				value = DataTypeUInt();
				break;
			}
			case (int)DataTypes::BOOLEAN: {
				value = DataTypeBool();
				break;
			}
			case (int)DataTypes::FLOAT_ARRAY: {
				value = DataTypeFloatArray();
				break;
			}
		}
		value.type = (DataTypes)config["type"].get<int>();
		bind = config["binding"].get<std::string>();
	}

	std::string name;
	DataType value;
	std::string bind;
};