#pragma once

#include "Scene.h"

class EditorScene : public Scene
{
public:
	EditorScene();
	~EditorScene() {
		Release();
	}

	void Init() override;
	void Release() override;

	bool Update(double dt) override;
	void Render(double dt) override;

private:
	// TEST
	void LoadTestModel(std::string name, DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3 rot = DirectX::XMFLOAT3(0,0,0)) {
		Model* new_model = new Model();
		new_model->SetData(LoadModelToLevel(name));
		new_model->SetPosition(pos);
		new_model->SetRotation(rot, true);
		new_model->Create();
		GameObjectManager::AddObject(new_model);
		allActiveModels.push_back(new_model);

		//selectedEditModel = allActiveModels.size() - 1;
	}

	SharedModelBuffers* LoadModelToLevel(std::string filename);
	std::vector<SharedModelBuffers*> loadedModels = std::vector<SharedModelBuffers*>();
	std::vector<Model*> allActiveModels = std::vector<Model*>(); 

	int selectedEditModel = -1;
	float fovCheck = dxshared::cameraFOV;

	Utilities dxutils = Utilities();

	bool testLastFrame = false;
	bool enablePickerTest = false;

	Camera main_cam;
	Light light_source;
};

