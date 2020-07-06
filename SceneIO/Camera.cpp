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
	if (InputHandler::KeyPressed(WindowsKey::SHIFT)) {
		moveSpeed *= 20;
	}
	if (InputHandler::KeyPressed(WindowsKey::W)) {
		SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z - moveSpeed));
	}
	if (InputHandler::KeyPressed(WindowsKey::S)) {
		SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z + moveSpeed));
	}
	if (InputHandler::KeyPressed(WindowsKey::A)) {
		SetPosition(XMFLOAT3(GetPosition().x + moveSpeed, GetPosition().y, GetPosition().z));
	}
	if (InputHandler::KeyPressed(WindowsKey::D)) {
		SetPosition(XMFLOAT3(GetPosition().x - moveSpeed, GetPosition().y, GetPosition().z));
	}
	if (InputHandler::KeyPressed(WindowsKey::Z)) {
		SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y + moveSpeed, GetPosition().z));
	}
	if (InputHandler::KeyPressed(WindowsKey::X)) {
		SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y - moveSpeed, GetPosition().z));
	}
	if (InputHandler::KeyPressed(WindowsKey::Q)) {
		SetRotation(XMFLOAT3(GetRotation().x, GetRotation().y, GetRotation().z - dt), true);
	}
	if (InputHandler::KeyPressed(WindowsKey::E)) {
		SetRotation(XMFLOAT3(GetRotation().x, GetRotation().y, GetRotation().z + dt), true);
	}
	if (InputHandler::KeyPressed(WindowsKey::F)) {
		SetRotation(XMFLOAT3(GetRotation().x - dt, GetRotation().y, GetRotation().z), true);
	}
	if (InputHandler::KeyPressed(WindowsKey::V)) {
		SetRotation(XMFLOAT3(GetRotation().x + dt, GetRotation().y, GetRotation().z), true);
	}
}
