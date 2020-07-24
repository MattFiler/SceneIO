#pragma once
#include "CommonMisc.h"

class DynamicMaterial;

/* Functionality shared between the main application and DLLs */

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