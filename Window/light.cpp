#include "light.h"

// Initialize all needed instances
HRESULT Light::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr = S_OK;

    // Set up lights
    for (int i = 0; i < MAX_LIGHT; i++) {
        m_posColorVector.push_back(std::pair<XMFLOAT3, XMFLOAT3>(
            XMFLOAT3((float)(rand() % 10 - 5), (float)(rand() % 10 - 5), (float)(rand() % 10 - 5)),
            XMFLOAT3(1.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f)));
    }

    UINT LatLines = 10;
    UINT LongLines = 10;
    // Create vertex array
    m_numSphereVertices = ((LatLines - 2) * LongLines) + 2;
    m_numSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

    float sphereYaw = 0.0f;
    float spherePitch = 0.0f;

    std::vector<Vertex> vertices(m_numSphereVertices);

    XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    vertices[0].x = 0.0f;
    vertices[0].y = 0.0f;
    vertices[0].z = 1.0f;

    for (UINT i = 0; i < LatLines - 2; i++) {
        spherePitch = (i + 1) * (XM_PI / (LatLines - 1));
        XMMATRIX Rotationx = XMMatrixRotationX(spherePitch);
        for (UINT j = 0; j < LongLines; j++) {
            sphereYaw = j * (XM_2PI / (LongLines));
            XMMATRIX Rotationy = XMMatrixRotationZ(sphereYaw);
            currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
            currVertPos = XMVector3Normalize(currVertPos);
            vertices[i * static_cast<__int64>(LongLines) + j + 1].x = XMVectorGetX(currVertPos);
            vertices[i * static_cast<__int64>(LongLines) + j + 1].y = XMVectorGetY(currVertPos);
            vertices[i * static_cast<__int64>(LongLines) + j + 1].z = XMVectorGetZ(currVertPos);
        }
    }

    vertices[static_cast<__int64>(m_numSphereVertices) - 1].x = 0.0f;
    vertices[static_cast<__int64>(m_numSphereVertices) - 1].y = 0.0f;
    vertices[static_cast<__int64>(m_numSphereVertices) - 1].z = -1.0f;

    // Create index array
    std::vector<UINT> indices(static_cast<__int64>(m_numSphereFaces) * 3);

    int k = 0;
    for (UINT i = 0; i < LongLines - 1; i++) {
        indices[k] = 0;
        indices[static_cast<__int64>(k) + 1] = i + 1;
        indices[static_cast<__int64>(k) + 2] = i + 2;
        k += 3;
    }
    indices[k] = 0;
    indices[static_cast<__int64>(k) + 1] = LongLines;
    indices[static_cast<__int64>(k) + 2] = 1;
    k += 3;

    for (UINT i = 0; i < LatLines - 3; i++) {
        for (UINT j = 0; j < LongLines - 1; j++) {
            indices[k] = i * LongLines + j + 1;
            indices[static_cast<__int64>(k) + 1] = i * LongLines + j + 2;
            indices[static_cast<__int64>(k) + 2] = (i + 1) * LongLines + j + 1;

            indices[static_cast<__int64>(k) + 3] = (i + 1) * LongLines + j + 1;
            indices[static_cast<__int64>(k) + 4] = i * LongLines + j + 2;
            indices[static_cast<__int64>(k) + 5] = (i + 1) * LongLines + j + 2;

            k += 6;
        }

        indices[k] = (i * LongLines) + LongLines;
        indices[static_cast<__int64>(k) + 1] = (i * LongLines) + 1;
        indices[static_cast<__int64>(k) + 2] = ((i + 1) * LongLines) + LongLines;

        indices[static_cast<__int64>(k) + 3] = ((i + 1) * LongLines) + LongLines;
        indices[static_cast<__int64>(k) + 4] = (i * LongLines) + 1;
        indices[static_cast<__int64>(k) + 5] = ((i + 1) * LongLines) + 1;

        k += 6;
    }

    for (UINT i = 0; i < LongLines - 1; i++) {
        indices[k] = m_numSphereVertices - 1;
        indices[static_cast<__int64>(k) + 1] = (m_numSphereVertices - 1) - (i + 1);
        indices[static_cast<__int64>(k) + 2] = (m_numSphereVertices - 1) - (i + 2);
        k += 3;
    }

    indices[k] = m_numSphereVertices - 1;
    indices[static_cast<__int64>(k) + 1] = (m_numSphereVertices - 1) - LongLines;
    indices[static_cast<__int64>(k) + 2] = m_numSphereVertices - 2;

    static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(Vertex) * m_numSphereVertices;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = &vertices[0];
        hr = device->CreateBuffer(&desc, &data, &m_pVertexBuffer);
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        ZeroMemory(&desc, sizeof(desc));

        desc.ByteWidth = sizeof(UINT) * m_numSphereFaces * 3;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &indices[0];

        hr = device->CreateBuffer(&desc, &data, &m_pIndexBuffer);
        assert(SUCCEEDED(hr));
    }

    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;
    int flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    D3DInclude includeObj;

    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"TransVertexShader.hlsl", NULL, &includeObj, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, NULL);
        hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
    }
    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"TransPixelShader.hlsl", NULL, &includeObj, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, NULL);
        hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
    }
    if (SUCCEEDED(hr)) {
        int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
        hr = device->CreateInputLayout(InputDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout);
    }

    SAFE_RELEASE(vertexShaderBuffer);
    SAFE_RELEASE(pixelShaderBuffer);

    // Set constant buffers
    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(GeomBuffer) * MAX_LIGHT;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        GeomBuffer geomBufferInst[MAX_LIGHT];
        for (int i = 0; i < MAX_LIGHT; i++) {
            geomBufferInst[i].mWorldMatrix = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixTranslation(m_posColorVector[i].first.x, m_posColorVector[i].first.y, m_posColorVector[i].first.z);
            geomBufferInst[i].color = XMFLOAT4(m_posColorVector[i].second.x, m_posColorVector[i].second.y, m_posColorVector[i].second.z, 1.0f);
        }

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &geomBufferInst;
        data.SysMemPitch = sizeof(geomBufferInst);
        data.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&desc, &data, &m_pWorldMatrixBuffer);
        assert(SUCCEEDED(hr));
    }
    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(SceneMatrixBuffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&desc, nullptr, &m_pSceneMatrixBuffer);
        assert(SUCCEEDED(hr));
    }

    // Set rastrizer state
    if (SUCCEEDED(hr)) {
        D3D11_RASTERIZER_DESC desc = {};
        desc.AntialiasedLineEnable = false;
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0.0f;
        desc.FrontCounterClockwise = false;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = false;
        desc.SlopeScaledDepthBias = 0.0f;

        hr = device->CreateRasterizerState(&desc, &m_pRasterizerState);
        assert(SUCCEEDED(hr));
    }

    if (FAILED(hr)) {
        Release();
    }

    return hr;
}

// Clean up all the objects we've created
void Light::Release() {
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pRasterizerState);
    SAFE_RELEASE(m_pSceneMatrixBuffer);
    SAFE_RELEASE(m_pWorldMatrixBuffer);
    SAFE_RELEASE(m_pPixelShader);
    m_posColorVector.clear();
}

bool Light::Frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
    GeomBuffer geomBufferInst[MAX_LIGHT];
    for (int i = 0; i < m_posColorVector.size(); i++) {
        geomBufferInst[i].mWorldMatrix = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixTranslation(m_posColorVector[i].first.x, m_posColorVector[i].first.y, m_posColorVector[i].first.z);
        geomBufferInst[i].color = XMFLOAT4(m_posColorVector[i].second.x, m_posColorVector[i].second.y, m_posColorVector[i].second.z, 1.0f);
    }

    context->UpdateSubresource(m_pWorldMatrixBuffer, 0, nullptr, &geomBufferInst, 0, 0);

    // Update Scene matrix
    D3D11_MAPPED_SUBRESOURCE subresource;
    HRESULT hr = context->Map(m_pSceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    assert(SUCCEEDED(hr));
    if (SUCCEEDED(hr)) {
        SceneMatrixBuffer& sceneBuffer = *reinterpret_cast<SceneMatrixBuffer*>(subresource.pData);
        sceneBuffer.mViewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
        context->Unmap(m_pSceneMatrixBuffer, 0);
    }

    return SUCCEEDED(hr);
}

void Light::Render(ID3D11DeviceContext* context) {
    context->RSSetState(m_pRasterizerState);

    context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    ID3D11Buffer* vertexBuffers[] = { m_pVertexBuffer };
    UINT strides[] = { 12 };
    UINT offsets[] = { 0 };
    context->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    context->IASetInputLayout(m_pInputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_pVertexShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &m_pWorldMatrixBuffer);
    context->VSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);
    context->PSSetConstantBuffers(0, 1, &m_pWorldMatrixBuffer);
    context->PSSetShader(m_pPixelShader, nullptr, 0);

    context->DrawIndexedInstanced(m_numSphereFaces * 3, (UINT)m_posColorVector.size(), 0 ,0, 0);
}
