#pragma once

#include "dxutils.h"
#include "SharedModelPart.h"
#include "GameObjectManager.h"

class SharedModelBuffers {
public:
	SharedModelBuffers(std::string filepath);
	~SharedModelBuffers();

	void Render(XMMATRIX mWorld);

	int GetVertCount() {
		return vertexCount;
	}

	int GetUseageCount() {
		return useCount;
	}
	void AddUseage() {
		useCount++;
	}
	void RemoveUseage() {
		useCount--;
	}

	std::string GetFilepath() {
		return objPath;
	}

	DirectX::XMFLOAT3 GetExtents() {
		return extents;
	}

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
		extents = XMFLOAT3((extents_tempMin.x * -1) + extents_tempMax.x, (extents_tempMin.y * -1) + extents_tempMax.y, (extents_tempMin.z * -1) + extents_tempMax.z);
	}
	DirectX::XMFLOAT3 extents_tempMin;
	DirectX::XMFLOAT3 extents_tempMax;
	DirectX::XMFLOAT3 extents;

	Utilities dxutils = Utilities();

	ID3D11Buffer* g_pVertexBuffer = nullptr;

	ID3D11SamplerState* g_pSamplerLinear = nullptr;
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_vertexLayout = nullptr;

	std::vector<SharedModelPart*> allModels = std::vector<SharedModelPart*>();
	std::vector<SimpleVertex> allVerts = std::vector<SimpleVertex>();

	std::string objPath = "";
	int vertexCount = 0;
	int useCount = 0;
};