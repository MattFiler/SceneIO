#pragma once
#include "../ImporterPlugin.h"
#include <fstream>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	supportedFormats.push_back(".obj");
	return new PluginDefinition("AssImp Importer", supportedFormats, PluginType::IMPORTER, false);
}

/* Load a single model */
extern "C" __declspec(dllexport) LoadedModel* LoadModel(std::string filePath)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	DynamicMaterialManager* materialManager = new DynamicMaterialManager();
	LoadedModel* thisModel = new LoadedModel();
	thisModel->filepath = filePath;
	for (UINT x = 0; x < pScene->mRootNode->mNumChildren; x++) {
		aiVector3D scale;
		aiQuaternion rotation;
		aiVector3D translation;
		pScene->mRootNode->mChildren[x]->mTransformation.Decompose(scale, rotation, translation);
		for (int z = 0; z < pScene->mRootNode->mChildren[x]->mNumMeshes; z++) {
			aiMesh* mesh = pScene->mMeshes[pScene->mRootNode->mChildren[x]->mMeshes[z]];
			LoadedModelPart modelPart = LoadedModelPart();

			for (int i = 0; i < mesh->mNumVertices; i++) {
				SimpleVertex thisVert = SimpleVertex();

				//TODO: transform this position by mTransformation matrix
				thisVert.Pos.x = mesh->mVertices[i].x;
				thisVert.Pos.y = mesh->mVertices[i].y;
				thisVert.Pos.z = mesh->mVertices[i].z;

				if (mesh->HasTextureCoords(i)) {
					thisVert.Tex.x = (float)mesh->mTextureCoords[0][i].x;
					thisVert.Tex.y = (float)mesh->mTextureCoords[0][i].y;
				}

				if (mesh->HasNormals()) {
					thisVert.Normal.x = (float)mesh->mNormals[i].x;
					thisVert.Normal.y = (float)mesh->mNormals[i].y;
					thisVert.Normal.z = (float)mesh->mNormals[i].z;
				}

				modelPart.compVertices.push_back(thisVert);
			}

			for (int i = 0; i < mesh->mNumFaces; i++) {
				for (int y = 0; y < mesh->mFaces[i].mNumIndices; y++) {
					modelPart.compIndices.push_back((WORD)mesh->mFaces[i].mIndices[y]);
				}
			}

			//TODO
			if (mesh->mMaterialIndex >= 0) {
				aiMaterial* mat = pScene->mMaterials[mesh->mMaterialIndex];
				for (int p = 0; p < mat->mNumProperties; p++) {
					aiMaterialProperty* prop = mat->mProperties[p];
					//prop->
				}
			}
			//modelPart.material = materialManager->GetMaterial(mesh->mMaterialIndex);
			modelPart.material = materialManager->GetMaterial(0);

			thisModel->modelParts.push_back(modelPart);
		}
	}
	return thisModel;
}

/* Load an entire scene */
extern "C" __declspec(dllexport) SceneDefinition* LoadScene(std::string filePath)
{
	//WE DON'T SUPPORT SCENES WITH THIS PLUGIN
	return nullptr;
}
