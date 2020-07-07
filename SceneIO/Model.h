#pragma once

#include "dxutils.h"
#include "GameObject.h"
#include "SharedModelBuffers.h"

class Model : public GameObject {
public:
	~Model() {
		Release();
	}

	void Create() override;
	void Release() override;
	void Update(float dt) override;
	void Render(float dt) override;

	void SetData(SharedModelBuffers* _m) {
		modelData = _m;
	}

	bool DoesIntersect(Ray& _r) {
		bool contains = false;
		for (int i = (int)dxshared::cameraNear; i < (int)dxshared::cameraFar; i += 1) {
			if (boundingBox.Contains(XMLoadFloat3(&XMFLOAT3(_r.origin.x + (_r.direction.x * i), _r.origin.y + (_r.direction.y * i), _r.origin.z + (_r.direction.z * i))))) {
				contains = true;
				break;
			}
		}
		return contains;
	}

protected:
	SharedModelBuffers* modelData = nullptr;
	DirectX::BoundingOrientedBox boundingBox;
};