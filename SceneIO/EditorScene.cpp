#include "EditorScene.h"
#include <iomanip>

/* Load config files for level */
EditorScene::EditorScene() 
{

}

/* Init the objects in the scene */
void EditorScene::Init()
{
	selectedEditModel = 0;
	dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

	//Setup cam & light source
	light_source = Light();
	main_cam = Camera();
	GameObjectManager::AddObject(&light_source);
	GameObjectManager::AddObject(&main_cam);
	GameObjectManager::Create();
	main_cam.SetLocked(false);
	light_source.SetPosition(DirectX::XMFLOAT3(0.5, 0.5, 0.5));

	//Position "player"
	main_cam.SetPosition(DirectX::XMFLOAT3(0, 0, 0));
	main_cam.SetRotation(DirectX::XMFLOAT3(0, 0, 0));
}

/* Release all objects in the scene */
void EditorScene::Release() {
	for (int i = 0; i < loadedModels.size(); i++) {
		delete loadedModels[i];
	}
	loadedModels.clear();
	GameObjectManager::Release();
}

/* Update the objects in the scene */
bool EditorScene::Update(double dt)
{
	bool open = true;
	bool camLock = main_cam.GetLocked();

	ImGui::SetNextWindowPos(ImVec2(950, 0));
	ImGui::SetNextWindowSize(ImVec2(330, 720));
	ImGui::Begin("Controls", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	ImGui::Text("Scene Controls");
	ImGui::Separator();
	ImGui::Checkbox("Lock Cam", &camLock);

	ImGui::Separator();
	ImGui::Text(("Cam Pos X: " + std::to_string(main_cam.GetPosition().x) + ", Y: " + std::to_string(main_cam.GetPosition().y) + "Z: " + std::to_string(main_cam.GetPosition().z)).c_str());
	ImGui::Text(("Cam Rot X: " + std::to_string(main_cam.GetRotation().x) + ", Y: " + std::to_string(main_cam.GetRotation().y) + "Z: " + std::to_string(main_cam.GetRotation().z)).c_str());

	ImGui::Separator();
	ImGui::SliderFloat("Ambient R", &dxshared::ambientLightColour.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient G", &dxshared::ambientLightColour.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient B", &dxshared::ambientLightColour.z, 0.0f, 1.0f);

	ImGui::Separator();
	if (ImGui::Button("Add Model")) {
		Model* new_model = new Model();
		new_model->SetData(LoadModelToLevel("shaders/test.obj"));
		new_model->Create();
		GameObjectManager::AddObject(new_model);
		allActiveModels.push_back(new_model);

		selectedEditModel = allActiveModels.size() - 1;
	}

	ImGui::End();

	main_cam.SetLocked(camLock);
	GameObjectManager::Update(dt);

	//Only continue if our requested edit object is valid
	if (true) return true;

	//Get the GameObject we're editing
	GameObject* objectToEdit = allActiveModels.at(selectedEditModel);

	//Get matrices as float arrays
	float* objectMatrix = &objectToEdit->GetWorldMatrix4X4().m[0][0];
	float* projMatrix = &dxutils.MatrixToFloat4x4(dxshared::mProjection).m[0][0];
	float* viewMatrix = &dxutils.MatrixToFloat4x4(dxshared::mView).m[0][0];

	//Show options to swap between different transforms
	ImGui::SetNextWindowPos(ImVec2(950, 590));
	ImGui::SetNextWindowSize(ImVec2(330, 130));
	ImGui::Begin("Transform Controls", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	if (ImGui::RadioButton("Translate", dxshared::mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	//if (currentEditorMode != 1 && ImGui::RadioButton("Rotate", dxshared::mCurrentGizmoOperation == ImGuizmo::ROTATE))
	//	dxshared::mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", dxshared::mCurrentGizmoOperation == ImGuizmo::SCALE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::SCALE;

	//Allow swap between local/world
	if (dxshared::mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", dxshared::mCurrentGizmoMode == ImGuizmo::LOCAL))
			dxshared::mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", dxshared::mCurrentGizmoMode == ImGuizmo::WORLD))
			dxshared::mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	//Draw manipulation control
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(viewMatrix, projMatrix, dxshared::mCurrentGizmoOperation, dxshared::mCurrentGizmoMode, objectMatrix, NULL, NULL); 

	//Get values from manipulation
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);

	//We don't allow gizmo editing of rotation, as ImGuizmo's accuracy really sucks, and throws everything off
	matrixRotation[0] = objectToEdit->GetRotation(false).x;
	matrixRotation[1] = objectToEdit->GetRotation(false).y;
	matrixRotation[2] = objectToEdit->GetRotation(false).z;

	//Allow text overwrite
	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	//if (currentEditorMode != 1) ImGui::InputFloat3("Rotation", matrixRotation, 3);
	ImGui::InputFloat3("Scale", matrixScale, 3);

	//Set new transforms back
	objectToEdit->SetPosition(DirectX::XMFLOAT3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
	//if (currentEditorMode != 1) objectToEdit->SetRotation(DirectX::XMFLOAT3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));
	objectToEdit->SetScale(DirectX::XMFLOAT3(matrixScale[0], matrixScale[1], matrixScale[2]));
	ImGui::End();

	return true;
}

/* Render the objects in the scene */
void EditorScene::Render(double dt)
{
	GameObjectManager::Render(dt);
}


/* Requested load of model: check our existing loaded data, and if not already loaded, load it */
SharedModelBuffers* EditorScene::LoadModelToLevel(std::string filename)
{
	//Return an already loaded model buffer, if it exists
	for (int i = 0; i < loadedModels.size(); i++) {
		if (loadedModels[i]->GetFilepath() == filename) {
			Debug::Log("Pulling model from pool.");
			return loadedModels[i];
		}
	}

	//Model isn't already loaded - load it
	SharedModelBuffers* newLoadedModel = new SharedModelBuffers(filename);
	loadedModels.push_back(newLoadedModel);
	return newLoadedModel;
}
