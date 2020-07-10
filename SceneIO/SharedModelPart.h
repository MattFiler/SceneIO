#pragma once

#include "Utilities.h"
#include "Light.h"
#include "DynamicMaterial.h"

/* A submesh within a SharedModelBuffers object (used per-material type) */
class SharedModelPart {
public:
	SharedModelPart(LoadedModelPart _m);
	~SharedModelPart();

	void Render(XMMATRIX world, DynamicMaterial* material);

private:
	ID3D11Buffer* g_pConstantBuffer = nullptr;
	ID3D11Buffer* g_pIndexBuffer = nullptr;

	LoadedModelPart modelMetaData = LoadedModelPart();

	int indexCount = 0;
};