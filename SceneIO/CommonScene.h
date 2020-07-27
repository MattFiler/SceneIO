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
	Vector3 position;
	Vector3 rotationEuler;
};

#if defined SCENEIO_PLUGIN_IMPORTER || defined SCENEIO_CORE_APPLICATION
/* Define a filepath and position/rotation for a model in the scene */
class SceneModel {
public:
	SceneModel() = default;
	SceneModel(std::string _path, Vector3 _pos, Vector3 _rot) {
		filepath = _path;
		position = _pos;
		rotationEuler = _rot;
	}

	std::string GetFilepath() { return filepath; }
	Vector3 GetPosition() { return position; }
	Vector3 GetRotation() { return rotationEuler; }

private:
	std::string filepath;
	Vector3 position;
	Vector3 rotationEuler;
};
#endif

/* A definition of a scene, containing camera data and models */
class SceneDefinition {
public:
	~SceneDefinition() {
#ifndef SCENEIO_PLUGIN
		for (int i = 0; i < loadedModels.size(); i++) {
			delete loadedModels[i];
		}
		loadedModels.clear();
		delete camera;
		camera = nullptr;
#endif
	}

#if defined SCENEIO_PLUGIN_EXPORTER || defined SCENEIO_CORE_APPLICATION
	std::vector<LoadedModel*> loadedModels = std::vector<LoadedModel*>();
#endif
#if defined SCENEIO_PLUGIN_IMPORTER || defined SCENEIO_CORE_APPLICATION
	std::vector<SceneModel> modelDefinitions = std::vector<SceneModel>();
#endif
	SceneCamera* camera = nullptr;
};