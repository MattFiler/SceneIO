#pragma once
#include "AssimpImporter.h"

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	supportedFormats.push_back(".obj");
	return new PluginDefinition("AssImp Mesh Importer", PluginType::MODEL_IMPORTER, supportedFormats);
}

/* Load a single model */
extern "C" __declspec(dllexport) LoadedModel* LoadModel(std::string filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) return nullptr;

	AssimpImporter importFunctionality = AssimpImporter();
	LoadedModel* thisModel = importFunctionality.GetLoadedModel(scene);
	thisModel->filepath = filePath;
	return thisModel;
}

/* Produces a LoadedModel object from an AssImp scene */
LoadedModel* AssimpImporter::GetLoadedModel(const aiScene* scene)
{
	thisModel = new LoadedModel(); //We don't clear up the old LoadedModel because this should be handled on the application side.
	ProcessNode(scene->mRootNode, scene);
	return thisModel;
}

/* Process a node in the AssImp scene */
void AssimpImporter::ProcessNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		thisModel->modelParts.push_back(ProcessMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

/* Process a mesh from an AssImp node */
LoadedModelPart AssimpImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	LoadedModelPart modelPart = LoadedModelPart();

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		SimpleVertex vertex;

		vertex.Pos.x = mesh->mVertices[i].x;
		vertex.Pos.y = mesh->mVertices[i].y;
		vertex.Pos.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.Tex.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.Tex.y = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasNormals())
		{
			vertex.Normal.x = (float)mesh->mNormals[i].x;
			vertex.Normal.y = (float)mesh->mNormals[i].y;
			vertex.Normal.z = (float)mesh->mNormals[i].z;
		}

		modelPart.compVertices.push_back(vertex);
	}
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
			modelPart.compIndices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	modelPart.material = materialManager->GetMaterial(0);

	return modelPart;
}