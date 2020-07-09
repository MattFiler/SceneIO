#pragma once
#include "Utilities.h"
#include "Model.h"
#include "SharedModelBuffers.h"

/* Manages all model instances within the scene */
class ModelManager {
public:
	ModelManager() = default;
	~ModelManager();

	void Update();

	Model* GetModel(int index) {
		if (index >= 0 && index < models.size()) return models[index];
		return nullptr;
	}

	int GetModelCount() {
		return models.size();
	}

	std::vector<Model*>* GetModels() {
		return &models;
	}

	void SelectModel(Ray& _r);

private:
	void ModelManagerUI();
	void ModelTransformUI();

	void LoadTestModel(std::string name, DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3 rot = DirectX::XMFLOAT3(0, 0, 0));

	SharedModelBuffers* LoadModelToLevel(std::string filename);
	std::vector<SharedModelBuffers*> modelBuffers = std::vector<SharedModelBuffers*>();
	std::vector<Model*> models = std::vector<Model*>();

	int selectedModelUI = -1;

	std::string requestedLoadPath = "";
	bool didRequestLoad = false;
};