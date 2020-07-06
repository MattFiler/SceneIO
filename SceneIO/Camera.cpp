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

	double moveSpeed = dt;
	if (InputHandler::KeyDown(WindowsKey::SHIFT)) {
		moveSpeed *= 20;
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
