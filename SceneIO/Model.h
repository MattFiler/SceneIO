#pragma once

#include "Utilities.h"
#include "GameObject.h"
#include "SharedModelBuffers.h"
#include "DynamicMaterialManager.h"

/* A model instance, pulling from a shared vertex/index buffer */
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
			materials.push_back(Shared::materialManager->GetMaterial(0));
		}
	}

	int GetSubmeshCount() {
		return modelData->GetSubmeshCount();
	}

	void SetSubmeshMaterial(int submeshIndex, DynamicMaterial materialType) {
		materials[submeshIndex] = materialType;
	}

	bool DoesIntersect(Ray& _r, float& _d);

protected:
	SharedModelBuffers* modelData = nullptr;
	DirectX::BoundingOrientedBox boundingBox;
	std::vector<DynamicMaterial> materials = std::vector<DynamicMaterial>();
};