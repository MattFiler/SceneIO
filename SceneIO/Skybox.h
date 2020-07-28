#pragma once

#include "Utilities.h"
#include "GameObject.h"

/* A model instance, pulling from a shared vertex/index buffer, with bespoke per-submesh material definitions */
class Skybox : public GameObject {
public:
	~Skybox() {
		Release();
	}

	void Create() override;
	void Release() override;
	void Update(float dt) override;
	void Render(float dt) override;

protected:
	int LatLines = 10;
	int LongLines = 10;

	ID3D11Buffer* sphereIndexBuffer;
	ID3D11Buffer* sphereVertBuffer;

	ID3D11VertexShader* SKYMAP_VS;
	ID3D11PixelShader* SKYMAP_PS;
	ID3D10Blob* SKYMAP_VS_Buffer;
	ID3D10Blob* SKYMAP_PS_Buffer;

	ID3D11ShaderResourceView* smrv;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	XMMATRIX sphereWorld;
};