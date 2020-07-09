#include "SceneManager.h"
#include "EditorScene.h"

/* Destroy active scene on exit, if one is */
SceneManager::~SceneManager()
{
	if (currentSceneIndex != -1)
		availableScenes[currentSceneIndex]->Release();
}

bool SceneManager::Init()
{
	bool dxInit = dxmain::Init();

	EditorScene* level_scene = new EditorScene();
	AddScene(level_scene);
	ChangeScene(0);

	Debug::Log("Material added, with Albedo type: ");
	Debug::Log((int)dxshared::materialManager.GetMaterial("diffuse")->GetParameter("Albedo")->type.type);
	Debug::Log("--");

	return dxInit;
}

/* Update the current scene, and handle swapping of scenes */
bool SceneManager::Update(double dt)
{
	//Swap scenes if requested
	if (requestedSceneIndex != currentSceneIndex)
	{
		if (currentSceneIndex != -1)
			availableScenes[currentSceneIndex]->Release();
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
	ImGui::SetNextWindowSize(ImVec2(dxshared::m_renderWidth, dxshared::m_renderHeight), ImGuiCond_FirstUseEver);
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

		ImGui::EndMenuBar();
	}
	ImGui::PopStyleVar();
	ImGui::End();
	
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
