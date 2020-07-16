#pragma once
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

/* A definition of a mesh vertex */
struct SimpleVertex
{
	Vector3 Pos;
	Vector2 Tex;
	Vector3 Normal;
};

/* A definition of a submesh within a mesh object */
struct LoadedModelPart
{
	std::vector<SimpleVertex> compVertices = std::vector<SimpleVertex>();
	std::vector<WORD> compIndices = std::vector<WORD>();
	std::string materialName = ""; //Old compatibility for OBJ importer (to be removed)
	int materialIndex = 0;
};

/* A definition of a mesh object */
struct LoadedModel
{
	std::vector<LoadedModelPart> modelParts = std::vector<LoadedModelPart>();
};