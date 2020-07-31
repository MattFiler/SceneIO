#pragma once
#include "Utilities.h"
#include "Model.h"
#include "SharedModelBuffers.h"
#include "DynamicMaterialManager.h"

/* Manages all model instances within the scene */
class ModelManager {
public:
	ModelManager();
	~ModelManager();

	void Update(double dt);

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
	void ModelMaterialUI();
	void MaterialDropdownUI(Model* model, int submeshID);

	bool LoadModel(std::string name, DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3 rot = DirectX::XMFLOAT3(0, 0, 0));
	bool SaveModel(std::string name);

	bool LoadScene(std::string name);
	bool SaveScene(std::string name);

	void CommonImportEvent();

	LoadedScene* GetSceneAsLoadedScene();
	bool SetSceneAsLoadedScene(LoadedScene* scene);

	SharedModelBuffers* LoadModelToLevel(std::string filename);
	SharedModelBuffers* LoadModelToLevel(LoadedModel* modelref);
	std::vector<SharedModelBuffers*> modelBuffers = std::vector<SharedModelBuffers*>();
	std::vector<Model*> models = std::vector<Model*>();

	std::vector<SharedModelBuffers*> modelBuffersSanity = std::vector<SharedModelBuffers*>(); //DO NOT USE - TEMP DEALLOC CHECK
	float deallocCheckAfter = 10.0f;
	float timeSinceDeallocCheck = 0.0f;

	int selectedModelUI = -1;
	int selectedModelSubmeshUI = -1;

	bool shouldExportAsWorld = false;

	bool didIOThisFrame = false;

	ImGui::FileBrowser modelImporterFileDialog;
	ImGui::FileBrowser modelExporterFileDialog;

	ImGui::FileBrowser sceneImporterFileDialog;
	ImGui::FileBrowser sceneExporterFileDialog;

	ImGui::FileBrowser textureSelectFileDialog;
	int currentTextureSelectIndex = -1;
};