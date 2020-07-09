#pragma once

#include <string>

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
	DataTypes type;
};

class RGBValue
{
public:
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