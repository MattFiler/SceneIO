#include "ModelManager.h"
#include "imgui/imgui.h"
#include "InputHandler.h"
#include "Camera.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <codecvt>
#include <math.h>

/* Setup available types in filepicker */
ModelManager::ModelManager()
{
	std::vector<PluginDefinition*> mI = Shared::pluginManager->GetPluginsOfType(PluginType::MODEL_IMPORTER);
	std::vector<PluginDefinition*> mE = Shared::pluginManager->GetPluginsOfType(PluginType::MODEL_EXPORTER);
	std::vector<PluginDefinition*> sI = Shared::pluginManager->GetPluginsOfType(PluginType::SCENE_IMPORTER);
	std::vector<PluginDefinition*> sE = Shared::pluginManager->GetPluginsOfType(PluginType::SCENE_EXPORTER);
	std::vector<PluginDefinition*> msI = Shared::pluginManager->GetPluginsOfType(PluginType::MODEL_AND_SCENE_IMPORTER);
	std::vector<PluginDefinition*> msE = Shared::pluginManager->GetPluginsOfType(PluginType::MODEL_AND_SCENE_EXPORTER);

	//Model import
	modelImporterFileDialog = ImGui::FileBrowser::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc);
	modelImporterFileDialog.SetTitle("Import Mesh");
	std::vector<const char*> filetypes = std::vector<const char*>();
	for (int i = 0; i < mI.size(); i++) for (int x = 0; x < mI[i]->supportedExtensions.size(); x++) filetypes.push_back(mI[i]->supportedExtensions[x].c_str());
	for (int i = 0; i < msI.size(); i++) for (int x = 0; x < msI[i]->supportedExtensions.size(); x++) filetypes.push_back(msI[i]->supportedExtensions[x].c_str());
	modelImporterFileDialog.SetTypeFilters(filetypes);

	//Model export
	modelExporterFileDialog = ImGui::FileBrowser::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir | ImGuiFileBrowserFlags_CloseOnEsc);
	modelExporterFileDialog.SetTitle("Export Mesh");
	filetypes.clear();
	for (int i = 0; i < mE.size(); i++) for (int x = 0; x < mE[i]->supportedExtensions.size(); x++) filetypes.push_back(mE[i]->supportedExtensions[x].c_str());
	for (int i = 0; i < msE.size(); i++) for (int x = 0; x < msE[i]->supportedExtensions.size(); x++) filetypes.push_back(msE[i]->supportedExtensions[x].c_str());
	modelExporterFileDialog.SetTypeFilters(filetypes);

	//Scene import
	sceneImporterFileDialog = ImGui::FileBrowser::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc);
	sceneImporterFileDialog.SetTitle("Import Scene");
	filetypes.clear();
	for (int i = 0; i < sI.size(); i++) for (int x = 0; x < sI[i]->supportedExtensions.size(); x++) filetypes.push_back(sI[i]->supportedExtensions[x].c_str());
	for (int i = 0; i < msI.size(); i++) for (int x = 0; x < msI[i]->supportedExtensions.size(); x++) filetypes.push_back(msI[i]->supportedExtensions[x].c_str());
	sceneImporterFileDialog.SetTypeFilters(filetypes);

	//Scene export
	sceneExporterFileDialog = ImGui::FileBrowser::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir | ImGuiFileBrowserFlags_CloseOnEsc);
	sceneExporterFileDialog.SetTitle("Export Scene");
	filetypes.clear();
	for (int i = 0; i < sE.size(); i++) for (int x = 0; x < sE[i]->supportedExtensions.size(); x++) filetypes.push_back(sE[i]->supportedExtensions[x].c_str());
	for (int i = 0; i < msE.size(); i++) for (int x = 0; x < msE[i]->supportedExtensions.size(); x++) filetypes.push_back(msE[i]->supportedExtensions[x].c_str());
	sceneExporterFileDialog.SetTypeFilters(filetypes);

	//Texture selection
	textureSelectFileDialog = ImGui::FileBrowser::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc);
	textureSelectFileDialog.SetTitle("Texture Selection");
}

/* Free all model instances */
ModelManager::~ModelManager()
{
	for (int i = 0; i < models.size(); i++) {
		Memory::SafeRelease(models[i]);
	}
	models.clear();
	for (int i = 0; i < modelBuffers.size(); i++) {
		Memory::SafeDelete(modelBuffers[i]);
	}
	modelBuffers.clear();
	Shared::textureManager->UnloadAll();
}

/* Render controls UI */
void ModelManager::Update(double dt)
{
	//Enable/disable input based on the model manager UI visibility (nicer UX)
	InputHandler::EnableInput(!(modelImporterFileDialog.IsOpened() || modelExporterFileDialog.IsOpened()));

	//Hotkeys for swapping modes (TODO: add rotation back)
	if (InputHandler::KeyDown(WindowsKey::O)) {
		Shared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	else if (InputHandler::KeyDown(WindowsKey::P)) {
		Shared::mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	//Deallocate from the buffer pool after a time of inactivity
	timeSinceDeallocCheck += dt;
	if (timeSinceDeallocCheck >= deallocCheckAfter) {
		modelBuffersSanity.clear();
		for (int i = 0; i < modelBuffers.size(); i++) {
			if (modelBuffers[i]->GetUsageCount() <= 0) {
				Memory::SafeDelete(modelBuffers[i]);
			}
			else
			{
				modelBuffersSanity.push_back(modelBuffers[i]);
			}
		}
		if (modelBuffersSanity.size() != modelBuffers.size()) {
			Debug::Log("Successfully deallocated " + std::to_string(modelBuffers.size() - modelBuffersSanity.size()) + " model buffer(s).");
			modelBuffers = modelBuffersSanity;
		}
		timeSinceDeallocCheck = 0.0f;
	}

	//UI
	ModelManagerUI();
	if (didIOThisFrame) {
		didIOThisFrame = false;
		GameObjectManager::Update(dt);
	}
	ModelTransformUI();
	ModelMaterialUI();

	Shared::textureManager->Update(dt);
}

/* Model manager core UI (list model instances, add new models, etc) */
void ModelManager::ModelManagerUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Content Controls", nullptr);
	ImGui::PopStyleVar();

	//Import new model
	if (ImGui::Button("Load New Model")) {
		modelImporterFileDialog.Open();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	modelImporterFileDialog.Display();
	ImGui::PopStyleVar();
	if (modelImporterFileDialog.IsOpened() && modelImporterFileDialog.HasSelected())
	{
		if (LoadModel(modelImporterFileDialog.GetSelected().string())) {
			modelImporterFileDialog.ClearSelected();
			modelImporterFileDialog.Close();
		}
	}

	ImGui::Separator();

	//Controls for selected model only
	if (selectedModelUI != -1) {
		if (ImGui::Button("Export Selected Model")) {
			modelExporterFileDialog.Open();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
		modelExporterFileDialog.Display();
		ImGui::PopStyleVar();
		if (modelExporterFileDialog.IsOpened() && modelExporterFileDialog.HasSelected())
		{
			if (SaveModel(modelExporterFileDialog.GetSelected().string())) {
				modelExporterFileDialog.ClearSelected();
				modelExporterFileDialog.Close();
			}
		}
		ImGui::SameLine();

		ImGui::Checkbox("Use Global Transform", &shouldExportAsWorld);

		if (ImGui::Button("Delete Selected Model")) {
			GameObjectManager::RemoveObject(models.at(selectedModelUI));
			delete models.at(selectedModelUI);
			models.erase(models.begin() + selectedModelUI);
			selectedModelUI -= 1;
		}
		ImGui::SameLine();

		if (ImGui::Button("Duplicate Selected Model")) {
			Model* duplicatedModel = new Model(*models.at(selectedModelUI));
			models.push_back(duplicatedModel);
			GameObjectManager::AddObject(duplicatedModel);
			selectedModelUI = models.size() - 1;
			CommonImportEvent();
		}

		ImGui::Separator();
	}

	//Import new content from scene definition
	if (ImGui::Button("Load Scene")) {
		sceneImporterFileDialog.Open();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	sceneImporterFileDialog.Display();
	ImGui::PopStyleVar();
	if (sceneImporterFileDialog.IsOpened() && sceneImporterFileDialog.HasSelected())
	{
		if (LoadScene(sceneImporterFileDialog.GetSelected().string())) {
			sceneImporterFileDialog.ClearSelected();
			sceneImporterFileDialog.Close();
		}
	}

	ImGui::SameLine();

	//Export full scene
	if (ImGui::Button("Save Scene")) {
		sceneExporterFileDialog.Open();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	sceneExporterFileDialog.Display();
	ImGui::PopStyleVar();
	if (sceneExporterFileDialog.IsOpened() && sceneExporterFileDialog.HasSelected())
	{
		if (SaveScene(sceneExporterFileDialog.GetSelected().string())) {
			sceneExporterFileDialog.ClearSelected();
			sceneExporterFileDialog.Close();
		}
	}

	ImGui::Separator();

	//Show all model instances in a list
	if (ImGui::CollapsingHeader("Model Instances", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int i = 0; i < models.size(); i++) {
			ImGui::RadioButton((std::to_string(i) + ": " + models[i]->GetSharedBuffers()->GetFilepath()).c_str(), &selectedModelUI, i);
		}
	}

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Plugin Buttons", nullptr);
	ImGui::PopStyleVar();

	//Send scene definition to UI button plugin, and then re-apply it back
	std::vector<PluginDefinition*> uiPlugins = Shared::pluginManager->GetPluginsOfType(PluginType::UI_BUTTON);
	for (int i = 0; i < uiPlugins.size(); i++) {
		if (ImGui::Button(("Plugin " + std::to_string(i) + ": " + uiPlugins[i]->pluginName).c_str())) {
			LoadedScene* sceneDefinition = GetSceneAsLoadedScene();
			Shared::pluginManager->CallButtonPressPlugin(sceneDefinition, uiPlugins[i]);
			SetSceneAsLoadedScene(sceneDefinition);
			didIOThisFrame = true;
		}
	}

	ImGui::End();
}

/* Model transform UI (control rotation/scale/position of selected model) */
void ModelManager::ModelTransformUI()
{
	//Only continue if our requested edit object is valid
	if (selectedModelUI == -1) return;

	//Get the GameObject we're editing
	GameObject* objectToEdit = models.at(selectedModelUI);

	//Get matrices as float arrays
	float* objectMatrix = &objectToEdit->GetWorldMatrix4X4().m[0][0];
	float* projMatrix = &Utilities::MatrixToFloat4x4(Shared::mProjection).m[0][0];
	float* viewMatrix = &Utilities::MatrixToFloat4x4(Shared::mView).m[0][0];

	//Show options to swap between different transforms
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Model Transform Control", nullptr);
	ImGui::PopStyleVar();

	if (ImGui::RadioButton("Translate Gizmo", Shared::mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		Shared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	/* if (ImGui::RadioButton("Rotate Gizmo", Shared::mCurrentGizmoOperation == ImGuizmo::ROTATE))
		Shared::mCurrentGizmoOperation = ImGuizmo::ROTATE;*/
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale Gizmo", Shared::mCurrentGizmoOperation == ImGuizmo::SCALE))
		Shared::mCurrentGizmoOperation = ImGuizmo::SCALE;

	//Allow swap between local/world
	/*
	if (Shared::mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", Shared::mCurrentGizmoMode == ImGuizmo::LOCAL))
			Shared::mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", Shared::mCurrentGizmoMode == ImGuizmo::WORLD))
			Shared::mCurrentGizmoMode = ImGuizmo::WORLD;
	}*/

	//If user isn't clicking on a mesh, allow gizmo control
	if (!(InputHandler::KeyDown(WindowsKey::SHIFT) && InputHandler::MouseDown(WindowsMouse::LEFT_CLICK)))
	{
		//Draw manipulation control
		ImGuizmo::SetRect(0, 0, Shared::m_renderWidth, Shared::m_renderHeight);
		ImGuizmo::Manipulate(viewMatrix, projMatrix, Shared::mCurrentGizmoOperation, Shared::mCurrentGizmoMode, objectMatrix, NULL, NULL);
	}

	//Get values from manipulation
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);

	//We don't allow gizmo editing of rotation, as ImGuizmo's accuracy really sucks, and throws everything off
	matrixRotation[0] = objectToEdit->GetRotation(false).x;
	matrixRotation[1] = objectToEdit->GetRotation(false).y;
	matrixRotation[2] = objectToEdit->GetRotation(false).z;

	ImGui::Separator();

	//Allow text overwrite
	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	ImGui::InputFloat3("Rotation", matrixRotation, 3);
	ImGui::InputFloat3("Scale", matrixScale, 3);

	//Set new transforms back
	objectToEdit->SetPosition(DirectX::XMFLOAT3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
	float test[3] = { objectToEdit->GetRotation(false).x, objectToEdit->GetRotation(false).y, objectToEdit->GetRotation(false).z };
	objectToEdit->SetRotation(DirectX::XMFLOAT3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));
	objectToEdit->SetScale(DirectX::XMFLOAT3(matrixScale[0], matrixScale[1], matrixScale[2]));

	/*
	DirectX::XMVECTOR positionv, rotationv, scalev;
	DirectX::XMMatrixDecompose(&scalev, &rotationv, &positionv, objectToEdit->GetWorldMatrix());
	DirectX::XMFLOAT3 position, scale; DirectX::XMFLOAT4 rotation;

	DirectX::XMStoreFloat3(&position, positionv);
	Vec3 test = Vec3(position.x, position.y, position.z);
	ImGui::gizmo3D("Position", test);
	position = XMFLOAT3(test.x, test.y, test.z);
	objectToEdit->SetPosition(position);

	DirectX::XMStoreFloat3(&scale, scalev);
	Vec3 test2 = Vec3(scale.x, scale.y, scale.z);
	ImGui::gizmo3D("Scale", test2);
	scale = XMFLOAT3(test2.x, test2.y, test2.z);
	objectToEdit->SetScale(scale);

	DirectX::XMStoreFloat4(&rotation, rotationv);
	Quat test3 = Quat(rotation.w, rotation.x, rotation.y, rotation.z);
	ImGui::gizmo3D("Rotation", test3);
	rotation = XMFLOAT4(test3.x, test3.y, test3.z, test3.w);
	rotationv = DirectX::XMLoadFloat4(&rotation);
	float roll = atan2(2 * rotation.y * rotation.w - 2 * rotation.x * rotation.z, 1 - 2 * rotation.y * rotation.y - 2 * rotation.z * rotation.z);
	float pitch = atan2(2 * rotation.x * rotation.w - 2 * rotation.y * rotation.z, 1 - 2 * rotation.x * rotation.x - 2 * rotation.z * rotation.z);
	float yaw = asin(2 * rotation.x * rotation.y + 2 * rotation.z * rotation.w);
	objectToEdit->SetRotation(XMFLOAT3(roll, pitch, yaw), true);
	*/

	ImGui::End();
}

/* Model material UI (change selected model's material parameters) */
void ModelManager::ModelMaterialUI()
{
	//Only continue if our requested edit object is valid
	if (selectedModelUI == -1) return;

	//Get the GameObject we're editing
	Model* objectToEdit = models.at(selectedModelUI);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Model Material Control", nullptr);
	ImGui::PopStyleVar();

	for (int i = 0; i < objectToEdit->GetSubmeshCount(); i++) {
		if (ImGui::CollapsingHeader(("Submesh " + std::to_string(i)).c_str())) {
			MaterialDropdownUI(objectToEdit, i);
		}
	}

	ImGui::End();
}
void ModelManager::MaterialDropdownUI(Model* model, int submeshID)
{
	DynamicMaterial* thisMaterial = model->GetSubmeshMaterial(submeshID);
	if (thisMaterial->GetParameterCount() == 0) return;

	//Allow change of material type
	if (ImGui::BeginCombo(("S" + std::to_string(submeshID) + " MaterialType").c_str(), thisMaterial->GetName().c_str())) {
		for (int x = 0; x < Shared::materialManager->GetMaterialCount(); x++) {
			const bool is_selected = (Shared::materialManager->GetMaterial(x)->GetName() == thisMaterial->GetName());
			if (ImGui::Selectable(Shared::materialManager->GetMaterial(x)->GetName().c_str(), is_selected)) {
				model->SetSubmeshMaterial(submeshID, x);
				thisMaterial = model->GetSubmeshMaterial(submeshID);
			}
			if (is_selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Separator();

	//Allow parameter tweaks
	for (int i = 0; i < thisMaterial->GetParameterCount(); i++) {
		MaterialParameter* thisParam = thisMaterial->GetParameter(i);
		std::string inputLabel = ("S" + std::to_string(submeshID) + " " + thisParam->name);
		switch (thisParam->value->type) {
			case DataTypes::RGB: {
				DataTypeRGB* param = static_cast<DataTypeRGB*>(thisParam->value);
				ImGui::ColorEdit3(inputLabel.c_str(), param->value.AsFloatArray());
				break;
			}
			case DataTypes::TEXTURE_FILEPATH: {
				DataTypeTextureFilepath* param = static_cast<DataTypeTextureFilepath*>(thisParam->value);
				if (ImGui::Button((inputLabel + " Select").c_str())) {
					textureSelectFileDialog.Open();
					currentTextureSelectIndex = i;
				}
				ImGui::SameLine();
				if (ImGui::Button((inputLabel + " Reset").c_str())) {
					static_cast<DataTypeTextureFilepath*>(thisParam->value)->value = "";
				}
				break;
			}
			case DataTypes::STRING: {
				DataTypeString* param = static_cast<DataTypeString*>(thisParam->value);
				ImGui::InputText(inputLabel.c_str(), &param->value);
				break;
			}
			case DataTypes::FLOAT: {
				DataTypeFloat* param = static_cast<DataTypeFloat*>(thisParam->value);
				ImGui::InputFloat(inputLabel.c_str(), &param->value);
				break;
			}
			case DataTypes::INTEGER: {
				DataTypeInt* param = static_cast<DataTypeInt*>(thisParam->value);
				ImGui::InputInt(inputLabel.c_str(), &param->value);
				break;
			}
			case DataTypes::UNSIGNED_INTEGER: {
				DataTypeUInt* param = static_cast<DataTypeUInt*>(thisParam->value);
				int toEdit = (int)param->value;
				ImGui::InputInt(inputLabel.c_str(), &toEdit);
				param->value = (uint32_t)toEdit;
				break;
			}
			case DataTypes::BOOLEAN: {
				DataTypeBool* param = static_cast<DataTypeBool*>(thisParam->value);
				ImGui::Checkbox(inputLabel.c_str(), &param->value);
				break;
			}
			case DataTypes::FLOAT_ARRAY: {
				DataTypeFloatArray* param = static_cast<DataTypeFloatArray*>(thisParam->value);
				//TODO: test this
				ImGui::InputInt((inputLabel + " length").c_str(), &param->length);
				for (int x = 0; x < param->length; x++) {
					ImGui::InputFloat((inputLabel + " i" + std::to_string(x)).c_str(), param->value[x]);
				}
				break;
			}
			case DataTypes::OPTIONS_LIST: {
				DataTypeOptionsList* param = static_cast<DataTypeOptionsList*>(thisParam->value);
				if (ImGui::BeginCombo(inputLabel.c_str(), param->options[param->value].c_str())) {
					for (int x = 0; x < param->options.size(); x++) {
						const bool is_selected = (param->value == x);
						if (ImGui::Selectable(param->options[x].c_str(), is_selected)) param->value = x;
						if (is_selected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				break;
			}
		}

		if (i == thisMaterial->GetParameterCount() - 1) ImGui::Separator();
	}

	//Filepicker for texture filepaths
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	textureSelectFileDialog.Display();
	ImGui::PopStyleVar();
	if (textureSelectFileDialog.IsOpened() && textureSelectFileDialog.HasSelected()) {
		static_cast<DataTypeTextureFilepath*>(thisMaterial->GetParameter(currentTextureSelectIndex)->value)->value = textureSelectFileDialog.GetSelected().string();
	}
}

/* Select a model given a ray through the scene */
void ModelManager::SelectModel(Ray& _r)
{
	Intersection closestHit = Intersection();
	float testDistance = 0.0f;
	for (int i = 0; i < models.size(); i++) {
		if (models[i]->DoesIntersect(_r, testDistance) && testDistance <= closestHit.distance) closestHit = Intersection(i, testDistance);
	}
	selectedModelUI = closestHit.entityIndex;
}

/* Create a model instance using a plugin */
bool ModelManager::LoadModel(std::string name, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot)
{
	SharedModelBuffers* sharedBuffer = LoadModelToLevel(name);
	if (sharedBuffer == nullptr) return false;

	Model* newModel = new Model();
	newModel->SetSharedBuffers(sharedBuffer);
	newModel->SetPosition(pos);
	newModel->SetRotation(rot, true);
	newModel->Create();
	GameObjectManager::AddObject(newModel);
	models.push_back(newModel);

	selectedModelUI = models.size() - 1;

	CommonImportEvent();
	return true;
}

/* Save a model instance using a plugin */
bool ModelManager::SaveModel(std::string name)
{
	//Get the model's shared buffers, then apply the instanced material information
	Model* modelInstance = models.at(selectedModelUI);
	LoadedModel* loadedModel = modelInstance->GetSharedBuffers()->GetAsLoadedModel();
	for (int i = 0; i < modelInstance->GetSubmeshCount(); i++) {
		loadedModel->modelParts[i].material = modelInstance->GetSubmeshMaterial(i);
	}

	//Move to worldspace if requested
	if (shouldExportAsWorld) {
		DirectX::XMMATRIX thisWorld = modelInstance->GetWorldMatrix();
		Utilities::TransformLoadedModel(loadedModel, thisWorld);
	}

	//Save it out
	bool result = Shared::pluginManager->SaveModelWithPlugin(loadedModel, name);
	Memory::SafeDelete(loadedModel);
	return result;
}

/* Load a definition of a scene and instance everything from it */
bool ModelManager::LoadScene(std::string name)
{
	bool returnVal = SetSceneAsLoadedScene(Shared::pluginManager->LoadSceneWithPlugin(name));
	CommonImportEvent();
	return returnVal;
}

/* Create and save a definition of the scene using a plugin */
bool ModelManager::SaveScene(std::string name)
{
	LoadedScene* sceneDefinition = GetSceneAsLoadedScene();
	bool result = Shared::pluginManager->SaveSceneWithPlugin(sceneDefinition, name);
	for (int i = 0; i < sceneDefinition->modelDefinitions.size(); i++) {
		Memory::SafeDelete(sceneDefinition->modelDefinitions[i].model);
	}
	Memory::SafeDelete(sceneDefinition);
	return result;
}

/* This should be called whenever a new model is instantiated */
void ModelManager::CommonImportEvent()
{
	std::vector<PluginDefinition*> importerPlugins = Shared::pluginManager->GetPluginsOfType(PluginType::IMPORT_EVENT);
	for (int i = 0; i < importerPlugins.size(); i++) {
		LoadedScene* sceneDefinition = GetSceneAsLoadedScene();
		Shared::pluginManager->CallImportEventPlugin(sceneDefinition, importerPlugins[i]);
		SetSceneAsLoadedScene(sceneDefinition);
	}

	if (selectedModelUI >= models.size()) selectedModelUI = -1;
	didIOThisFrame = true;
}

/* Generate a LoadedScene object to represent the current scene content */
LoadedScene* ModelManager::GetSceneAsLoadedScene()
{
	LoadedScene* sceneDefinition = new LoadedScene();
	sceneDefinition->camera = SceneCamera(Utilities::Vec3FromDXVec3(Shared::activeCamera->GetPosition()), Utilities::Vec3FromDXVec3(Shared::activeCamera->GetRotation()), Shared::cameraFOV);
	sceneDefinition->environmentMat = Shared::environmentMaterial;
	for (int x = 0; x < models.size(); x++) {
		LoadedModel* loadedModel = models.at(x)->GetSharedBuffers()->GetAsLoadedModel();
		for (int i = 0; i < models.at(x)->GetSubmeshCount(); i++) {
			loadedModel->modelParts[i].material = new DynamicMaterial(*models.at(x)->GetSubmeshMaterial(i));
		}
		LoadedModelPositioner loadedModelPos = LoadedModelPositioner();
		loadedModelPos.model = loadedModel;
		loadedModelPos.position = Utilities::Vec3FromDXVec3(models.at(x)->GetPosition());
		loadedModelPos.rotation = Utilities::Vec3FromDXVec3(models.at(x)->GetRotation());
		sceneDefinition->modelDefinitions.push_back(loadedModelPos);
	}
	sceneDefinition->targetResolution = Vector2(Shared::API_targetRenderWidth, Shared::API_targetRenderHeight);
	sceneDefinition->editorResolution = Vector2(Shared::m_renderWidth, Shared::m_renderHeight);
	return sceneDefinition;
}

/* Repopulate the entire scene from a LoadedScene object */
bool ModelManager::SetSceneAsLoadedScene(LoadedScene* scene)
{
	Shared::activeCamera->SetPosition(Utilities::DXVec3FromVec3(scene->camera.GetPosition()));
	Shared::activeCamera->SetRotation(Utilities::DXVec3FromVec3(scene->camera.GetRotation()), scene->camera.IsRotationInRadians());
	Shared::cameraFOV = scene->camera.GetFOV();
	if (scene->environmentMat) Shared::environmentMaterial = scene->environmentMat;

	Shared::API_targetRenderWidth = scene->targetResolution.x;
	Shared::API_targetRenderHeight = scene->targetResolution.y;

	for (int i = 0; i < models.size(); i++) {
		GameObjectManager::RemoveObject(models.at(i));
		Memory::SafeRelease(models[i]);
	}
	models.clear();

	bool didLoadOK = true;
	for (int i = 0; i < scene->modelDefinitions.size(); i++) {
		SharedModelBuffers* newLoadedModel = LoadModelToLevel(scene->modelDefinitions[i].model);
		if (newLoadedModel->DidLoadOK()) {
			Model* newModel = new Model();
			newModel->SetSharedBuffers(newLoadedModel);
			newModel->SetPosition(Utilities::DXVec3FromVec3(scene->modelDefinitions[i].position));
			newModel->SetRotation(Utilities::DXVec3FromVec3(scene->modelDefinitions[i].rotation), scene->modelDefinitions[i].rotationIsInRadians);
			for (int x = 0; x < scene->modelDefinitions[i].model->modelParts.size(); x++) {
				newModel->SetSubmeshMaterial(x, scene->modelDefinitions[i].model->modelParts[x].material);
			}
			newModel->Create();
			GameObjectManager::AddObject(newModel);
			models.push_back(newModel);
		}
		else {
			didLoadOK = false;
		}
	}

	for (int i = 0; i < scene->modelDefinitions.size(); i++) {
		for (int x = 0; x < scene->modelDefinitions[i].model->modelParts.size(); x++) {
			Memory::SafeDelete(scene->modelDefinitions[i].model->modelParts[x].material);
		}
		Memory::SafeDelete(scene->modelDefinitions[i].model);
	}
	Memory::SafeDelete(scene);

	Debug::Log("Finished setting LoadedScene, with " + std::to_string(models.size()) + " models and " + std::to_string(modelBuffers.size()) + " buffers.");
	Debug::Log("GameObjectManager has " + std::to_string(GameObjectManager::GetModels().size()) + " models.");

	return didLoadOK;
}

/* Requested load of model: check our existing loaded data, and if not already loaded, load it */
SharedModelBuffers* ModelManager::LoadModelToLevel(std::string filename)
{
	//Return an already loaded model buffer, if it exists
	for (int i = 0; i < modelBuffers.size(); i++) {
		if (modelBuffers[i]->GetFilepath() == filename) {
			Debug::Log("Pulling model from pool.");
			return modelBuffers[i];
		}
	}

	//Model isn't already loaded - load it
	SharedModelBuffers* newLoadedModel = new SharedModelBuffers(filename);
	if (!newLoadedModel->DidLoadOK()) return nullptr;
	modelBuffers.push_back(newLoadedModel);
	return newLoadedModel;
}
SharedModelBuffers* ModelManager::LoadModelToLevel(LoadedModel* modelref)
{
	//Return an already loaded model buffer, if it exists
	for (int i = 0; i < modelBuffers.size(); i++) {
		if (modelBuffers[i]->GetFilepath() == modelref->filepath) {
			Debug::Log("Pulling model from pool.");
			return modelBuffers[i];
		}
	}

	//Model isn't already loaded - load it
	SharedModelBuffers* newLoadedModel = new SharedModelBuffers(modelref);
	if (!newLoadedModel->DidLoadOK()) return nullptr;
	modelBuffers.push_back(newLoadedModel);
	return newLoadedModel;
}
