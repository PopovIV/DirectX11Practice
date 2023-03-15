#include "scene.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// Initialize all needed instances
HRESULT Scene::Init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight) {
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr)) {
        hr = InitScene(device, context);
    }

    if (SUCCEEDED(hr)) {
        hr = InitSceneTransparent(device, context);
    }

    if (SUCCEEDED(hr)) {
        m_pCubeMap = new CubeMap;
        if (!m_pCubeMap) {
            hr = S_FALSE;
        }
    }

    if (SUCCEEDED(hr)) {
        hr = m_pCubeMap->Init(device, context, screenWidth, screenHeight);
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
        {{-0.5, -0.5,  0.5}, {0,1}, {0,-1,0}, {1,0,0}},
        {{ 0.5, -0.5,  0.5}, {1,1}, {0,-1,0}, {1,0,0}},
        {{ 0.5, -0.5, -0.5}, {1,0}, {0,-1,0}, {1,0,0}},
        {{-0.5, -0.5, -0.5}, {0,0}, {0,-1,0}, {1,0,0}},
        // Top face
        {{-0.5,  0.5, -0.5}, {0,1}, {0,1,0}, {1,0,0}},
        {{ 0.5,  0.5, -0.5}, {1,1}, {0,1,0}, {1,0,0}},
        {{ 0.5,  0.5,  0.5}, {1,0}, {0,1,0}, {1,0,0}},
        {{-0.5,  0.5,  0.5}, {0,0}, {0,1,0}, {1,0,0}},
        // Front face
        {{ 0.5, -0.5, -0.5}, {0,1}, {1,0,0}, {0,0,1}},
        {{ 0.5, -0.5,  0.5}, {1,1}, {1,0,0}, {0,0,1}},
        {{ 0.5,  0.5,  0.5}, {1,0}, {1,0,0}, {0,0,1}},
        {{ 0.5,  0.5, -0.5}, {0,0}, {1,0,0}, {0,0,1}},
        // Back face
        {{-0.5, -0.5,  0.5}, {0,1}, {-1,0,0}, {0,0,-1}},
        {{-0.5, -0.5, -0.5}, {1,1}, {-1,0,0}, {0,0,-1}},
        {{-0.5,  0.5, -0.5}, {1,0}, {-1,0,0}, {0,0,-1}},
        {{-0.5,  0.5,  0.5}, {0,0}, {-1,0,0}, {0,0,-1}},
        // Left face
        {{ 0.5, -0.5,  0.5}, {0,1}, {0,0,1}, {-1,0,0}},
        {{-0.5, -0.5,  0.5}, {1,1}, {0,0,1}, {-1,0,0}},
        {{-0.5,  0.5,  0.5}, {1,0}, {0,0,1}, {-1,0,0}},
        {{ 0.5,  0.5,  0.5}, {0,0}, {0,0,1}, {-1,0,0}},
        // Right face
        {{-0.5, -0.5, -0.5}, {0,1}, {0,0,-1}, {1,0,0}},
        {{ 0.5, -0.5, -0.5}, {1,1}, {0,0,-1}, {1,0,0}},
        {{ 0.5,  0.5, -0.5}, {1,0}, {0,0,-1}, {1,0,0}},
        {{-0.5,  0.5, -0.5}, {0,0}, {0,0,-1}, {1,0,0}},
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
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
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

    D3DInclude includeObj;

    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"VertexShader.hlsl", NULL, &includeObj, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, NULL);
        hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
    }
    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"PixelShader.hlsl", NULL, &includeObj, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, NULL);
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
        if (SUCCEEDED(hr)) {
            hr = device->CreateBuffer(&desc, &data, &m_pWorldMatrixBuffer2);
        }
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
        hr = tmp.Init(device, context, L"data/brick_diffuse.dds");
        m_textureArray.push_back(tmp);
    }

    if (SUCCEEDED(hr)) {
        Texture tmp;
        hr = tmp.Init(device, context, L"data/brick_normal.dds");
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

    // Create depth state
    if (SUCCEEDED(hr)) {
        D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
        dsDesc.StencilEnable = FALSE;

        hr = device->CreateDepthStencilState(&dsDesc, &m_pDepthState);
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        // Init one light
        Light l;
        hr = l.Init(device, context);
        assert(SUCCEEDED(hr));
        m_lights.push_back(l);
    }

    return hr;
}

HRESULT Scene::InitSceneTransparent(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr = S_OK;

    static const USHORT Indices[] = {
        0, 2, 1, 0, 3, 2
    };
    static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

        hr = device->CreateBuffer(&desc, &data, &m_pTransVertexBuffer);
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

        hr = device->CreateBuffer(&desc, &data, &m_pTransIndexBuffer);
        assert(SUCCEEDED(hr));
    }

    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;
    int flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    D3DInclude includeObj;
    D3D_SHADER_MACRO Shader_Macros[] = { {"USE_LIGHTS"}, {NULL, NULL} };

    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"TransVertexShader.hlsl", NULL, NULL, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, NULL);
        hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_pTransVertexShader);
    }
    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"TransPixelShader.hlsl", Shader_Macros, &includeObj, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, NULL);
        hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pTransPixelShader);
    }
    if (SUCCEEDED(hr)) {
        int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
        hr = device->CreateInputLayout(InputDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pTransInputLayout);
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

        hr = device->CreateBuffer(&desc, &data, &m_pTransWorldMatrixBuffer);
        if (SUCCEEDED(hr)) {
            hr = device->CreateBuffer(&desc, &data, &m_pTransWorldMatrixBuffer2);
        }
        assert(SUCCEEDED(hr));
    }

    // Create rasterizer state
    if (SUCCEEDED(hr)) {
        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.FrontCounterClockwise = false;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0.0f;
        desc.DepthBiasClamp = 0.0f;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = false;
        desc.AntialiasedLineEnable = false;

        hr = device->CreateRasterizerState(&desc, &m_pTransRasterizerState);
        assert(SUCCEEDED(hr));
    }

    // Create blend state
    if (SUCCEEDED(hr)) {
        D3D11_BLEND_DESC desc = { 0 };
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED |
            D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

        hr = device->CreateBlendState(&desc, &m_pTransBlendState);
        assert(SUCCEEDED(hr));
    }

    // Create depth state
    if (SUCCEEDED(hr)) {
        D3D11_DEPTH_STENCIL_DESC desc = { 0 };
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_GREATER;
        desc.StencilEnable = FALSE;

        hr = device->CreateDepthStencilState(&desc, &m_pTransDepthState);
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
    SAFE_RELEASE(m_pWorldMatrixBuffer2);
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pSampler);
    SAFE_RELEASE(m_pDepthState);
    SAFE_RELEASE(m_pTransVertexBuffer);
    SAFE_RELEASE(m_pTransIndexBuffer);
    SAFE_RELEASE(m_pTransInputLayout);
    SAFE_RELEASE(m_pTransVertexShader);
    SAFE_RELEASE(m_pTransPixelShader);
    SAFE_RELEASE(m_pTransRasterizerState);
    SAFE_RELEASE(m_pTransWorldMatrixBuffer);
    SAFE_RELEASE(m_pTransWorldMatrixBuffer2);
    SAFE_RELEASE(m_pTransDepthState);
    SAFE_RELEASE(m_pTransBlendState);
    SAFE_RELEASE(m_pCubeMap);

    for (auto& t : m_textureArray) {
        t.Shutdown();
    }
    m_textureArray.clear();

    for (auto& t : m_lights) {
        t.Shutdown();
    }
    m_lights.clear();
}

bool Scene::Frame(ID3D11DeviceContext* context, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    WorldMatrixBuffer worldMatrixBuffer;
    // Update world matrix 1
    worldMatrixBuffer.mWorldMatrix = worldMatrix * XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
    worldMatrixBuffer.color = XMFLOAT4(300.0f, 0.0f, 0.0f, 0.0f);

    context->UpdateSubresource(m_pWorldMatrixBuffer, 0, nullptr, &worldMatrixBuffer, 0, 0);
    // Update world matrix 2
    worldMatrixBuffer.mWorldMatrix = XMMatrixTranslation(2.0f, 0.0f, 2.0f);

    context->UpdateSubresource(m_pWorldMatrixBuffer2, 0, nullptr, &worldMatrixBuffer, 0, 0);
    // Update transparent world matrix
    worldMatrixBuffer.mWorldMatrix = XMMatrixTranslation(0.8f, 0.3f, 1.1f);
    worldMatrixBuffer.color = XMFLOAT4(0.6f, 0.0f, 1.0f, 0.5f); // purple
    context->UpdateSubresource(m_pTransWorldMatrixBuffer, 0, nullptr, &worldMatrixBuffer, 0, 0);

    // Calculate distance between rectangle points and camera
    XMFLOAT4 rectVert[4];
    float maxDist = -D3D11_FLOAT32_MAX;
    std::copy(Vertices, Vertices + 4, rectVert);
    for (int i = 0; i < 4; i++) {
        XMStoreFloat4(&rectVert[i], XMVector4Transform(XMLoadFloat4(&rectVert[i]), worldMatrixBuffer.mWorldMatrix));
        float dist = (rectVert[i].x * cameraPos.x) + (rectVert[i].y * cameraPos.y) + (rectVert[i].z * cameraPos.z);
        maxDist = max(maxDist, dist);
    }

    // Update transparent world matrix
    worldMatrixBuffer.mWorldMatrix = XMMatrixTranslation(1.1f, 0.0f, 1.3f);
    worldMatrixBuffer.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.5f); // yellow
    context->UpdateSubresource(m_pTransWorldMatrixBuffer2, 0, nullptr, &worldMatrixBuffer, 0, 0);

    // Calculate distance between second rectangle points and camera
    float maxDist2 = -D3D11_FLOAT32_MAX;
    std::copy(Vertices, Vertices + 4, rectVert);
    for (int i = 0; i < 4; i++) {
        XMStoreFloat4(&rectVert[i], XMVector4Transform(XMLoadFloat4(&rectVert[i]), worldMatrixBuffer.mWorldMatrix));
        float dist = (rectVert[i].x * cameraPos.x) + (rectVert[i].y * cameraPos.y) + (rectVert[i].z * cameraPos.z);
        maxDist2 = max(maxDist2, dist);
    }

    m_yellowRect = maxDist2 < maxDist;

    // Update Scene matrix
    D3D11_MAPPED_SUBRESOURCE subresource;
    HRESULT hr = context->Map(m_pSceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    assert(SUCCEEDED(hr));
    if (SUCCEEDED(hr)) {
        SceneMatrixBuffer& sceneBuffer = *reinterpret_cast<SceneMatrixBuffer*>(subresource.pData);
        sceneBuffer.mViewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
        sceneBuffer.cameraPos = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
        sceneBuffer.ambientColor = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
        sceneBuffer.lightCount = XMINT4(int(m_lights.size()), m_useNormalMap ? 1 : 0, m_showNormals ? 1 : 0, 0);
        for (int i = 0; i < m_lights.size(); i++) {
            sceneBuffer.lightPos[i] = m_lights[i].GetPosition();
            sceneBuffer.lightColor[i] = m_lights[i].GetColor();
        }
        context->Unmap(m_pSceneMatrixBuffer, 0);
    }

    for (auto& l : m_lights) {
        l.Frame(context, viewMatrix, projectionMatrix);
    }

    m_pCubeMap->Frame(context, viewMatrix, projectionMatrix, cameraPos);

    return SUCCEEDED(hr);
}

HRESULT Scene::CreateNewLight(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr = S_OK;

    if (m_lights.size() < MAX_LIGHT) {
        // Init one light
        Light l;
        hr = l.Init(device, context);
        assert(SUCCEEDED(hr));
        m_lights.push_back(l);
    }

    return hr;
}

void Scene::DeleteLight() {
    if (!m_lights.empty()) {
        m_lights[m_lights.size() - 1].Shutdown();
        m_lights.pop_back();
    }
};

void Scene::Render(ID3D11DeviceContext* context) {
    context->OMSetDepthStencilState(m_pDepthState, 0);

    context->RSSetState(m_pRasterizerState);

    ID3D11SamplerState* samplers[] = { m_pSampler };
    context->PSSetSamplers(0, 1, samplers);

    ID3D11ShaderResourceView* resources[] = { m_textureArray[0].GetTexture(), m_textureArray[1].GetTexture() };
    context->PSSetShaderResources(0, 2, resources);

    context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    ID3D11Buffer* vertexBuffers[] = { m_pVertexBuffer };
    UINT strides[] = { sizeof(Vertex) };
    UINT offsets[] = { 0 };
    context->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    context->IASetInputLayout(m_pInputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_pVertexShader, nullptr, 0);
    context->VSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);
    context->PSSetShader(m_pPixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &m_pWorldMatrixBuffer);
    context->PSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);

    // Draw first cube
    {
        context->VSSetConstantBuffers(0, 1, &m_pWorldMatrixBuffer);
        context->DrawIndexed(36, 0, 0);
    }

    // Draw second cube
    {
        context->VSSetConstantBuffers(0, 1, &m_pWorldMatrixBuffer2);
        context->DrawIndexed(36, 0, 0);
    }

    // Render Spheres
    if (m_isSpheresOn) {
        for (auto& l : m_lights) {
            l.Render(context);
        }
    }

    //m_pCubeMap->Render(context);

    RenderTransparent(context);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void Scene::RenderTransparent(ID3D11DeviceContext* context) {
    context->IASetIndexBuffer(m_pTransIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    ID3D11Buffer* vertexBuffers[] = { m_pTransVertexBuffer };
    UINT stride = sizeof(XMFLOAT4);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
    context->IASetInputLayout(m_pTransInputLayout);

    context->RSSetState(m_pRasterizerState);
    context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->RSSetState(m_pTransRasterizerState);

    context->VSSetShader(m_pTransVertexShader, nullptr, 0);
    context->PSSetShader(m_pTransPixelShader, nullptr, 0);
    context->VSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);

    context->OMSetBlendState(m_pTransBlendState, nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_pTransDepthState, 0);

    if (m_yellowRect) {
        {
            context->VSSetConstantBuffers(0, 1, &m_pTransWorldMatrixBuffer2);
            context->PSSetConstantBuffers(0, 1, &m_pTransWorldMatrixBuffer2);

            context->DrawIndexed(6, 0, 0);
        }
        {
            context->VSSetConstantBuffers(0, 1, &m_pTransWorldMatrixBuffer);
            context->PSSetConstantBuffers(0, 1, &m_pTransWorldMatrixBuffer);

            context->DrawIndexed(6, 0, 0);
        }
    }
    else {
        {
            context->VSSetConstantBuffers(0, 1, &m_pTransWorldMatrixBuffer);
            context->PSSetConstantBuffers(0, 1, &m_pTransWorldMatrixBuffer);

            context->DrawIndexed(6, 0, 0);
        }
        {
            context->VSSetConstantBuffers(0, 1, &m_pTransWorldMatrixBuffer2);
            context->PSSetConstantBuffers(0, 1, &m_pTransWorldMatrixBuffer2);

            context->DrawIndexed(6, 0, 0);
        }
    }
}