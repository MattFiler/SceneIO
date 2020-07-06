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
	SharedModelBuffers* LoadModelToLevel(std::string filename);
	std::vector<SharedModelBuffers*> loadedModels = std::vector<SharedModelBuffers*>();
	std::vector<Model*> allActiveModels = std::vector<Model*>(); 

	int selectedEditModel = 0;

	Utilities dxutils = Utilities();

	Camera main_cam;
	Light light_source;
};

