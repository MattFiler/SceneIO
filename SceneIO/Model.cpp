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
	modelData = nullptr;
}

/* Update bounding box transforms as well as normal transforms */
void Model::Update(float dt)
{
	GameObject::Update(dt);

	DirectX::XMStoreFloat4(&boundingBox.Orientation, DirectX::XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));
	boundingBox.Extents = Utilities::MultiplyFloat3(modelData->GetExtents(), scale);
	boundingBox.Center = Utilities::AddFloat3(position, modelData->GetExtentsOffset());
}

/* Render all model parts */
void Model::Render(float dt)
{
	GameObject::Render(dt);

	if (!isActive) return;
	if (isInvisible) return;
	
	if (modelData) modelData->Render(mWorld, &materials);
}

/* Check for an intersection with a ray and this model instance */
bool Model::DoesIntersect(Ray& _r, float& _d)
{
	//First, check less expensive bounding box
	if (!boundingBox.Intersects(XMLoadFloat3(&_r.origin), XMLoadFloat3(&_r.direction), _d)) return false;

	//If hit the bounding box, check the mesh (expensive)
	return modelData->DoesRayIntersect(_r, mWorld, _d);
}
