#pragma once
#include "CommonMesh.h"

/* Functionality shared between the main application and DLLs */

/* Position and rotation data for the scene's camera */
class SceneCamera {
public:
	SceneCamera() = default;
	SceneCamera(Vector3 _pos, Vector3 _rot, float _fov, bool _usingRadians = true) {
		position = _pos;
		rotation = _rot;
		fov = _fov;
		rotationIsInRadians = _usingRadians;
	}

	Vector3 GetPosition() { return position; }
	Vector3 GetRotation() { return rotation; }
	bool IsRotationInRadians() { return rotationIsInRadians; }
	float GetFOV() { return fov; }

private:
	Vector3 position = Vector3(0,0,0);
	Vector3 rotation = Vector3(0,0,0);
	bool rotationIsInRadians = true;
	float fov = 1.5f;
};

struct LoadedModelPositioner {
	LoadedModel* model = nullptr;
	Vector3 position = Vector3(0, 0, 0);
	Vector3 rotation = Vector3(0, 0, 0);
	bool rotationIsInRadians = true;
};

/* A definition of a scene, containing camera data and model info */
class LoadedScene {
public:
	std::vector<LoadedModelPositioner> modelDefinitions = std::vector<LoadedModelPositioner>();
	SceneCamera camera = SceneCamera();
	DynamicMaterial* environmentMat = nullptr;
};