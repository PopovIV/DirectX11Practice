#include "scene.h"

// Initialize all needed instances
HRESULT Scene::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr)) {
        hr = InitScene(device, context);
    }

    if (FAILED(hr)) {
        Release();
    }

    return hr;
}

HRESULT Scene::InitScene(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr = S_OK;

    static const Vertex Vertices[] = {
        // Bottom face
        {-0.5, -0.5,  0.5, 0, 1},
        { 0.5, -0.5,  0.5, 1, 1},
        { 0.5, -0.5, -0.5, 1, 0},
        {-0.5, -0.5, -0.5, 0, 0},
        // Top face
        {-0.5,  0.5, -0.5, 0, 1},
        { 0.5,  0.5, -0.5, 1, 1},
        { 0.5,  0.5,  0.5, 1, 0},
        {-0.5,  0.5,  0.5, 0, 0},
        // Front face
        { 0.5, -0.5, -0.5, 0, 1},
        { 0.5, -0.5,  0.5, 1, 1},
        { 0.5,  0.5,  0.5, 1, 0},
        { 0.5,  0.5, -0.5, 0, 0},
        // Back face
        {-0.5, -0.5,  0.5, 0, 1},
        {-0.5, -0.5, -0.5, 1, 1},
        {-0.5,  0.5, -0.5, 1, 0},
        {-0.5,  0.5,  0.5, 0, 0},
        // Left face
        { 0.5, -0.5,  0.5, 0, 1},
        {-0.5, -0.5,  0.5, 1, 1},
        {-0.5,  0.5,  0.5, 1, 0},
        { 0.5,  0.5,  0.5, 0, 0},
        // Right face
        {-0.5, -0.5, -0.5, 0, 1},
        { 0.5, -0.5, -0.5, 1, 1},
        { 0.5,  0.5, -0.5, 1, 0},
        {-0.5,  0.5, -0.5, 0, 0}
    };
    static const USHORT Indices[] = {
        0, 2, 1, 0, 3, 2,
        4, 6, 5, 4, 7, 6,
        8, 10, 9, 8, 11, 10,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 22, 21, 20, 23, 22
    };
    static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(Vertices);
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &Vertices;
        data.SysMemPitch = sizeof(Vertices);
        data.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&desc, &data, &m_pVertexBuffer);
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(Indices);
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &Indices;
        data.SysMemPitch = sizeof(Indices);
        data.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&desc, &data, &m_pIndexBuffer);
        assert(SUCCEEDED(hr));
    }

    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;
    int flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, NULL);
        hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
    }
    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"PixelShader.hlsl", NULL, NULL, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, NULL);
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
        desc.ByteWidth = sizeof(WorldMatrixBuffer);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        WorldMatrixBuffer worldMatrixBuffer;
        worldMatrixBuffer.mWorldMatrix = DirectX::XMMatrixIdentity();

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &worldMatrixBuffer;
        data.SysMemPitch = sizeof(worldMatrixBuffer);
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
        desc.CullMode = D3D11_CULL_BACK;
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

    if (SUCCEEDED(hr)) {
        Texture tmp;
        hr = tmp.Init(device, context, L"data/morgana.dds");
        m_textureArray.push_back(tmp);
    }

    // Set sampler state
    if (SUCCEEDED(hr)) {
        D3D11_SAMPLER_DESC desc = {};

        desc.Filter = D3D11_FILTER_ANISOTROPIC;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MinLOD = -D3D11_FLOAT32_MAX;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 1.0f;

        hr = device->CreateSamplerState(&desc, &m_pSampler);
        assert(SUCCEEDED(hr));
    }

    return hr;
}

// Clean up all the objects we've created
void Scene::Release() {
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pRasterizerState);
    SAFE_RELEASE(m_pSceneMatrixBuffer);
    SAFE_RELEASE(m_pWorldMatrixBuffer);
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pSampler);

    for (auto t : m_textureArray) {
        t.Shutdown();
    }
    m_textureArray.clear();

}

bool Scene::Frame(ID3D11DeviceContext* context, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
    // Update world matrix
    WorldMatrixBuffer worldMatrixBuffer;

    worldMatrixBuffer.mWorldMatrix = worldMatrix;

    context->UpdateSubresource(m_pWorldMatrixBuffer, 0, nullptr, &worldMatrixBuffer, 0, 0);
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

void Scene::Render(ID3D11DeviceContext* context) {
    context->RSSetState(m_pRasterizerState);

    ID3D11SamplerState* samplers[] = { m_pSampler };
    context->PSSetSamplers(0, 1, samplers);

    ID3D11ShaderResourceView* resources[] = { m_textureArray[0].GetTexture() };
    context->PSSetShaderResources(0, 1, resources);

    context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    ID3D11Buffer* vertexBuffers[] = { m_pVertexBuffer };
    UINT strides[] = { 20 };
    UINT offsets[] = { 0 };
    context->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    context->IASetInputLayout(m_pInputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_pVertexShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &m_pWorldMatrixBuffer);
    context->VSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);
    context->PSSetShader(m_pPixelShader, nullptr, 0);
    context->DrawIndexed(36, 0, 0);
}
