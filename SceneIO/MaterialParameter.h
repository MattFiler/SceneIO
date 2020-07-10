#pragma once

#include "DataTypes.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

/* A parameter for a JSON define material */
class MaterialParameter {
public:
	MaterialParameter(json config) {
		name = config["name"].get<std::string>();
		ChangeValueType((DataTypes)config["type"].get<int>());
		bind = config["binding"].get<std::string>();
	}

	/* Change the datatype of this parameter (WILL RESET THE VALUE) */
	void ChangeValueType(DataTypes newType) {
		switch (newType) {
			case DataTypes::RGB: {
				value = new DataTypeRGB();
				break;
			}
			case DataTypes::STRING: {
				value = new DataTypeString();
				break;
			}
			case DataTypes::FLOAT: {
				value = new DataTypeFloat();
				break;
			}
			case DataTypes::INTEGER: {
				value = new DataTypeInt();
				break;
			}
			case DataTypes::UNSIGNED_INTEGER: {
				value = new DataTypeUInt();
				break;
			}
			case DataTypes::BOOLEAN: {
				value = new DataTypeBool();
				break;
			}
			case DataTypes::FLOAT_ARRAY: {
				value = new DataTypeFloatArray();
				break;
			}
		}
		value->type = newType;
	}

	std::string name;
	DataType* value = nullptr;
	std::string bind;
};