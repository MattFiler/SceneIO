#include "DynamicMaterialManager.h"
#include <fstream>
#include "imgui/imgui.h"

/* Initialise materials */
DynamicMaterialManager::DynamicMaterialManager()
{
	json config;
	std::fstream cmd_js("data/material_config.json");
	cmd_js >> config;

	for (int i = 0; i < config["materials"].size(); i++) {
		materials.emplace_back(config["materials"][i]);
	}

	if (materials.size() == 0) {
		throw std::out_of_range("ERROR! No materials provided in configuration JSON file. Fatal.");
	}
}

/* Render material manager UI */
void DynamicMaterialManager::Update()
{
	MaterialManagerUI();
	MaterialConfigUI();

	/*
	Debug::Log("Found material: " + dxshared::materialManager->GetMaterial(i)->GetName());
	for (int x = 0; x < dxshared::materialManager->GetMaterial(i)->GetParameterCount(); x++) {
		Debug::Log("    With parameter: " + dxshared::materialManager->GetMaterial(i)->GetParameter(x)->name);
		Debug::Log("        Of type: " + std::to_string((int)dxshared::materialManager->GetMaterial(i)->GetParameter(x)->value.type));
		Debug::Log("        Binding: " + dxshared::materialManager->GetMaterial(i)->GetParameter(x)->bind);
	}
	Debug::Log("---");
	*/

	/*
	//Debug out loaded material types
	for (int i = 0; i < dxshared::materialManager.GetMaterialCount(); i++) {
		Debug::Log("Found material: " + dxshared::materialManager.GetMaterial(i)->GetName());
		for (int x = 0; x < dxshared::materialManager.GetMaterial(i)->GetParameterCount(); x++) {
			Debug::Log("    With parameter: " + dxshared::materialManager.GetMaterial(i)->GetParameter(x)->name);
			Debug::Log("        Of type: " + std::to_string((int)dxshared::materialManager.GetMaterial(i)->GetParameter(x)->value.type));
			Debug::Log("        Binding: " + dxshared::materialManager.GetMaterial(i)->GetParameter(x)->bind);
		}
		Debug::Log("---");
	}
	*/
}

/* Material manager core UI (list of materials, etc) */
void DynamicMaterialManager::MaterialManagerUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Materials", nullptr);
	ImGui::PopStyleVar();

	if (ImGui::Button("Add New")) {

	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Material Instances", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int i = 0; i < GetMaterialCount(); i++) {
			ImGui::RadioButton(materials[i].GetName().c_str(), &selectedMaterialUI, i);
		}
	}

	ImGui::End();
}

/* Material configuration UI (parameters, etc) */
void DynamicMaterialManager::MaterialConfigUI()
{
	//Only continue if our requested edit index is valid
	if (selectedMaterialUI == -1) return;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Material Config", nullptr);
	ImGui::PopStyleVar();

	ImGui::Text("Selected:");
	ImGui::Separator();
	ImGui::Text(materials[selectedMaterialUI].GetName().c_str());

	ImGui::Separator();
	ImGui::Dummy(ImVec2(15.0f, 15.0f));
	ImGui::Separator();

	//This should match the DataTypes enum in DataTypes.h
	const char* items[] = { "RGB", "STRING", "FLOAT", "INTEGER", "UNSIGNED_INTEGER", "BOOLEAN", "FLOAT_ARRAY" };

	if (materials[selectedMaterialUI].GetParameterCount() != 0) {
		ImGui::Text("Parameters:");
		ImGui::Separator();
	}
	for (int i = 0; i < materials[selectedMaterialUI].GetParameterCount(); i++) {
		ImGui::Text(("(" + std::to_string(i) + ") Name: " + materials[selectedMaterialUI].GetParameter(i)->name).c_str());
		int valueType = (int)materials[selectedMaterialUI].GetParameter(i)->value.type;
		ImGui::Combo(("(" + std::to_string(i) + ") DataType").c_str(), &valueType, items, IM_ARRAYSIZE(items));
		ImGui::Text(("(" + std::to_string(i) + ") Binding: " + materials[selectedMaterialUI].GetParameter(i)->bind).c_str());
		ImGui::Separator();
	}

	ImGui::End();
}
