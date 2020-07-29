#pragma once
#include "../ExporterPlugin.h"
#include <fstream>

/* Register the plugin with the application */
extern "C" __declspec(dllexport) PluginDefinition* RegisterPlugin()
{
	std::vector<std::string> supportedFormats = std::vector<std::string>();
	supportedFormats.push_back(".obj");
	return new PluginDefinition("OBJ Mesh Exporter", PluginType::MODEL_EXPORTER, supportedFormats);
}

/* Save a single model */
extern "C" __declspec(dllexport) bool SaveModel(LoadedModel* model, std::string filepath)
{
	std::ofstream objFile;
	objFile.open(filepath);
	std::ofstream mtlFile;
	std::string mtlPath = filepath.substr(0, filepath.length() - 3) + "mtl";
	mtlFile.open(mtlPath);
	if (!objFile.is_open()) return false;
	if (!mtlFile.is_open()) return false;
	objFile << "#### AUTOGENERATED BY OBJ DLL ####\n";
	objFile << "mtllib " << mtlPath << "\n";
	mtlFile << "#### AUTOGENERATED BY OBJ DLL ####\n";
	std::vector<std::string> parsedMats = std::vector<std::string>();
	int offset = 0;
	for (int i = 0; i < model->modelParts.size(); i++) {
		//OBJ
		objFile << "o part_" << std::to_string(i) << "\n";
		for (int x = 0; x < model->modelParts[i].compVertices.size(); x++) {
			Vector3 thisVert = model->modelParts[i].compVertices[x].Pos;
			objFile << "v " << std::to_string(thisVert.x) << " " << std::to_string(thisVert.y) << " " << std::to_string(thisVert.z) << "\n";
		}
		for (int x = 0; x < model->modelParts[i].compVertices.size(); x++) {
			Vector2 thisCoord = model->modelParts[i].compVertices[x].Tex;
			objFile << "vt " << std::to_string(thisCoord.x) << " " << std::to_string(thisCoord.y * -1) << "\n";
		}
		for (int x = 0; x < model->modelParts[i].compVertices.size(); x++) {
			Vector3 thisNorm = model->modelParts[i].compVertices[x].Normal;
			objFile << "vn " << std::to_string(thisNorm.x) << " " << std::to_string(thisNorm.y) << " " << std::to_string(thisNorm.z) << "\n";
		}
		objFile << "usemtl " << model->modelParts[i].material->GetName() << "\n";
		for (int x = 0; x < model->modelParts[i].compIndices.size(); x += 3) {
			objFile << "f " << model->modelParts[i].compIndices[x] + 1 + offset << "/" << model->modelParts[i].compIndices[x] + 1 + offset << "/" << model->modelParts[i].compIndices[x] + 1 + offset << " ";
			objFile << model->modelParts[i].compIndices[x + 1] + 1 + offset << "/" << model->modelParts[i].compIndices[x + 1] + 1 + offset << "/" << model->modelParts[i].compIndices[x + 1] + 1 + offset << " ";
			objFile << model->modelParts[i].compIndices[x + 2] + 1 + offset << "/" << model->modelParts[i].compIndices[x + 2] + 1 + offset << "/" << model->modelParts[i].compIndices[x + 2] + 1 + offset << "\n";
		}
		offset += model->modelParts[i].compIndices[model->modelParts[i].compIndices.size() - 1] + 1;

		//MTL
		bool alreadyExported = false;
		for (int x = 0; x < parsedMats.size(); x++) {
			if (parsedMats[x] == model->modelParts[i].material->GetName()) {
				alreadyExported = true;
				break;
			}
		}
		if (alreadyExported) continue;
		parsedMats.push_back(model->modelParts[i].material->GetName());
		mtlFile << "\nnewmtl " << model->modelParts[i].material->GetName() << "\n";
		RGBValue rgbVal = static_cast<DataTypeRGB*>(model->modelParts[i].material->GetParameter("albedoColour")->value)->value;
		mtlFile << "Kd " << rgbVal.R << " " << rgbVal.G << " " << rgbVal.B << "\n";
		mtlFile << "d \"" << static_cast<DataTypeString*>(model->modelParts[i].material->GetParameter("albedoTexture")->value)->value << "\"\n";
		mtlFile << "map_Kd \"" << static_cast<DataTypeString*>(model->modelParts[i].material->GetParameter("albedoTexture")->value)->value << "\"\n";
		mtlFile << "illum 2\n";
	}
	objFile.close();
	mtlFile.close();
	return true;
}