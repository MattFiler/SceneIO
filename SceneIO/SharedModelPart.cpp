#include "SharedModelPart.h"
#include "DataTypes.h"

/* Create the model part (a child to SharedModelBuffers) */
SharedModelPart::SharedModelPart(LoadedModelPart _m)
{
	modelMetaData = _m;

	//Create index buffer 
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	indexCount = modelMetaData.compIndices.size();
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = modelMetaData.compIndices.data();
	HR(Shared::m_pDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer));

	//Create the constant buffer 
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR(Shared::m_pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer));
}

/* Destroy our model part */
SharedModelPart::~SharedModelPart()
{
	Memory::SafeRelease(g_pIndexBuffer);
	Memory::SafeRelease(g_pConstantBuffer);
}

/* Render our model part */
void SharedModelPart::Render(XMMATRIX world, DynamicMaterial* material)
{
	//Update and set constant buffer
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(Shared::mView);
	cb.mProjection = XMMatrixTranspose(Shared::mProjection);
	DataTypeRGB* rgbVal = static_cast<DataTypeRGB*>(&material->GetParameter("Albedo")->value);
	cb.colourTint = XMFLOAT4(rgbVal->value.R, rgbVal->value.G, rgbVal->value.B, 1.0f);
	//cb.numOfLights = (LightManager::GetLightCount() > 10) ? 10 : LightManager::GetLightCount();
	//for (int i = 0; i < 10; i++) {
		//if (i >= LightManager::GetLightCount()) break;
		XMFLOAT3 pos = GameObjectManager::GetLights()[0]->GetPosition();
		cb.pointlightPosition = XMFLOAT4(pos.x, pos.y, pos.z, GameObjectManager::GetLights()[0]->GetIntensity());
		cb.pointlightColour = GameObjectManager::GetLights()[0]->GetColour();
	//}
	cb.ambientLight = Shared::ambientLightColour;
	Shared::m_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	Shared::m_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	Shared::m_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	//Set texture
	//Shared::m_pImmediateContext->PSSetShaderResources(0, 1, &loadedMaterial.materialTexture);

	//Set index buffer and draw
	Shared::m_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	Shared::m_pImmediateContext->DrawIndexed(indexCount, 0, 0);
}
