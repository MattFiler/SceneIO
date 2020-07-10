#include "Shared.h"

ID3D11Device* Shared::m_pDevice = nullptr;
ID3D11DeviceContext* Shared::m_pImmediateContext = nullptr;

HWND Shared::m_hwnd;

UINT Shared::m_renderWidth = 0;
UINT Shared::m_renderHeight = 0;

DirectX::XMMATRIX Shared::mView;
DirectX::XMMATRIX Shared::mProjection;

DirectX::XMFLOAT4 Shared::ambientLightColour;

float Shared::mouseCameraSensitivity = 0.03f;
CameraControlType Shared::cameraControlType = CameraControlType::MOUSE;

float Shared::cameraFOV = 1.5f;
float Shared::cameraNear = 0.01f;
float Shared::cameraFar = 2000.0f;

DynamicMaterialManager* Shared::materialManager = nullptr;

ImGuizmo::OPERATION Shared::mCurrentGizmoOperation = ImGuizmo::ROTATE;
ImGuizmo::MODE Shared::mCurrentGizmoMode = ImGuizmo::WORLD;