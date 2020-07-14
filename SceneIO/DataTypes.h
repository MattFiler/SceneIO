#pragma once

#include <vector>
#include <string>
#include <stdexcept>

/* Inheritable datatypes for dynamic casting */

//This should match the values and order in Shared.cpp
enum class DataTypes {
	RGB,
	STRING,
	FLOAT,
	INTEGER,
	UNSIGNED_INTEGER,
	BOOLEAN,
	FLOAT_ARRAY,
	OPTIONS_LIST,
};

class DataType {
public:
	static DataTypes TypeStringToEnum(std::string _t) {
		if (_t == "RGB") return DataTypes::RGB;
		if (_t == "STRING") return DataTypes::STRING;
		if (_t == "FLOAT") return DataTypes::FLOAT;
		if (_t == "INTEGER") return DataTypes::INTEGER;
		if (_t == "UNSIGNED_INTEGER") return DataTypes::UNSIGNED_INTEGER;
		if (_t == "BOOLEAN") return DataTypes::BOOLEAN;
		if (_t == "FLOAT_ARRAY") return DataTypes::FLOAT_ARRAY;
		if (_t == "OPTIONS_LIST") return DataTypes::OPTIONS_LIST;
		throw new std::invalid_argument("Could not match type string to enum value!");
	}
	DataTypes type;
};

class RGBValue
{
public:
	float* AsFloatArray() {
		float* toReturn[3] = { &R, &G, &B };
		return *toReturn;
	}

	float R = 0.0f;
	float G = 0.0f;
	float B = 0.0f;
};

class DataTypeRGB : public DataType {
public:
	RGBValue value = RGBValue();
};

class DataTypeString : public DataType {
public:
	std::string value = "";
};

class DataTypeFloat : public DataType {
public:
	float value = 0.0f;
};

class DataTypeInt : public DataType {
public:
	int value = 0;
};

class DataTypeUInt : public DataType {
public:
	uint32_t value = 0;
};

class DataTypeBool : public DataType {
public:
	bool value = false;
};

class DataTypeFloatArray : public DataType {
public:
	float* value[];
};

class DataTypeOptionsList : public DataType {
public:
	int value = 0;
	std::vector<std::string> options = std::vector<std::string>();
};