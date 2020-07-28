#include "Skybox.h"

void Skybox::Create() 
{
    /*
    int NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
    int NumSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

    float sphereYaw = 0.0f;
    float spherePitch = 0.0f;

    std::vector<SimpleVertex> vertices(NumSphereVertices);

    XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    vertices[0].Pos.x = 0.0f;
    vertices[0].Pos.y = 0.0f;
    vertices[0].Pos.z = 1.0f;

    for (DWORD i = 0; i < LatLines - 2; ++i)
    {
        spherePitch = (i + 1) * (3.14 / (LatLines - 1));
        DirectX::XMMATRIX Rotationx = XMMatrixRotationX(spherePitch);
        for (DWORD j = 0; j < LongLines; ++j)
        {
            sphereYaw = j * (6.28 / (LongLines));
            DirectX::XMMATRIX Rotationy = XMMatrixRotationZ(sphereYaw);
            currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
            currVertPos = XMVector3Normalize(currVertPos);
            vertices[i * LongLines + j + 1].Pos.x = XMVectorGetX(currVertPos);
            vertices[i * LongLines + j + 1].Pos.y = XMVectorGetY(currVertPos);
            vertices[i * LongLines + j + 1].Pos.z = XMVectorGetZ(currVertPos);
        }
    }

    vertices[NumSphereVertices - 1].Pos.x = 0.0f;
    vertices[NumSphereVertices - 1].Pos.y = 0.0f;
    vertices[NumSphereVertices - 1].Pos.z = -1.0f;


    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * NumSphereVertices;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;

    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = &vertices[0];
    HR(Shared::m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &sphereVertBuffer));


    std::vector<DWORD> indices(NumSphereFaces * 3);

    int k = 0;
    for (DWORD l = 0; l < LongLines - 1; ++l)
    {
        indices[k] = 0;
        indices[k + 1] = l + 1;
        indices[k + 2] = l + 2;
        k += 3;
    }

    indices[k] = 0;
    indices[k + 1] = LongLines;
    indices[k + 2] = 1;
    k += 3;

    for (DWORD i = 0; i < LatLines - 3; ++i)
    {
        for (DWORD j = 0; j < LongLines - 1; ++j)
        {
            indices[k] = i * LongLines + j + 1;
            indices[k + 1] = i * LongLines + j + 2;
            indices[k + 2] = (i + 1) * LongLines + j + 1;

            indices[k + 3] = (i + 1) * LongLines + j + 1;
            indices[k + 4] = i * LongLines + j + 2;
            indices[k + 5] = (i + 1) * LongLines + j + 2;

            k += 6; // next quad
        }

        indices[k] = (i * LongLines) + LongLines;
        indices[k + 1] = (i * LongLines) + 1;
        indices[k + 2] = ((i + 1) * LongLines) + LongLines;

        indices[k + 3] = ((i + 1) * LongLines) + LongLines;
        indices[k + 4] = (i * LongLines) + 1;
        indices[k + 5] = ((i + 1) * LongLines) + 1;

        k += 6;
    }

    for (DWORD l = 0; l < LongLines - 1; ++l)
    {
        indices[k] = NumSphereVertices - 1;
        indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
        indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
        k += 3;
    }

    indices[k] = NumSphereVertices - 1;
    indices[k + 1] = (NumSphereVertices - 1) - LongLines;
    indices[k + 2] = NumSphereVertices - 2;

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;

    iinitData.pSysMem = &indices[0];
    Shared::m_pDevice->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIndexBuffer);

    HR(Utilities::CompileShaderFromFile(L"Effects.fx", "SKYMAP_VS", "vs_4_0", &SKYMAP_VS_Buffer));
    HR(Utilities::CompileShaderFromFile(L"Effects.fx", "SKYMAP_PS", "ps_4_0", &SKYMAP_PS_Buffer));

    HR(Shared::m_pDevice->CreateVertexShader(SKYMAP_VS_Buffer->GetBufferPointer(), SKYMAP_VS_Buffer->GetBufferSize(), NULL, &SKYMAP_VS));
    HR(Shared::m_pDevice->CreatePixelShader(SKYMAP_PS_Buffer->GetBufferPointer(), SKYMAP_PS_Buffer->GetBufferSize(), NULL, &SKYMAP_PS));

    D3DX11_IMAGE_LOAD_INFO loadSMInfo;
    loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    Texture* thisTex = Utilities::LoadTexture("skymap.dds");
    ID3D11Texture2D* SMTexture = 0;
    hr = D3DX11CreateTextureFromFile(d3d11Device, L"skymap.dds",
        &loadSMInfo, 0, (ID3D11Resource**)&SMTexture, 0);

    D3D11_TEXTURE2D_DESC SMTextureDesc;
    SMTexture->GetDesc(&SMTextureDesc);

    D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
    SMViewDesc.Format = SMTextureDesc.Format;
    SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
    SMViewDesc.TextureCube.MostDetailedMip = 0;

    hr = d3d11Device->CreateShaderResourceView(SMTexture, &SMViewDesc, &smrv);
    */
}

void Skybox::Release()
{
    sphereIndexBuffer->Release();
    sphereVertBuffer->Release();

    SKYMAP_VS->Release();
    SKYMAP_PS->Release();
    SKYMAP_VS_Buffer->Release();
    SKYMAP_PS_Buffer->Release();

    smrv->Release();

    DSLessEqual->Release();
    RSCullNone->Release();
}
