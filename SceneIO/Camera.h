#pragma once

#include "GameObject.h"

class Camera : public GameObject
{
public:
	~Camera() {
		Release();
	}

	void Update(float dt) override;

	void SetLocked(bool locked) {
		isLocked = locked;
	}
	bool GetLocked() {
		return isLocked;
	}

	Ray GeneratePickerRay();

private:
	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR camTarget = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	POINT initMousePos;
	XMFLOAT2 prevMousePos = XMFLOAT2(0, 0);
	bool mouseWasDownLastFrame = false;
	bool mouseWasReset = true;

	bool isLocked = false;
};