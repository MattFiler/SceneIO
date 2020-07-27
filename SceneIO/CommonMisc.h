#pragma once
#include <string>
#include <vector>

/* Functionality shared between the main application and DLLs */

/* A vector of X and Y values */
struct Vector2 {
	Vector2() {}
	Vector2(float _x, float _y) {
		x = _x;
		y = _y;
	}
	//TODO: more useful addition overloads etc

	float x = 0.0f;
	float y = 0.0f;
};

/* A vector of X, Y, and Z values */
struct Vector3 {
	Vector3() {}
	Vector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}
	//TODO: more useful addition overloads etc

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

/* Plugin definition */
enum class PluginType {
	DUMMY,
	IMPORTER,
	EXPORTER,
	//TODO: UI button thing
};
struct PluginDefinition {
	//Filled in by the editor tool
	std::string pluginPath = ""; 

	//Filled in by the plugin itself
	std::string pluginName = ""; 
	std::vector<std::string> supportedExtensions = std::vector<std::string>();
	PluginType pluginType = PluginType::DUMMY;
	bool supportsScenes = false;

	PluginDefinition() {};
	PluginDefinition(std::string _name, std::vector<std::string> _extensions, PluginType _type, bool _scenes) : pluginName(_name), supportedExtensions(_extensions), pluginType(_type), supportsScenes(_scenes) {};
};