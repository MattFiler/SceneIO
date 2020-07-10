#include "ModelManager.h"
#include "imgui/imgui.h"
#include "InputHandler.h"

/* Free all model instances */
ModelManager::~ModelManager()
{
	for (int i = 0; i < modelBuffers.size(); i++) {
		delete modelBuffers[i];
	}
	modelBuffers.clear();
}

/* Render controls UI */
void ModelManager::Update()
{
	//Hotkeys for swapping modes (TODO: add rotation back)
	if (InputHandler::KeyDown(WindowsKey::O)) {
		Shared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	else if (InputHandler::KeyDown(WindowsKey::P)) {
		Shared::mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	//UI
	ModelManagerUI();
	ModelTransformUI();
	ModelMaterialUI();
}

/* Model manager core UI (list model instances, add new models, etc) */
void ModelManager::ModelManagerUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Model Controls", nullptr);
	ImGui::PopStyleVar();

	if (ImGui::Button("Add Suzanne Model")) {
		LoadTestModel("data/test.obj");
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Grid Model")) {
		LoadTestModel("data/test_grid.obj");
	}

	/*
	if (didRequestLoad) {
		if (requestedLoadPath != "") {
			LoadTestModel(requestedLoadPath);
		}
		didRequestLoad = false;
	}
	if (ImGui::Button("Select Model To Add")) {
		requestedLoadPath = Utilities::OpenFile();
		didRequestLoad = true;
	}
	*/

	ImGui::Separator();

	if (ImGui::Button("Delete Selected") && selectedModelUI != -1) {
		GameObjectManager::RemoveObject(models.at(selectedModelUI));
		delete models.at(selectedModelUI);
		models.erase(models.begin() + selectedModelUI);
		selectedModelUI -= 1;
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Model Instances", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int i = 0; i < GetModelCount(); i++) {
			ImGui::RadioButton(std::to_string(i).c_str(), &selectedModelUI, i);
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
	if (ImGui::RadioButton("Translate", Shared::mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		Shared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	//if (ImGui::RadioButton("Rotate", dxshared::mCurrentGizmoOperation == ImGuizmo::ROTATE))
	//	dxshared::mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", Shared::mCurrentGizmoOperation == ImGuizmo::SCALE))
		Shared::mCurrentGizmoOperation = ImGuizmo::SCALE;

	//Allow swap between local/world
	if (Shared::mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", Shared::mCurrentGizmoMode == ImGuizmo::LOCAL))
			Shared::mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", Shared::mCurrentGizmoMode == ImGuizmo::WORLD))
			Shared::mCurrentGizmoMode = ImGuizmo::WORLD;
	}

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

	//Allow text overwrite
	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	ImGui::InputFloat3("Rotation", matrixRotation, 3);
	ImGui::InputFloat3("Scale", matrixScale, 3);

	//Set new transforms back
	objectToEdit->SetPosition(DirectX::XMFLOAT3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
	objectToEdit->SetRotation(DirectX::XMFLOAT3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));
	objectToEdit->SetScale(DirectX::XMFLOAT3(matrixScale[0], matrixScale[1], matrixScale[2]));

	ImGui::End();
}

/* Model material UI (change selected model's material parameters) */
void ModelManager::ModelMaterialUI()
{
	//Only continue if our requested edit object is valid
	if (selectedModelUI == -1) return;

	//Get the GameObject we're editing
	Model* objectToEdit = models.at(selectedModelUI);

	//Show options to change material properties
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Model Material Control", nullptr);
	ImGui::PopStyleVar();

	//Shared::materialManager->GetMaterial(objectToEdit->);

	ImGui::End();
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

/* Testing: this will be reworked once the plugins are in place */
void ModelManager::LoadTestModel(std::string name, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot)
{
	Model* new_model = new Model();
	new_model->SetData(LoadModelToLevel(name));
	new_model->SetPosition(pos);
	new_model->SetRotation(rot, true);
	new_model->Create();
	GameObjectManager::AddObject(new_model);
	models.push_back(new_model);

	selectedModelUI = models.size() - 1;
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
	modelBuffers.push_back(newLoadedModel);
	return newLoadedModel;
}
