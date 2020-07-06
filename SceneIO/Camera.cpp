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
				ImGuiIO& io = ImGui::GetIO();

				XMFLOAT2 newMousePos = XMFLOAT2(io.MousePos.x, io.MousePos.y);
				if (!mouseWasDownLastFrame) {
					prevMousePos = newMousePos;
					GetCursorPos(&initMousePos);
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

				prevMousePos = newMousePos;
				mouseWasDownLastFrame = true;
			}
			else
			{
				mouseWasDownLastFrame = false;
				if (!mouseWasReset) {
					SetCursorPos(initMousePos.x, initMousePos.y);
					ShowCursor(true);
					mouseWasReset = true;
				}
			}
			break;
		}
	}

	//Camera to scene interaction (test)
	if (InputHandler::MouseDown(WindowsMouse::LEFT_CLICK)) {
		Ray picker = GeneratePickerRay();
		std::string sdf = "";
	}
}

/* Generate a "picker" ray from the camera through the scene */
Ray Camera::GeneratePickerRay()
{
	ImGuiIO& io = ImGui::GetIO();

	// Move the mouse cursor coordinates into the -1 to +1 range.
	float pointX = ((2.0f * (float)io.MousePos.x) / (float)dxshared::m_renderWidth) - 1.0f;
	float pointY = (((2.0f * (float)io.MousePos.y) / (float)dxshared::m_renderHeight) - 1.0f) * -1.0f;

	// Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
	pointX = pointX / dxshared::mProjection.r[0].m128_f32[0];
	pointY = pointY / dxshared::mProjection.r[1].m128_f32[1];

	// Get the inverse of the view matrix.
	DirectX::XMMATRIX inverseViewMatrix = dxshared::mView;
	DirectX::XMMatrixInverse(nullptr, inverseViewMatrix);

	// Calculate the direction of the picking ray in view space.
	DirectX::XMFLOAT3 direction;
	direction.x = (pointX * inverseViewMatrix.r[0].m128_f32[0]) + (pointY * inverseViewMatrix.r[1].m128_f32[0]) + inverseViewMatrix.r[2].m128_f32[0];
	direction.y = (pointX * inverseViewMatrix.r[0].m128_f32[1]) + (pointY * inverseViewMatrix.r[1].m128_f32[1]) + inverseViewMatrix.r[2].m128_f32[1];
	direction.z = (pointX * inverseViewMatrix.r[0].m128_f32[2]) + (pointY * inverseViewMatrix.r[1].m128_f32[2]) + inverseViewMatrix.r[2].m128_f32[2];

	// Get the world matrix and translate to the location of the sphere.
	DirectX::XMMATRIX worldMatrix = GetWorldMatrix();
	DirectX::XMMATRIX translateMatrix = DirectX::XMMatrixTranslation(-5.0f, 1.0f, 5.0f);
	worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, translateMatrix);

	// Now get the inverse of the translated world matrix.
	DirectX::XMMATRIX inverseWorldMatrix = worldMatrix;
	DirectX::XMMatrixInverse(nullptr, inverseWorldMatrix);

	// Now transform the ray origin and the ray direction from view space to world space.
	DirectX::XMVECTOR rayOrigin = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), inverseWorldMatrix);
	DirectX::XMVECTOR rayDirection = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&direction), inverseWorldMatrix);

	// Normalize the ray direction.
	rayOrigin = DirectX::XMVector3Normalize(rayOrigin);

	Ray toReturn = Ray();
	XMStoreFloat3(&toReturn.origin, rayOrigin);
	XMStoreFloat3(&toReturn.direction, rayDirection);
	return toReturn;
}
