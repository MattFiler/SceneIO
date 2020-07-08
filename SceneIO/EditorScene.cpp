#include "EditorScene.h"
#include <iomanip>

/* Load config files for level */
EditorScene::EditorScene() 
{

}

/* Init the objects in the scene */
void EditorScene::Init()
{
	selectedEditModel = -1;
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
	ImGui::Begin("Controls", nullptr);

	ImGui::Text("Scene Controls");
	ImGui::Separator();
	bool camLock = main_cam.GetLocked();
	ImGui::Checkbox("Lock Cam", &camLock);
	main_cam.SetLocked(camLock);

	ImGui::Separator();
	ImGui::Text(("Cam Pos X: " + std::to_string(main_cam.GetPosition().x) + ", Y: " + std::to_string(main_cam.GetPosition().y) + "Z: " + std::to_string(main_cam.GetPosition().z)).c_str());
	ImGui::Text(("Cam Rot X: " + std::to_string(main_cam.GetRotation().x) + ", Y: " + std::to_string(main_cam.GetRotation().y) + "Z: " + std::to_string(main_cam.GetRotation().z)).c_str());

	ImGui::Separator();
	ImGui::SliderFloat("Ambient R", &dxshared::ambientLightColour.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient G", &dxshared::ambientLightColour.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient B", &dxshared::ambientLightColour.z, 0.0f, 1.0f);

	ImGui::Separator();
	if (ImGui::Button("Add Suzanne Model")) {
		LoadTestModel("data/test.obj");
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Grid Model")) {
		LoadTestModel("data/test_grid.obj");
	}

	ImGui::Checkbox("Enable Picker Debug", &enablePickerTest);

	ImGui::Separator();
	int option = dxshared::cameraControlType;
	ImGui::RadioButton("Camera Control Scheme: Keyboard", &option, 0);
	ImGui::RadioButton("Camera Control Scheme: Mouse", &option, 1);
	dxshared::cameraControlType = (CameraControlType)option;
	ImGui::SliderFloat("Sensitivity", &dxshared::mouseCameraSensitivity, 0.0f, 1.0f);
	ImGui::SliderFloat("Camera FOV", &dxshared::cameraFOV, 0.01f, 3.14f);
	if (fovCheck != dxshared::cameraFOV) dxshared::mProjection = DirectX::XMMatrixPerspectiveFovLH(dxshared::cameraFOV, dxshared::m_renderWidth / (FLOAT)dxshared::m_renderHeight, dxshared::cameraNear, dxshared::cameraFar);
	fovCheck = dxshared::cameraFOV;

	ImGui::End();

	GameObjectManager::Update(dt);

	//Allow user to click on a mesh
	if (InputHandler::KeyDown(WindowsKey::SHIFT) && InputHandler::MouseDown(WindowsMouse::LEFT_CLICK))
	{
		//Create picker ray
		Ray picker = main_cam.GeneratePickerRay();

		//Picker to mesh intersection
		if (!testLastFrame) {
			Intersection closestHit = Intersection();
			float testDistance = 0.0f;
			for (int i = 0; i < allActiveModels.size(); i++) {
				if (allActiveModels[i]->DoesIntersect(picker, testDistance)) {
					if (testDistance <= closestHit.distance) closestHit = Intersection(i, testDistance);
				}
			}
			if (closestHit.entityIndex != -1) {
				GameObjectManager::RemoveObject(allActiveModels.at(closestHit.entityIndex));
				delete allActiveModels.at(closestHit.entityIndex);
				allActiveModels.erase(allActiveModels.begin() + closestHit.entityIndex);
				selectedEditModel -= 1;
				Debug::Log("Intersected with model " + std::to_string(closestHit.entityIndex) + " at distance " + std::to_string(closestHit.distance));
			}
			testLastFrame = true;
		}

		//Debug
		if (enablePickerTest) for (int i = 1; i < 10; i++) LoadTestModel("data/cube.obj", XMFLOAT3(picker.origin.x + (picker.direction.x * 10 * i), picker.origin.y + (picker.direction.y * 10 * i), picker.origin.z + (picker.direction.z * 10 * i)));
	}

	//Only continue if our requested edit object is valid
	if (selectedEditModel == -1) return true;

	//Get the GameObject we're editing
	GameObject* objectToEdit = allActiveModels.at(selectedEditModel);

	//Get matrices as float arrays
	float* objectMatrix = &objectToEdit->GetWorldMatrix4X4().m[0][0];
	float* projMatrix = &Utilities::MatrixToFloat4x4(dxshared::mProjection).m[0][0];
	float* viewMatrix = &Utilities::MatrixToFloat4x4(dxshared::mView).m[0][0];

	//Show options to swap between different transforms
	ImGui::Begin("Transform Controls", nullptr);
	if (ImGui::RadioButton("Translate", dxshared::mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	//if (ImGui::RadioButton("Rotate", dxshared::mCurrentGizmoOperation == ImGuizmo::ROTATE))
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

	//If user isn't clicking on a mesh, allow gizmo control
	if (!(InputHandler::KeyDown(WindowsKey::SHIFT) && InputHandler::MouseDown(WindowsMouse::LEFT_CLICK))) 
	{
		//Draw manipulation control
		ImGuizmo::SetRect(0, 0, dxshared::m_renderWidth, dxshared::m_renderHeight);
		ImGuizmo::Manipulate(viewMatrix, projMatrix, dxshared::mCurrentGizmoOperation, dxshared::mCurrentGizmoMode, objectMatrix, NULL, NULL);

		testLastFrame = false;
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
