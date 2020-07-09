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

	if (materials.size() > 0) selectedMaterialUI = 0;
}

/* Render material manager UI */
void DynamicMaterialManager::Update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Materials", nullptr);
	ImGui::PopStyleVar();

	if (ImGui::Button("Add New")) {

	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Material Instances", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int i = 0; i < GetMaterialCount(); i++) {
			ImGui::RadioButton(GetMaterial(i)->GetName().c_str(), &selectedMaterialUI, i);
		}
	}

	ImGui::Separator();

	if (selectedMaterialUI != -1) {
		//ImGui::InputText("Name");

		//This should match the DataTypes enum in DataTypes.h
		const char* items[] = { "RGB", "STRING", "FLOAT", "INTEGER", "UNSIGNED_INTEGER", "BOOLEAN", "FLOAT_ARRAY" };
		static int item_current = 0;
		ImGui::Combo("DataType", &item_current, items, IM_ARRAYSIZE(items));
	}

	ImGui::End();


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
