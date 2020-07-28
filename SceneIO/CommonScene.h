#pragma once
#include "CommonMesh.h"

/* Functionality shared between the main application and DLLs */

/* Position and rotation data for the scene's camera */
class SceneCamera {
public:
	SceneCamera() = default;
	SceneCamera(Vector3 _pos, Vector3 _rot) {
		position = _pos;
		rotationEuler = _rot;
	}

	Vector3 GetPosition() { return position; }
	Vector3 GetRotation() { return rotationEuler; }

private:
	Vector3 position = Vector3(0,0,0);
	Vector3 rotationEuler = Vector3(0,0,0);
};

/* Define a filepath and position/rotation for a model in the scene - also optionally allows you to specify per-submesh materials to override the model's defaults */
class SceneModel {
public:
	SceneModel() = default;
	SceneModel(std::string _path, Vector3 _pos, Vector3 _rot, bool _useMats = false, std::vector<DynamicMaterial*> _mats = std::vector<DynamicMaterial*>()) {
		filepath = _path;
		materials = _mats;
		useMatOverride = _useMats;
		position = _pos;
		rotationEuler = _rot;
	}

	std::string GetFilepath() { return filepath; }
	bool IsUsingMaterialOverride() { return useMatOverride; }
	std::vector<DynamicMaterial*> GetMaterials() { return materials; }
	Vector3 GetPosition() { return position; }
	Vector3 GetRotation() { return rotationEuler; }

private:
	std::string filepath;
	bool useMatOverride = false;
	std::vector<DynamicMaterial*> materials = std::vector<DynamicMaterial*>();
	Vector3 position;
	Vector3 rotationEuler;
};

/* A definition of a scene, containing camera data and models */
class SceneDefinition {
public:
	std::vector<SceneModel> modelDefinitions = std::vector<SceneModel>();
	SceneCamera camera = SceneCamera();
};