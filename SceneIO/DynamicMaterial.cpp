#include "DynamicMaterial.h"

/* Created organically */
DynamicMaterial::DynamicMaterial(json _config, int _index)
{
	config = _config;
	matIndex = _index;
	Setup(false);
}

/* Created from copy */
DynamicMaterial::DynamicMaterial(const DynamicMaterial& cpy)
{
	config = cpy.config;
	m_vertexLayout = cpy.m_vertexLayout;
	m_vertexShader = cpy.m_vertexShader;
	m_pixelShader = cpy.m_pixelShader;
	matIndex = cpy.matIndex;
	Setup(true);
}

/* Initialise the dynamic material */
void DynamicMaterial::Setup(bool _isCopy)
{
	//Setup base material values
	name = config["name"].get<std::string>();
	
	std::string typeString = config["type"].get<std::string>();
	if (typeString == "SURFACE") type = MaterialSurfaceTypes::SURFACE;
	if (typeString == "VOLUME") type = MaterialSurfaceTypes::VOLUME;
	if (typeString == "ENVIRONMENT") type = MaterialSurfaceTypes::ENVIRONMENT;

	for (int i = 0; i < config["parameters"].size(); i++) {
		MaterialParameter* newParam = new MaterialParameter(config["parameters"][i]);
		parameters.push_back(newParam);
	}

	isCopy = _isCopy;
	if (_isCopy) return;

	//Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	std::string s = "data/materials/" + name + ".fx";
	std::wstring stemp = std::wstring(s.begin(), s.end());
	HR(Utilities::CompileShaderFromFile(stemp.c_str(), "VS", "vs_4_0", &pVSBlob));

	//Create the vertex shader
	HR(Shared::m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_vertexShader));

	//Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	//Create the input layout
	HR(Shared::m_pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_vertexLayout));
	pVSBlob->Release();

	//Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	HR(Utilities::CompileShaderFromFile(stemp.c_str(), "PS", "ps_4_0", &pPSBlob));

	//Create the pixel shader
	HR(Shared::m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pixelShader));
	pPSBlob->Release();
}
