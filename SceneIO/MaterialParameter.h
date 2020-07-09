#pragma once

#include "DataTypes.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

class MaterialParameter {
public:
	MaterialParameter(json config) {
		name = config["name"].get<std::string>();
		switch (config["type"].get<int>()) {
			case (int)DataTypes::RGB: {
				type = DataTypeRGB();
				break;
			}
			case (int)DataTypes::STRING: {
				type = DataTypeString();
				break;
			}
			case (int)DataTypes::FLOAT: {
				type = DataTypeFloat();
				break;
			}
			case (int)DataTypes::INTEGER: {
				type = DataTypeInt();
				break;
			}
			case (int)DataTypes::UNSIGNED_INTEGER: {
				type = DataTypeUInt();
				break;
			}
			case (int)DataTypes::BOOLEAN: {
				type = DataTypeBool();
				break;
			}
			case (int)DataTypes::FLOAT_ARRAY: {
				type = DataTypeFloatArray();
				break;
			}
		}
		type.type = (DataTypes)config["type"].get<int>();
		bind = config["binding"].get<std::string>();
	}

	std::string name;
	DataType type;
	std::string bind;
};