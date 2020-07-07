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
