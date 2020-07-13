#pragma once

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
};

class DataType {
public:
	static DataTypes TypeStringToEnum(std::string _t) {
		if (_t == "rgb") return DataTypes::RGB;
		if (_t == "string") return DataTypes::STRING;
		if (_t == "float") return DataTypes::FLOAT;
		if (_t == "int") return DataTypes::INTEGER;
		if (_t == "uint") return DataTypes::UNSIGNED_INTEGER;
		if (_t == "bool") return DataTypes::BOOLEAN;
		if (_t == "array") return DataTypes::FLOAT_ARRAY;
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