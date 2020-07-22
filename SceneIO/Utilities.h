#pragma once

#include "Shared.h"
#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

#include <DirectXColors.h>

#include "Memory.h"
#include "EXErr.h"
#include "FreeImage.h"
#include "InputHandler.h"

#include "CommonMesh.h"

#include <fstream>
#include <thread>
#include <mutex>
#include <time.h>
#include <random>
#include <iostream>
#include <filesystem>

typedef HRESULT(CALLBACK* LPFNDLLFUNC1)(DWORD, UINT*);

using namespace DirectX;

/* Handle DX HRESULT errors nicely in debug */
#ifdef _DEBUG
	#ifndef HR
		#define HR(x) { HRESULT hr = x; if (FAILED(hr)) { DXTraceW(__FILEW__, __LINE__, hr, L#x, TRUE); } }
	#endif
#else
	#ifndef HR
		#define HR(x) x;
	#endif
#endif

/* Debug logger */
class Debug
{
public:
	static void Log(std::string msg) {
		OutputDebugString(msg.c_str());
		OutputDebugString("\n");
	}
	static void Log(int msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(float msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(double msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(unsigned long msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(size_t msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(DirectX::XMFLOAT3 msg) {
		OutputDebugString(("(X: " + std::to_string(msg.x) + ", Y: " + std::to_string(msg.y) + ", Z: " + std::to_string(msg.z) + ")").c_str());
		OutputDebugString("\n");
	}
};

struct SimpleVertexAlt
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct Ray {
	XMFLOAT3 origin;
	XMFLOAT3 direction;
};

struct Intersection {
	Intersection() = default;
	Intersection(int _i, float _d) {
		entityIndex = _i;
		distance = _d;
	}
	int entityIndex = -1;
	float distance = Shared::cameraFar;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct ConstantBufferAlt
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct ConstantBufferRGB
{
	XMFLOAT4 colourVal = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct Texture {
	~Texture() {
		Memory::SafeRelease(textureView);
		Memory::SafeDelete(textureView);
		Memory::SafeRelease(texture);
		Memory::SafeDelete(texture);
		Memory::SafeDelete(textureBuffer);
	}
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;
	XMFLOAT2 dimensions;
	char* textureBuffer = nullptr;
	std::string texturePath = "";
};

class Utilities
{
public:
	/* Try and compile a shader from file (function created by Microsoft originally) */
	static HRESULT CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
	{
		//Request d3d debugging if in debug
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		dwShaderFlags |= D3DCOMPILE_DEBUG;
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		//Try and compile shader, handle errors
		ID3DBlob* pErrorBlob = nullptr;
		HRESULT hr = S_OK;
		hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
				pErrorBlob->Release();
			}
			return hr;
		}
		if (pErrorBlob) pErrorBlob->Release();

		return S_OK;
	}

	/* Load a model through a DLL */
	typedef LoadedModel* (*modelImporterPlugin)(std::string filepath);
	static LoadedModel* LoadModelWithPlugin(std::wstring pluginName, std::string filePath) 
	{
		LoadedModel* newModel = nullptr;
		HMODULE hModule = LoadLibraryW(pluginName.c_str());
		if (hModule != NULL)
		{
			modelImporterPlugin LoadModel = (modelImporterPlugin)GetProcAddress(hModule, "LoadModel");
			if (LoadModel != NULL)
			{
				newModel = LoadModel(filePath);
			}
		}
		FreeLibrary(hModule);
		return newModel;
	}

	/* Export a model through a DLL */
	typedef void (*modelExporterPlugin)(LoadedModel* model, std::string filepath);
	static void SaveModelWithPlugin(std::wstring pluginName, LoadedModel* model, std::string filepath)
	{
		HMODULE hModule = LoadLibraryW(pluginName.c_str());
		if (hModule != NULL)
		{
			modelExporterPlugin SaveModel = (modelExporterPlugin)GetProcAddress(hModule, "SaveModel");
			if (SaveModel != NULL)
			{
				SaveModel(model, filepath);
			}
		}
		FreeLibrary(hModule);
	}

	/* Get a list of all DLLs for loading */
	static std::vector<std::string> GetImporterPlugins()
	{
		std::vector<std::string> allPlugins = std::vector<std::string>();
		for (const auto& entry : std::filesystem::directory_iterator("import_plugins/")) {
			std::string thisFilepath = entry.path().u8string();
			if (thisFilepath.length() < 4) continue;
			if (thisFilepath.substr(thisFilepath.length() - 4) != ".dll") continue;
			allPlugins.push_back(thisFilepath);
		}
		return allPlugins;
	}

	/* Get a list of all DLLs for exporting */
	static std::vector<std::string> GetExporterPlugins()
	{
		std::vector<std::string> allPlugins = std::vector<std::string>();
		for (const auto& entry : std::filesystem::directory_iterator("export_plugins/")) {
			std::string thisFilepath = entry.path().u8string();
			if (thisFilepath.length() < 4) continue;
			if (thisFilepath.substr(thisFilepath.length() - 4) != ".dll") continue;
			allPlugins.push_back(thisFilepath);
		}
		return allPlugins;
	}

	/* Convert a DirectX Matrix to Float4X4 */
	static XMFLOAT4X4 MatrixToFloat4x4(XMMATRIX mat)
	{
		XMFLOAT4X4 temp;
		XMStoreFloat4x4(&temp, mat);
		return temp;
	}

	/* Add two XMFLOAT3 objects */
	static XMFLOAT3 AddFloat3(XMFLOAT3 a, XMFLOAT3 b)
	{
		return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	/* Multiply two XMFLOAT3 objects */
	static XMFLOAT3 MultiplyFloat3(XMFLOAT3 a, XMFLOAT3 b)
	{
		return XMFLOAT3(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	/* Multiply an XMFLOAT3 by a float */
	static XMFLOAT3 MultiplyFloat3(XMFLOAT3 a, float b)
	{
		return XMFLOAT3(a.x * b, a.y * b, a.z * b);
	}

	/* Load an image to texture using FreeImage */
	static Texture* LoadTexture(std::string filepath)
	{
		const char* filename = filepath.c_str();
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);
		if (format == FREE_IMAGE_FORMAT::FIF_UNKNOWN) return nullptr;
		FIBITMAP* image = FreeImage_Load(format, filename);

		int BPP = FreeImage_GetBPP(image);
		if (BPP != 32) {
			FIBITMAP* tmp = FreeImage_ConvertTo32Bits(image);
			FreeImage_Unload(image);
			image = tmp;
		}
		BPP = FreeImage_GetBPP(image);
		FreeImage_FlipVertical(image);

		Texture* thisTex = new Texture();
		thisTex->texturePath = filepath;
		thisTex->dimensions = XMFLOAT2(FreeImage_GetWidth(image), FreeImage_GetHeight(image));
		int imgLength = thisTex->dimensions.x * thisTex->dimensions.y * 4;
		thisTex->textureBuffer = new char[imgLength];
		memcpy(thisTex->textureBuffer, FreeImage_GetBits(image), imgLength);
		FreeImage_Unload(image);

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = thisTex->dimensions.x;
		desc.Height = thisTex->dimensions.y;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = thisTex->textureBuffer;
		initData.SysMemPitch = thisTex->dimensions.x * 4;
		initData.SysMemSlicePitch = imgLength;
		HR(Shared::m_pDevice->CreateTexture2D(&desc, &initData, &thisTex->texture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = desc.MipLevels - 1;
		HR(Shared::m_pDevice->CreateShaderResourceView(thisTex->texture, &srvDesc, &thisTex->textureView));

		return thisTex;
	}

	/* Copy a file from one location to another */
	static void CopyFile(std::string from, std::string to) {
		std::ifstream  src(from, std::ios::binary);
		std::ofstream  dst(to, std::ios::binary);
		dst << src.rdbuf();
		src.close();
		dst.close();
	}

	/* Allows the user to select a file from a file picker window */
	static std::string OpenFile(std::string filter = "All Files (*.*)\0*.*\0", HWND owner = NULL) {
		OPENFILENAME ofn;
		char fileName[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = owner;
		ofn.lpstrFilter = filter.c_str();
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = "";
		std::string fileNameStr;
		if (GetOpenFileName(&ofn))
			fileNameStr = fileName;
		return fileNameStr;
	}

	/* Transform a loadedmodel by a world matrix, and push it to another loadedmodel */
	static void TransformAndPush(XMMATRIX world, LoadedModel& model, LoadedModel& push_to) {
		XMFLOAT3 transformedVert = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (int x = 0; x < model.modelParts.size(); x++) {
			LoadedModelPart thisPart = LoadedModelPart();
			thisPart.compIndices = model.modelParts[x].compIndices;
			thisPart.material->GetName() = model.modelParts[x].material->GetName();
			for (int y = 0; y < model.modelParts[x].compVertices.size(); y++) {
				SimpleVertex thisVertInfo = model.modelParts[x].compVertices[y];
				XMFLOAT3 originalVert = XMFLOAT3(model.modelParts[x].compVertices[y].Pos.x, model.modelParts[x].compVertices[y].Pos.y, model.modelParts[x].compVertices[y].Pos.z);
				XMVECTOR transformedVert = XMVector3Transform(XMLoadFloat3(&originalVert), world);
				XMFLOAT3 newVert;
				XMStoreFloat3(&newVert, transformedVert);
				thisVertInfo.Pos = Vector3(newVert.x, newVert.y, newVert.z);
				thisPart.compVertices.push_back(thisVertInfo);
			}
			push_to.modelParts.push_back(thisPart);
		}
	}
};