#include "SceneManager.h"
#include "EditorScene.h"

/* Destroy active scene on exit, if one is */
SceneManager::~SceneManager()
{
	Memory::SafeDelete(Shared::materialManager);
	if (currentSceneIndex != -1) availableScenes[currentSceneIndex]->Release();
	for (int i = 0; i < availableScenes.size(); i++) {
		Memory::SafeRelease(availableScenes[i]);
	}
}

/* Set up the core functionality */
bool SceneManager::Init()
{
	bool dxInit = MainSetup::Init();

	EditorScene* level_scene = new EditorScene();
	AddScene(level_scene);
	ChangeScene(0);

	Shared::materialManager = new DynamicMaterialManager();

	return dxInit;
}

/* Update the current scene, and handle swapping of scenes */
bool SceneManager::Update(double dt)
{
	//Swap scenes if requested
	if (requestedSceneIndex != currentSceneIndex)
	{
		if (currentSceneIndex != -1) availableScenes[currentSceneIndex]->Release();
		currentSceneIndex = requestedSceneIndex;
		availableScenes[currentSceneIndex]->Init();
	}

	//New ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	//Dockspace
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(Shared::m_renderWidth, Shared::m_renderHeight), ImGuiCond_FirstUseEver);
	ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar);
	ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene")) int test1 = 0;
			if (ImGui::MenuItem("Load Scene")) int test2 = 0;
			ImGui::Separator();
			if (ImGui::MenuItem("Quit")) return false;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About")) {
			if (ImGui::MenuItem("Controls")) {
				showControlsMenu = true;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	ImGui::PopStyleVar();
	ImGui::End();
	
	//Controls menu
	if (showControlsMenu) {
		ImGui::SetNextWindowPos(ImVec2(490, 248));
		ImGui::SetNextWindowSize(ImVec2(279, 205));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
		ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::PopStyleVar();

		ImGui::Text("Select Model: Shift+LMB");
		ImGui::Text("Move Camera: RMB");
		ImGui::Text("Move Camera Alt: Ctrl+RMB");

		ImGui::Separator();

		ImGui::Text("Translate Mode: O");
		ImGui::Text("Scale Mode: P");

		ImGui::Separator();

		if (ImGui::Button("Close")) {
			showControlsMenu = false;
		}

		ImGui::End();
	}

	//Update current scene
	if (currentSceneIndex != -1)
		return availableScenes[currentSceneIndex]->Update(dt);
}

/* Render the current scene (if one is set) */
void SceneManager::Render(double dt)
{
	//Clear back buffer & depth stencil view
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::CornflowerBlue);
	m_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Render scene
	if (currentSceneIndex != -1)
		availableScenes[currentSceneIndex]->Render(dt);

	//Render ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//Present the back buffer to front buffer
	m_pSwapChain->Present(0, 0);
}
