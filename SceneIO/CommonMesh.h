#pragma once
#include <vector>

class DynamicMaterial;

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
	// This struct doesn't have a destructor as it's used a bit casually throughout the application, and destroying the material
	// each time can easily cause corruptions. For this reason, when getting rid of an instance of this struct for the final
	// time, you should be aware that you need to MANUALLY delete the material object.

	std::vector<SimpleVertex> compVertices = std::vector<SimpleVertex>();
	std::vector<WORD> compIndices = std::vector<WORD>();
	DynamicMaterial* material = nullptr;
};

/* A definition of a mesh object */
struct LoadedModel
{
	std::vector<LoadedModelPart> modelParts = std::vector<LoadedModelPart>();
};