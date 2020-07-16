#pragma once
#include "ObjLoader.h"
#include <fstream>

enum VertReaderType
{
	VERTEX,
	COORDINATE,
	NORMAL,
};

struct VertexGroup
{
	int v; //Vertex
	int c; //Tex coord
	int n; //Normal
	bool set = false;
};

struct Face
{
	std::vector<VertexGroup> verts = std::vector<VertexGroup>(); //Vertices (SHOULD BE 3 - TRIANGULATED)
	std::string materialName = ""; //Material name to link with MTL
};

struct Material
{
	std::string materialName = "";

	float r, g, b, a = 1.0f;
	std::string texturePath = "data/plastic_base.dds"; //placeholder blank texture
};

extern "C" __declspec(dllexport) LoadedModel* LoadModel(std::string filePath)
{
	//Open OBJ
	std::ifstream in(filePath.c_str());
	if (!in) return nullptr;

	//Parse the OBJ to vertices/texcoords/normals
	std::vector<Vector3> verts;
	std::vector<Vector2> coords;
	std::vector<Vector3> normals;
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
				Vector3 thisVertPos;
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
				Vector2 thisTexCoord;
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
				Vector3 thisNormal;
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
							//if (verts.size() == 0) Debug::Log("This model has no vertices!");
							//if (coords.size() == 0) Debug::Log("This model has no UVs!");
							//if (normals.size() == 0) Debug::Log("This model has no normals!");
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
				//if (thisFace.verts.size() != 3) Debug::Log("This model is not triangulated!");
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
		for (int i = 0; i < filePath.length(); i++)
		{
			if (filePath[i] == '/' || filePath[i] == '\\')
			{
				modelPath.push_back(currentPath);
				currentPath = "";
				continue;
			}
			currentPath += filePath[i];
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
		//if (!in2) Debug::Log("Failed to open material file!");

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
						//Debug::Log("Texture uses system path! " + currentMaterial.texturePath);
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
	LoadedModel* thisModel = new LoadedModel();
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
					thisModel->modelParts.push_back(modelPart);
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
	thisModel->modelParts.push_back(modelPart);
	return thisModel;
}
