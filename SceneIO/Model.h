#pragma once

#include "Utilities.h"
#include "GameObject.h"
#include "SharedModelBuffers.h"
#include "DynamicMaterialManager.h"

/* A model instance, pulling from a shared vertex/index buffer, with bespoke per-submesh material definitions */
class Model : public GameObject {
public:
	~Model() {
		Release();
	}

	void Create() override;
	void Release() override;
	void Update(float dt) override;
	void Render(float dt) override;

	void SetData(SharedModelBuffers* _m) {
		modelData = _m;
		for (int i = 0; i < modelData->GetSubmeshCount(); i++) {
			//Currently we default to index 0, but potentially in future this will be pre-assigned.
			materials.push_back(nullptr);
			SetSubmeshMaterial(i, 0);
		}
	}

	int GetSubmeshCount() {
		return modelData->GetSubmeshCount();
	}

	//Set & get material data which is important to our API and editor
	void SetSubmeshMaterial(int submeshIndex, int materialIndex) {
		//This leads to a memory leak because we aren't clearing up the old DynamicMaterial - TODO: work out what the hell this issue is
		DynamicMaterial* materialTemplate = Shared::materialManager->GetMaterial(materialIndex);
		materials[submeshIndex] = new DynamicMaterial(*materialTemplate);
	}
	DynamicMaterial* GetSubmeshMaterial(int submeshIndex) {
		return materials[submeshIndex];
	}

	bool DoesIntersect(Ray& _r, float& _d);

protected:
	SharedModelBuffers* modelData = nullptr;
	DirectX::BoundingOrientedBox boundingBox;
	std::vector<DynamicMaterial*> materials = std::vector<DynamicMaterial*>();
};