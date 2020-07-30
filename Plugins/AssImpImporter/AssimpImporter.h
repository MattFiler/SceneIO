#pragma once

#include "../ImporterPlugin.h"

#include <fstream>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpImporter {
public:
	AssimpImporter() {
		materialManager = new DynamicMaterialManager();
	}
	~AssimpImporter() {
		if (materialManager) {
			delete materialManager;
			materialManager = nullptr;
		}
	}
	LoadedModel* GetLoadedModel(const aiScene* scene);

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	LoadedModelPart ProcessMesh(aiMesh* mesh, const aiScene* scene);

	DynamicMaterialManager* materialManager = nullptr;
	LoadedModel* thisModel = nullptr;
};