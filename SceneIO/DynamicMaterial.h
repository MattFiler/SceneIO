#pragma once

#include "MaterialParameter.h"
#include "Utilities.h"

enum class MaterialSurfaceTypes {
	SURFACE,
	VOLUME,
	ENVIRONMENT,
};

/* A material which can be defined by JSON */
class DynamicMaterial {
public:
	DynamicMaterial(json _config, int _index);
	DynamicMaterial(const DynamicMaterial& cpy);
	~DynamicMaterial() {
		for (int i = 0; i < parameters.size(); i++) {
			Memory::SafeDelete(parameters[i]);
		}
		parameters.clear();
		if (isCopy) return;
		Memory::SafeRelease(m_vertexShader);
		Memory::SafeRelease(m_pixelShader);
		Memory::SafeDelete(m_vertexLayout);
	}

	std::string GetName() {
		return name;
	}
	int GetIndex() {
		return matIndex;
	}
	MaterialSurfaceTypes GetSurfaceType() {
		return type;
	}

	MaterialParameter* GetParameter(std::string name) {
		for (int i = 0; i < parameters.size(); i++) {
			if (parameters[i]->name == name) {
				return parameters[i];
			}
		}
		return nullptr;
	}
	MaterialParameter* GetParameter(int index) {
		if (index >= 0 && index < parameters.size()) return parameters[index];
		return nullptr;
	}

	int GetParameterCount() {
		return parameters.size();
	}

	void SetShader() {
		Shared::m_pImmediateContext->VSSetShader(m_vertexShader, nullptr, 0);
		Shared::m_pImmediateContext->PSSetShader(m_pixelShader, nullptr, 0);
		Shared::m_pImmediateContext->IASetInputLayout(m_vertexLayout);
	}

private:
	void Setup(bool setupShaders);

	std::string name = "";
	MaterialSurfaceTypes type = MaterialSurfaceTypes::SURFACE;
	std::vector<MaterialParameter*> parameters = std::vector<MaterialParameter*>();
	json config;

	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_vertexLayout = nullptr;

	int matIndex = 0;

	bool isCopy = false;
};