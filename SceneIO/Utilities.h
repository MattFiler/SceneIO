#pragma once

#include <windows.h>
#include "Shared.h"
#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

#include <DirectXMath.h>
#include <DirectXColors.h>

#include "Memory.h"
#include "EXErr.h"
#include "FreeImage.h"
#include "InputHandler.h"

#include "PluginManager.h"
#include "CommonMesh.h"

#include <string>
#include <vector>
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

class ConstantBufferRGB {
public:
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

	/* Get the file extension from a filename/path */
	static std::string GetFileExtension(std::string filename)
	{
		const char* file_name = filename.c_str();

		int ext = '.';
		const char* extension = NULL;
		extension = strrchr(file_name, ext);

		if (extension == NULL) return "";
		return extension;
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
};