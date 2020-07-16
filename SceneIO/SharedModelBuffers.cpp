#include "SharedModelBuffers.h"

/* Load a model and create the buffers */
SharedModelBuffers::SharedModelBuffers(std::wstring plugin, std::string filepath)
{
	//Push data for our vertex buffer, and create children index buffers
	Debug::Log("Loading model from disk.");
	objPath = filepath;
	LoadedModel* _m = Utilities::LoadModelWithPlugin(plugin, filepath);
	if (_m == nullptr) {
		Debug::Log("Failed to load model! May be a plugin error, or file could not exist.");
		return;
	}
	for (int i = 0; i < _m->modelParts.size(); i++) {
		for (int x = 0; x < _m->modelParts[i].compVertices.size(); x++) {
			CheckAgainstBoundingPoints(XMFLOAT3(_m->modelParts[i].compVertices[x].Pos.x, _m->modelParts[i].compVertices[x].Pos.y, _m->modelParts[i].compVertices[x].Pos.z));
			allVerts.push_back(_m->modelParts[i].compVertices[x]);
		}
		allModels.push_back(new SharedModelPart(_m->modelParts[i]));
		defaultMatIndexes.push_back(_m->modelParts[i].materialIndex);
	}
	Memory::SafeDelete(_m);
	CalculateFinalExtents();

	//Create vertex buffer 
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	vertexCount = allVerts.size();
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = allVerts.data();
	HR(Shared::m_pDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer));

	//Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(Shared::m_pDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear));
}

/* Clear all buffers */
SharedModelBuffers::~SharedModelBuffers()
{
	Memory::SafeRelease(g_pVertexBuffer);
	for (int i = 0; i < allModels.size(); i++) {
		Memory::SafeDelete(allModels[i]);
	}
	allModels.clear();
	allVerts.clear();
}

/* Render the model parts */
void SharedModelBuffers::Render(XMMATRIX mWorld, std::vector<DynamicMaterial*> materials)
{
	//Validate: this will cause an error spam - and for good reason!
	if (vertexCount == 0) {
		Debug::Log("WARNING: Model buffer is not rendering - no vertices.");
		return;
	}
	if (allModels.size() == 0) {
		Debug::Log("WARNING: Model buffer is not rendering - no submeshes.");
		return;
	}
	if (materials.size() != allModels.size()) {
		Debug::Log("WARNING: Model buffer is not rendering - incorrect number of materials passed.");
		return;
	}

	//Set vertex buffer 
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	Shared::m_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//Set sampler
	Shared::m_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);

	//Render each model part
	for (int i = 0; i < allModels.size(); i++) {
		materials.at(i)->SetShader();
		allModels[i]->Render(mWorld, materials.at(i));
	}
}

/* Perform an expensive ray-triangle check */
bool SharedModelBuffers::DoesRayIntersect(Ray& _r, DirectX::XMMATRIX _world, float& _d)
{
	for (int i = 0; i < allVerts.size(); i += 3) {
		if (DirectX::TriangleTests::Intersects(DirectX::XMLoadFloat3(&_r.origin), DirectX::XMLoadFloat3(&_r.direction),
			DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&XMFLOAT3(allVerts[i].Pos.x, allVerts[i].Pos.y, allVerts[i].Pos.z)), _world),
			DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&XMFLOAT3(allVerts[i + 1].Pos.x, allVerts[i + 1].Pos.y, allVerts[i + 1].Pos.z)), _world),
			DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&XMFLOAT3(allVerts[i + 2].Pos.x, allVerts[i + 2].Pos.y, allVerts[i + 2].Pos.z)), _world), _d)) {
			return true;
		}
	}
	return false;
}
