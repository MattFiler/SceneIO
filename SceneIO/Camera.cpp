#include "Camera.h"
#include <fstream>
#include <vector>

/* Update the Camera */
void Camera::Update(float dt)
{
	GameObject::Update(dt);

	if (!isActive) return;

	camTarget = DirectX::XMLoadFloat3(&position) + XMVector3Normalize(XMVector3TransformCoord(DefaultForward, XMMatrixRotationRollPitchYaw(rotation.x, rotation.z, rotation.y)));
	dxshared::mView = XMMatrixLookAtLH(DirectX::XMLoadFloat3(&position), camTarget, camUp);

	if (isLocked) return;

	//Camera look controls
	switch (dxshared::cameraControlType) {
		case CameraControlType::KEYBOARD: {
			double moveSpeed = dt;
			if (InputHandler::KeyDown(WindowsKey::SHIFT)) {
				moveSpeed *= 10;
			}

			if (InputHandler::KeyDown(WindowsKey::W)) {
				SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z - moveSpeed));
			}
			if (InputHandler::KeyDown(WindowsKey::S)) {
				SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z + moveSpeed));
			}
			if (InputHandler::KeyDown(WindowsKey::A)) {
				SetPosition(XMFLOAT3(GetPosition().x + moveSpeed, GetPosition().y, GetPosition().z));
			}
			if (InputHandler::KeyDown(WindowsKey::D)) {
				SetPosition(XMFLOAT3(GetPosition().x - moveSpeed, GetPosition().y, GetPosition().z));
			}
			if (InputHandler::KeyDown(WindowsKey::Z)) {
				SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y + moveSpeed, GetPosition().z));
			}
			if (InputHandler::KeyDown(WindowsKey::X)) {
				SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y - moveSpeed, GetPosition().z));
			}

			if (InputHandler::KeyDown(WindowsKey::Q)) {
				SetRotation(XMFLOAT3(GetRotation().x, GetRotation().y, GetRotation().z - dt), true);
			}
			if (InputHandler::KeyDown(WindowsKey::E)) {
				SetRotation(XMFLOAT3(GetRotation().x, GetRotation().y, GetRotation().z + dt), true);
			}
			if (InputHandler::KeyDown(WindowsKey::F)) {
				SetRotation(XMFLOAT3(GetRotation().x - dt, GetRotation().y, GetRotation().z), true);
			}
			if (InputHandler::KeyDown(WindowsKey::V)) {
				SetRotation(XMFLOAT3(GetRotation().x + dt, GetRotation().y, GetRotation().z), true);
			}
			break;
		}
		case CameraControlType::MOUSE: {
			if (InputHandler::MouseDown(WindowsMouse::RIGHT_CLICK)) {
				POINT newMousePos;
				GetCursorPos(&newMousePos);
				if (!mouseWasDownLastFrame) {
					prevMousePos = newMousePos;
					ShowCursor(false);
					mouseWasReset = false;
				}

				DirectX::XMFLOAT3 forward = DirectX::XMFLOAT3(dxshared::mView.r[0].m128_f32[2], dxshared::mView.r[1].m128_f32[2], dxshared::mView.r[2].m128_f32[2]);
				DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(dxshared::mView.r[0].m128_f32[0], dxshared::mView.r[1].m128_f32[0], dxshared::mView.r[2].m128_f32[0]);
				float mov_x = (prevMousePos.x - newMousePos.x) * dxshared::mouseCameraSensitivity;
				float mov_y = (prevMousePos.y - newMousePos.y) * dxshared::mouseCameraSensitivity;

				if (InputHandler::KeyDown(WindowsKey::CTRL)) {
					SetPosition(XMFLOAT3(GetPosition().x + (mov_x * right.x), GetPosition().y - (mov_y), GetPosition().z + (mov_x * right.z)));
				}
				else {
					SetPosition(XMFLOAT3(GetPosition().x + (mov_y * forward.x), GetPosition().y, GetPosition().z + (mov_y * forward.z)));
					SetRotation(XMFLOAT3(GetRotation().x, GetRotation().y, GetRotation().z - (mov_x * 0.25f)), true);
					if (GetRotation(false).z >= 360.0f) SetRotation(XMFLOAT3(GetRotation(false).x, GetRotation(false).y, GetRotation(false).z - 360.0f));
				}

				SetCursorPos(prevMousePos.x, prevMousePos.y);
				mouseWasDownLastFrame = true;
			}
			else
			{
				mouseWasDownLastFrame = false;
				if (!mouseWasReset) {
					SetCursorPos(prevMousePos.x, prevMousePos.y);
					ShowCursor(true);
					mouseWasReset = true;
				}
			}
			break;
		}
	}
}

/* Generate a "picker" ray from the camera through the scene */
Ray Camera::GeneratePickerRay()
{
	ImGuiIO& io = ImGui::GetIO();

	float px = (((2.0f * io.MousePos.x) / dxshared::m_renderWidth) - 1.0f) / Utilities::MatrixToFloat4x4(dxshared::mProjection).m[0][0];
	float py = -(((2.0f * io.MousePos.y) / dxshared::m_renderHeight) - 1.0f) / Utilities::MatrixToFloat4x4(dxshared::mProjection).m[1][1];

	XMMATRIX inverseView = DirectX::XMMatrixInverse(nullptr, dxshared::mView);
	XMVECTOR origTransformed = DirectX::XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f)), inverseView);
	XMVECTOR dirTransformed = DirectX::XMVector3TransformNormal(XMLoadFloat3(&XMFLOAT3(px, py, 1.0f)), inverseView);
	dirTransformed = DirectX::XMVector3Normalize(dirTransformed);

	Ray toReturn = Ray();
	XMStoreFloat3(&toReturn.origin, origTransformed);
	XMStoreFloat3(&toReturn.direction, dirTransformed);
	return toReturn;
}
