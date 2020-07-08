#include "Model.h"

/* Add useage to model data */
void Model::Create()
{
	GameObject::Create();
	if (!modelData) Debug::Log("Creating a model without its data - check scripts!");
	modelData->AddUseage();
}

/* Remove useage from model data */
void Model::Release()
{
	GameObject::Release();
	if (modelData) modelData->RemoveUseage();
}

/* Update bounding box transforms as well as normal transforms */
void Model::Update(float dt)
{
	GameObject::Update(dt);

	DirectX::XMStoreFloat4(&boundingBox.Orientation, DirectX::XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));
	boundingBox.Extents = modelData->GetExtents();
	boundingBox.Center = position;
}

/* Render all model parts */
void Model::Render(float dt)
{
	GameObject::Render(dt);

	if (!isActive) return;
	if (isInvisible) return;
	
	if (modelData) modelData->Render(mWorld);
}

/* Check for an intersection with a ray and this model instance */
bool Model::DoesIntersect(Ray& _r)
{
	/*
	//First, check less expensive bounding box
	bool contains = false;
	for (int i = (int)dxshared::cameraNear; i < (int)dxshared::cameraFar; i += 1) {
		if (boundingBox.Contains(XMLoadFloat3(&XMFLOAT3(_r.origin.x + (_r.direction.x * i), _r.origin.y + (_r.direction.y * i), _r.origin.z + (_r.direction.z * i))))) {
			contains = true;
			break;
		}
	}
	if (!contains) return false;
	*/
	//If hit the bounding box, check the mesh (expensive)
	return modelData->DoesRayIntersect(_r, mWorld);
}
