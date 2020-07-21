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

	//Set and get the shared buffers used by this instance
	void SetSharedBuffers(SharedModelBuffers* _m) {
		modelData = _m;
		for (int i = 0; i < modelData->GetSubmeshCount(); i++) {
			materials.push_back(nullptr);
			SetSubmeshMaterial(i, _m->GetDefaultMaterialIndexes()[i]);
		}
	}
	SharedModelBuffers* GetSharedBuffers() {
		return modelData;
	}

	int GetSubmeshCount() {
		return modelData->GetSubmeshCount();
	}

	//Set & get material data which is important to our API and editor
	void SetSubmeshMaterial(int submeshIndex, int materialIndex) {
		Memory::SafeDelete(materials[submeshIndex]);
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