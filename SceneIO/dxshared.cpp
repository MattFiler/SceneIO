#include "dxshared.h"

/* Init shared values */
ID3D11Device* dxshared::m_pDevice = nullptr;
ID3D11DeviceContext* dxshared::m_pImmediateContext = nullptr;

UINT dxshared::m_renderWidth = 0;
UINT dxshared::m_renderHeight = 0;

DirectX::XMMATRIX dxshared::mView;
DirectX::XMMATRIX dxshared::mProjection;

DirectX::XMFLOAT4 dxshared::ambientLightColour;

float dxshared::mouseCameraSensitivity = 0.03f;
CameraControlType dxshared::cameraControlType = CameraControlType::MOUSE;

float dxshared::cameraFOV = 1.5f;
float dxshared::cameraNear = 0.01f;
float dxshared::cameraFar = 2000.0f;

HWND dxshared::m_hwnd;

bool dxshared::drawBoundingBoxes = false;
#if _DEBUG
bool dxshared::pauseNPCs = true;
#else
bool dxshared::pauseNPCs = false;
#endif

std::string dxshared::currentLevelPath = "";

ImGuizmo::OPERATION dxshared::mCurrentGizmoOperation = ImGuizmo::ROTATE;
ImGuizmo::MODE dxshared::mCurrentGizmoMode = ImGuizmo::WORLD;