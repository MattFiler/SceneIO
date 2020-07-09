#include "EditorScene.h"
#include <iomanip>

/* Init the objects in the scene */
void EditorScene::Init()
{
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

	//Setup subsystems
	materialManager = new DynamicMaterialManager();
	modelManager = new ModelManager();
}

/* Release all objects in the scene */
void EditorScene::Release() {
	if (materialManager) {
		delete materialManager;
		materialManager = nullptr;
	}
	if (modelManager) {
		delete modelManager;
		modelManager = nullptr;
	}
	GameObjectManager::Release();
}

/* Update the objects in the scene */
bool EditorScene::Update(double dt)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::Begin("Scene Controls", nullptr);
	ImGui::PopStyleVar();

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
	int option = dxshared::cameraControlType;
	ImGui::RadioButton("Camera Control Scheme: Keyboard", &option, 0);
	ImGui::RadioButton("Camera Control Scheme: Mouse", &option, 1);
	dxshared::cameraControlType = (CameraControlType)option;
	ImGui::SliderFloat("Sensitivity", &dxshared::mouseCameraSensitivity, 0.0f, 1.0f);
	ImGui::SliderFloat("Camera FOV", &dxshared::cameraFOV, 0.01f, 3.14f);
	if (fovCheck != dxshared::cameraFOV) dxshared::mProjection = DirectX::XMMatrixPerspectiveFovLH(dxshared::cameraFOV, dxshared::m_renderWidth / (FLOAT)dxshared::m_renderHeight, dxshared::cameraNear, dxshared::cameraFar);
	fovCheck = dxshared::cameraFOV;

	ImGui::End();

	modelManager->Update();
	materialManager->Update();

	GameObjectManager::Update(dt);

	//Allow user to click on a mesh
	if (InputHandler::KeyDown(WindowsKey::SHIFT) && InputHandler::MouseDown(WindowsMouse::LEFT_CLICK))
	{
		//Create picker ray
		Ray picker = main_cam.GeneratePickerRay();

		//Picker to mesh intersection
		if (!testLastFrame) {
			modelManager->SelectModel(picker);
			testLastFrame = true;
		}
	}
	else 
	{
		testLastFrame = false;
	}

	return true;
}

/* Render the objects in the scene */
void EditorScene::Render(double dt)
{
	GameObjectManager::Render(dt);
}