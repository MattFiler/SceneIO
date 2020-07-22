#pragma once

#include "DataTypes.h"
#include "Memory.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

/* A parameter for a JSON define material */
class MaterialParameter {
public:
	MaterialParameter(json config) {
		//Setup base info
		name = config["name"].get<std::string>();
		DataTypes type = DataType::TypeStringToEnum(config["type"].get<std::string>());
		ChangeValueType(type);
		isBound = config["is_bound"].get<bool>();

		//The options parameter takes in its options from the definition
		if (type == DataTypes::OPTIONS_LIST) {
			DataTypeOptionsList* valueOptions = static_cast<DataTypeOptionsList*>(value);
			for (int i = 0; i < config["values"].size(); i++) {
				valueOptions->options.push_back(config["values"][i].get<std::string>());
			}
		}
	}
	~MaterialParameter() {
		Release();
	}

	/* Change the datatype of this parameter (WILL RESET THE VALUE) */
	void ChangeValueType(DataTypes newType) {
		Release();
		switch (newType) {
			case DataTypes::RGB: {
				value = new DataTypeRGB();
				break;
			}
			case DataTypes::TEXTURE_FILEPATH: {
				value = new DataTypeTextureFilepath();
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
			case DataTypes::OPTIONS_LIST: {
				value = new DataTypeOptionsList();
				break;
			}
		}
		value->type = newType;

		#ifndef SCENEIO_PLUGIN
		value->SetupBindable(); //Not all datatypes are bindable, but this is safe to call for any for ease of use.
		#endif
	}

	std::string name = "";
	DataType* value = nullptr;
	bool isBound = false;

private:
	void Release() {
		if (value == nullptr) return;
		switch (value->type) {
			#ifndef SCENEIO_PLUGIN
			case DataTypes::RGB: {
				DataTypeRGB* valueCast = static_cast<DataTypeRGB*>(value);
				Memory::SafeDelete(valueCast);
				break;
			}
			case DataTypes::TEXTURE_FILEPATH: {
				DataTypeTextureFilepath* valueCast = static_cast<DataTypeTextureFilepath*>(value);
				Memory::SafeDelete(valueCast);
				break;
			}
			#endif
			default: {
				Memory::SafeDelete(value);
				break;
			}
		}
	}
};