#pragma once

#include "Utilities.h"
#include "SharedModelPart.h"
#include "GameObjectManager.h"

/* A model data buffer which can be shared between multiple model instances to save memory */
class SharedModelBuffers {
public:
	SharedModelBuffers(std::string filepath);
	~SharedModelBuffers();

	void Render(XMMATRIX mWorld, std::vector<DynamicMaterial>* materials);

	int GetVertCount() {
		return vertexCount;
	}

	int GetUsageCount() {
		return useCount;
	}
	void AddUsage() {
		Debug::Log("Logged new buffer usage.");
		useCount++;
	}
	void RemoveUsage() {
		Debug::Log("Removed log of buffer usage.");
		useCount--;
	}

	std::string GetFilepath() {
		return objPath;
	}

	DirectX::XMFLOAT3 GetExtents() {
		return extents;
	}
	DirectX::XMFLOAT3 GetExtentsOffset() {
		return extentsOffset;
	}

	int GetSubmeshCount() {
		return allModels.size();
	}

	bool DoesRayIntersect(Ray& _r, DirectX::XMMATRIX _world, float& _d);

private:
	void CheckAgainstBoundingPoints(XMFLOAT3 pos) {
		if (pos.x < extents_tempMin.x) extents_tempMin.x = pos.x;
		if (pos.y < extents_tempMin.y) extents_tempMin.y = pos.y;
		if (pos.z < extents_tempMin.z) extents_tempMin.z = pos.z;
		if (pos.x > extents_tempMax.x) extents_tempMax.x = pos.x;
		if (pos.y > extents_tempMax.y) extents_tempMax.y = pos.y;
		if (pos.z > extents_tempMax.z) extents_tempMax.z = pos.z;
	}
	void CalculateFinalExtents() {
		extents = XMFLOAT3(((extents_tempMin.x * -1) + extents_tempMax.x) / 2.0f, ((extents_tempMin.y * -1) + extents_tempMax.y) / 2.0f, ((extents_tempMin.z * -1) + extents_tempMax.z) / 2.0f);
		extentsOffset = XMFLOAT3(extents_tempMax.x + extents_tempMin.x, extents_tempMax.y + extents_tempMin.y, extents_tempMax.z + extents_tempMin.z);
	}
	DirectX::XMFLOAT3 extents_tempMin;
	DirectX::XMFLOAT3 extents_tempMax;
	DirectX::XMFLOAT3 extents;
	DirectX::XMFLOAT3 extentsOffset;

	ID3D11Buffer* g_pVertexBuffer = nullptr;
	ID3D11SamplerState* g_pSamplerLinear = nullptr;

	std::vector<SharedModelPart*> allModels = std::vector<SharedModelPart*>();
	std::vector<SimpleVertex> allVerts = std::vector<SimpleVertex>();

	std::string objPath = "";
	int vertexCount = 0;
	int useCount = 0;
};