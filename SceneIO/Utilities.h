#pragma once

#include <windows.h>
#include "Shared.h"
#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

#include <DirectXMath.h>
#include <DirectXColors.h>

#include "EXErr.h"
#include "FreeImage.h"
#include "InputHandler.h"

#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>

#include <time.h>
#include <random>

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

namespace Memory
{
	/* Safely delete a pointer */
	template <class T> void SafeDelete(T& t)
	{
		if (t)
		{
			delete t;
			t = nullptr;
		}
	}

	/* Safely delete a pointer array */
	template <class T> void SafeDeleteArray(T& t)
	{
		if (t)
		{
			delete[] t;
			t = nullptr;
		}
	}

	/* Safely release a d3d resource */
	template <class T> void SafeRelease(T& t)
	{
		if (t)
		{
			t->Release();
			t = nullptr;
		}
	}
};

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Normal;
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

struct VertexGroup
{
	int v; //Vertex
	int c; //Tex coord
	int n; //Normal
	bool set = false;
};

enum VertReaderType  
{
	VERTEX,
	COORDINATE,
	NORMAL,
};

struct Material
{
	std::string materialName = "";

	float r, g, b, a = 1.0f;
	std::string texturePath = "data/plastic_base.dds"; //placeholder blank texture
};

struct LoadedMaterial
{
	std::string materialName = "";
	ID3D11ShaderResourceView* materialTexture = nullptr;
	XMFLOAT4 materialColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct LoadedModelPart
{
	std::vector<SimpleVertex> compVertices = std::vector<SimpleVertex>();
	std::vector<WORD> compIndices = std::vector<WORD>();
	std::string materialName = ""; //Old compatibility for OBJ importer (to be removed)
};

struct LoadedModel 
{
	std::vector<LoadedModelPart> modelParts = std::vector<LoadedModelPart>();
};

struct Face
{
	std::vector<VertexGroup> verts = std::vector<VertexGroup>(); //Vertices (SHOULD BE 3 - TRIANGULATED)
	std::string materialName = ""; //Material name to link with MTL
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

	/*
	TODO: load a model using a linked library plugin
	static LoadedModel LoadModel(std::string path) {
		LoadedModel newModel = LoadedModel();

		HINSTANCE hDLL;               // Handle to DLL
		LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer
		HRESULT hrReturnVal;

		hDLL = LoadLibrary("MyDLL");
		if (NULL != hDLL)
		{
			lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL, "DLLFunc1");
			if (NULL != lpfnDllFunc1)
			{
				// call the function
				hrReturnVal = lpfnDllFunc1(dwParam1, puParam2);
			}
			else
			{
				// report the error
				hrReturnVal = ERROR_DELAY_LOAD_FAILED;
			}
			FreeLibrary(hDLL);
		}
		else
		{
			hrReturnVal = ERROR_DELAY_LOAD_FAILED;
		}

		return newModel;
	}
	*/

	/* Load a model from an OBJ file and return its indices and vertexes (todo: make it condense the vertex array) */
	static LoadedModel LoadModelFromOBJ(std::string path)
	{
		//Open OBJ
		std::ifstream in(path.c_str());
		if (!in) Debug::Log("Failed to open model file!");

		//Parse the OBJ to vertices/texcoords/normals
		std::vector<XMFLOAT3> verts;
		std::vector<XMFLOAT2> coords;
		std::vector<XMFLOAT3> normals;
		std::vector<Face> faces;
		std::string str;
		std::string materialLibrary = "";
		std::string currentMaterial = "";
		while (std::getline(in, str))
		{
			if (str.size() > 0)
			{
				if (str.length() > 7 && str.substr(0, 7) == "mtllib ")
				{
					materialLibrary = str.substr(7);
				}
				else if (str.length() > 2 && str.substr(0, 2) == "v ")
				{
					str = str.substr(2);
					std::string thisPos = "";
					XMFLOAT3 thisVertPos;
					int vertPosIndex = 0;
					for (int i = 0; i < str.length() + 1; i++)
					{
						if (str[i] == ' ' || i == str.length())
						{
							if (thisPos == "") continue;
							if (vertPosIndex == 0) thisVertPos.x = std::stof(thisPos);
							if (vertPosIndex == 1) thisVertPos.y = std::stof(thisPos);
							if (vertPosIndex == 2) thisVertPos.z = std::stof(thisPos);
							vertPosIndex++;
							thisPos = "";
							continue;
						}
						thisPos += str[i];
					}
					verts.push_back(thisVertPos);
				}
				else if (str.length() > 3 && str.substr(0, 3) == "vt ")
				{
					str = str.substr(3);
					std::string thisPos = "";
					XMFLOAT2 thisTexCoord;
					int texCoordIndex = 0;
					for (int i = 0; i < str.length() + 1; i++)
					{
						if (str[i] == ' ' || i == str.length())
						{
							if (thisPos == "") continue;
							if (texCoordIndex == 0) thisTexCoord.x = std::stof(thisPos);
							if (texCoordIndex == 1) thisTexCoord.y = std::stof(thisPos) * -1;
							texCoordIndex++;
							thisPos = "";
							continue;
						}
						thisPos += str[i];
					}
					coords.push_back(thisTexCoord);
				}
				else if (str.length() > 3 && str.substr(0, 3) == "vn ")
				{
					str = str.substr(3);
					std::string thisPos = "";
					XMFLOAT3 thisNormal;
					int normalIndex = 0;
					for (int i = 0; i < str.length() + 1; i++)
					{
						if (str[i] == ' ' || i == str.length())
						{
							if (thisPos == "") continue;
							if (normalIndex == 0) thisNormal.x = std::stof(thisPos);
							if (normalIndex == 1) thisNormal.y = std::stof(thisPos);
							if (normalIndex == 2) thisNormal.z = std::stof(thisPos);
							normalIndex++;
							thisPos = "";
							continue;
						}
						thisPos += str[i];
					}
					normals.push_back(thisNormal);
				}
				else if (str.length() > 7 && str.substr(0, 7) == "usemtl ")
				{
					currentMaterial = str.substr(7);
				}
				else if (str.length() > 2 && str.substr(0, 2) == "f ")
				{
					str = str.substr(2);
					Face thisFace = Face();
					VertexGroup thisVert = VertexGroup();
					VertReaderType next = VertReaderType::VERTEX;
					std::string currentNumber = "";
					for (int i = 0; i < str.length() + 1; i++) {
						char thisChar = str[i];
						if (thisChar == '/' || thisChar == ' ' || i == str.length())
						{
							if (currentNumber == "") {
								if (verts.size() == 0) Debug::Log("This model has no vertices!");
								if (coords.size() == 0) Debug::Log("This model has no UVs!");
								if (normals.size() == 0) Debug::Log("This model has no normals!");
								continue;
							}
							switch (next) {
							case VertReaderType::VERTEX:
								thisVert.v = std::stoi(currentNumber);
								next = VertReaderType::COORDINATE;
								break;
							case VertReaderType::COORDINATE:
								thisVert.c = std::stoi(currentNumber);
								next = VertReaderType::NORMAL;
								break;
							case VertReaderType::NORMAL:
								thisVert.n = std::stoi(currentNumber);
								next = VertReaderType::VERTEX;
								break;
							}
							thisVert.set = true;
							currentNumber = "";

							if (thisChar == '/') continue;
							if (!thisVert.set) continue;
							thisFace.verts.push_back(thisVert);
							thisVert = VertexGroup();
							continue;
						}
						currentNumber += thisChar;
					}
					if (thisFace.verts.size() != 3) Debug::Log("This model is not triangulated!");
					thisFace.materialName = currentMaterial;
					faces.push_back(thisFace);
				}
			}
		}
		in.close();

		//Open and parse MTL if it exists
		std::vector<Material> materials = std::vector<Material>();
		if (materialLibrary != "")
		{
			//Get model path parts
			std::vector<std::string> modelPath = std::vector<std::string>();
			std::string currentPath = "";
			for (int i = 0; i < path.length(); i++) 
			{
				if (path[i] == '/' || path[i] == '\\')
				{
					modelPath.push_back(currentPath);
					currentPath = "";
					continue;
				}
				currentPath += path[i];
			}
			modelPath.push_back(currentPath);

			//Get material path parts
			std::vector<std::string> mtlPath = std::vector<std::string>();
			currentPath = "";
			for (int i = 0; i < materialLibrary.length(); i++)
			{
				if (materialLibrary[i] == '/' || materialLibrary[i] == '\\')
				{
					mtlPath.push_back(currentPath);
					currentPath = "";
					continue;
				}
				currentPath += materialLibrary[i];
			}
			mtlPath.push_back(currentPath);

			//Reconstruct a good path to MTL using model path
			std::string pathToMtl = "";
			if (mtlPath.size() == 1) 
			{
				if (modelPath.size() == 1) 
				{
					pathToMtl = mtlPath[0];
				}
				else
				{
					for (int i = 0; i < modelPath.size() - 1; i++) 
					{
						pathToMtl += modelPath[i] + "/";
					}
					pathToMtl += mtlPath[0];
				}
			}
			else
			{
				pathToMtl = materialLibrary;
			}

			//Open MTL
			std::ifstream in2(pathToMtl.c_str());
			if (!in2) Debug::Log("Failed to open material file!"); 
			
			//Parse MTL into materials
			std::string str;
			Material currentMaterial = Material();
			while (std::getline(in2, str))
			{
				if (str.size() > 0)
				{
					if (str.length() > 7 && str.substr(0, 7) == "newmtl ")
					{
						if (currentMaterial.materialName != "")
						{
							materials.push_back(currentMaterial);
							currentMaterial = Material();
						}
						currentMaterial.materialName = str.substr(7);
					}
					else if (str.length() > 3 && str.substr(0, 3) == "Kd ")
					{
						str = str.substr(3);
						std::string thisColour = "";
						int thisColourIndex = 0;
						for (int i = 0; i < str.length() + 1; i++)
						{
							if (str[i] == ' ' || i == str.length())
							{
								if (thisColour == "") continue;
								if (thisColourIndex == 0) currentMaterial.r = std::stof(thisColour);
								if (thisColourIndex == 1) currentMaterial.g = std::stof(thisColour);
								if (thisColourIndex == 2) currentMaterial.b = std::stof(thisColour);
								thisColourIndex++;
								thisColour = "";
								continue;
							}
							thisColour += str[i];
						}
					}
					else if (str.length() > 2 && str.substr(0, 2) == "d ")
					{
						currentMaterial.a = std::stof(str.substr(2));
					}
					else if (str.length() > 7 && str.substr(0, 7) == "map_Kd ")
					{
						currentMaterial.texturePath = str.substr(7);
						if (currentMaterial.texturePath[1] == ':') {
							Debug::Log("Texture uses system path! " + currentMaterial.texturePath);
						}
						else
						{
							std::string texPrepend = "";
							for (int i = pathToMtl.length(); i >= 0; i--)
							{
								if (pathToMtl[i] == '/' || pathToMtl[i] == '\\')
								{
									texPrepend = pathToMtl.substr(0, i);
									break;
								}
							}
							currentMaterial.texturePath = texPrepend + "/" + currentMaterial.texturePath;
						}
					}
				}
			}
			if (currentMaterial.materialName != "")
			{
				materials.push_back(currentMaterial);
			}
		}

		//Create vertex and index arrays from the data
		LoadedModel thisModel = LoadedModel();
		LoadedModelPart modelPart = LoadedModelPart();
		int totalIndex = 0;
		for (int i = 0; i < faces.size(); i++)
		{
			for (int x = 0; x < faces[i].verts.size(); x++)
			{
				SimpleVertex thisVertInfo = SimpleVertex();
				thisVertInfo.Pos = verts[faces[i].verts[x].v - 1];
				thisVertInfo.Tex = coords[faces[i].verts[x].c - 1];
				thisVertInfo.Normal = normals[faces[i].verts[x].n - 1];

				if (modelPart.materialName != faces[i].materialName) 
				{
					if (totalIndex != 0)
					{
						thisModel.modelParts.push_back(modelPart);
						modelPart = LoadedModelPart();
					}
					for (int y = 0; y < materials.size(); y++)
					{
						if (materials[y].materialName == faces[i].materialName)
						{
							modelPart.materialName = materials[y].materialName;
							break;
						}
					}
				}

				modelPart.compVertices.push_back(thisVertInfo);
				modelPart.compIndices.push_back((WORD)totalIndex);

				totalIndex++;
			}
		}
		thisModel.modelParts.push_back(modelPart);
		return thisModel;
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
			thisPart.materialName = model.modelParts[x].materialName;
			for (int y = 0; y < model.modelParts[x].compVertices.size(); y++) {
				SimpleVertex thisVertInfo = model.modelParts[x].compVertices[y];
				XMStoreFloat3(&transformedVert, XMVector3Transform(XMLoadFloat3(&model.modelParts[x].compVertices[y].Pos), world));
				thisVertInfo.Pos = transformedVert;
				thisPart.compVertices.push_back(thisVertInfo);
			}
			push_to.modelParts.push_back(thisPart);
		}
	}
};