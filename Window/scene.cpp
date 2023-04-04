#include "scene.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// Initialize all needed instances
HRESULT Scene::Init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight) {
    HRESULT hr = S_OK;

    D3D11_QUERY_DESC desc;
    desc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
    desc.MiscFlags = 0;
    for (int i = 0; i < MAX_QUERY && SUCCEEDED(hr); i++) {
        hr = device->CreateQuery(&desc, &m_queries[i]);
    }

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

    if (SUCCEEDED(hr)) {
        m_pLight = new Light;
        if (!m_pLight) {
            hr = S_FALSE;
        }
    }

    if (SUCCEEDED(hr)) {
        hr = m_pLight->Init(device, context);
    }

    if (SUCCEEDED(hr)) {
        m_pFrustum = new Frustum;
        if (!m_pFrustum) {
            hr = S_FALSE;
        }
    }

    if (SUCCEEDED(hr)) {
        m_pFrustum->Init(SCREEN_NEAR);
    }

    if (FAILED(hr)) {
        Release();
    }

    return hr;
}

HRESULT Scene::InitScene(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr = S_OK;

    // Set up cubes
    for (int i = 0; i < MAX_CUBE; i++) {
        CubeModel tmp;
        float textureIndex = (float)(rand() % 2);
        tmp.pos = XMFLOAT4((float)(rand() % 10 - 5), (float)(rand() % 10 - 5), (float)(rand() % 10 - 5), (float)(rand() % 6 - 3));
        tmp.shineSpeedIdNM = XMFLOAT4(300.0f, (float)(rand() % 5), textureIndex, textureIndex > 0.0f ? 0.0f : 1.0f);
        m_cubeModelVector.push_back(tmp);
    }

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

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = sizeof(UINT);

        hr = device->CreateBuffer(&desc, nullptr, &m_pInderectArgsSrc);
        if (SUCCEEDED(hr)) {
            hr = device->CreateUnorderedAccessView(m_pInderectArgsSrc, nullptr, &m_pInderectArgsUAV);
        }
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
        desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&desc, nullptr, &m_pInderectArgs);
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(XMINT4) * MAX_CUBE;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = sizeof(XMINT4);

        hr = device->CreateBuffer(&desc, nullptr, &m_pGeomBufferInstVisGpu);
        if (SUCCEEDED(hr)) {
            hr = device->CreateUnorderedAccessView(m_pGeomBufferInstVisGpu, nullptr, &m_pGeomBufferInstVisGpu_UAV);
        }
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(XMINT4) * MAX_CUBE;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&desc, nullptr, &m_pGeomBufferInstVis);
        assert(SUCCEEDED(hr));
    }

    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;
    ID3D10Blob* computeShaderBuffer = nullptr;
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
        hr = D3DCompileFromFile(L"FrustumCullingShader.hlsl", NULL, &includeObj, "main", "cs_5_0", flags, 0, &computeShaderBuffer, NULL);
        hr = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &m_pCullShader);
    }
    if (SUCCEEDED(hr)) {
        int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
        hr = device->CreateInputLayout(InputDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout);
    }

    SAFE_RELEASE(vertexShaderBuffer);
    SAFE_RELEASE(pixelShaderBuffer);
    SAFE_RELEASE(computeShaderBuffer);

    // Set constant buffers
    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(GeomBuffer) * MAX_CUBE;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        GeomBuffer geomBufferInst[MAX_CUBE];
        CullParams cullParams;
        for (int i = 0; i < m_cubeModelVector.size(); i++) {
            geomBufferInst[i].mWorldMatrix = XMMatrixTranslation(m_cubeModelVector[i].pos.x, m_cubeModelVector[i].pos.y, m_cubeModelVector[i].pos.z);
            geomBufferInst[i].norm = geomBufferInst[i].mWorldMatrix;
            geomBufferInst[i].shineSpeedTexIdNM = m_cubeModelVector[i].shineSpeedIdNM;

            XMFLOAT4 min, max;
            XMStoreFloat4(&min, XMVector4Transform(XMLoadFloat4(&AABB[0]), geomBufferInst[i].mWorldMatrix));
            XMStoreFloat4(&max, XMVector4Transform(XMLoadFloat4(&AABB[1]), geomBufferInst[i].mWorldMatrix));
            cullParams.bbMin[i] = min;
            cullParams.bbMax[i] = max;
        }
        cullParams.numShapes = XMINT4(int(m_cubeModelVector.size()), 0, 0, 0);

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &geomBufferInst;
        data.SysMemPitch = sizeof(geomBufferInst);
        data.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&desc, &data, &m_pGeomBufferInst);
        assert(SUCCEEDED(hr));

        data.pSysMem = &cullParams;
        data.SysMemPitch = sizeof(cullParams);
        data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&desc, &data, &m_pCullParams);
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(SceneConstantBuffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&desc, nullptr, &m_pSceneConstantBuffer);
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(LightConstantBuffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&desc, nullptr, &m_pLightConstantBuffer);
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
        hr = tmp.InitArray(device, context, { L"data/brick_diffuse.dds", L"data/morgana.dds" });
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
        hr = D3DCompileFromFile(L"TransVertexShader.hlsl", Shader_Macros, &includeObj, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, NULL);
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
    SAFE_RELEASE(m_pSceneConstantBuffer);
    SAFE_RELEASE(m_pCullParams);
    SAFE_RELEASE(m_pLightConstantBuffer);
    SAFE_RELEASE(m_pGeomBufferInst);
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pCullShader);
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
    SAFE_RELEASE(m_pInderectArgsSrc);
    SAFE_RELEASE(m_pInderectArgs);
    SAFE_RELEASE(m_pGeomBufferInstVisGpu)
    SAFE_RELEASE(m_pGeomBufferInstVisGpu_UAV)
    SAFE_RELEASE(m_pGeomBufferInstVis)
    SAFE_RELEASE(m_pInderectArgsUAV);
    SAFE_RELEASE(m_pCubeMap);
    SAFE_RELEASE(m_pLight);
    SAFE_RELEASE(m_pFrustum);

    for (auto& q : m_queries) {
        q->Release();
    }

    for (auto& t : m_textureArray) {
        t.Shutdown();
    }
    m_textureArray.clear();
}

bool Scene::Frame(ID3D11DeviceContext* context, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    // Update our time
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0) {
        timeStart = timeCur;
    }
    t = (timeCur - timeStart) / 1000.0f;

    GeomBuffer geomBufferInst[MAX_CUBE];
    for (int i = 0; i < m_cubesCount; i++) {
        geomBufferInst[i].mWorldMatrix = XMMatrixRotationY(m_cubeModelVector[i].pos.w * t * m_cubeModelVector[i].shineSpeedIdNM.y) * XMMatrixTranslation(m_cubeModelVector[i].pos.x, m_cubeModelVector[i].pos.y, m_cubeModelVector[i].pos.z);
        geomBufferInst[i].norm = geomBufferInst[i].mWorldMatrix;
        geomBufferInst[i].shineSpeedTexIdNM = m_cubeModelVector[i].shineSpeedIdNM;
    }

    context->UpdateSubresource(m_pGeomBufferInst, 0, nullptr, &geomBufferInst, 0, 0);

    CullParams cullParams;
    // Calculate frustum
    m_pFrustum->ConstructFrustum(viewMatrix, projectionMatrix);
    // Find cubes in frustum
    m_cubeIndexies.clear();
    for (int i = 0; i < m_cubesCount; i++) {
        XMFLOAT4 min, max;
        XMStoreFloat4(&min, XMVector4Transform(XMLoadFloat4(&AABB[0]), geomBufferInst[i].mWorldMatrix));
        XMStoreFloat4(&max, XMVector4Transform(XMLoadFloat4(&AABB[1]), geomBufferInst[i].mWorldMatrix));
        if (m_pFrustum->CheckRectangle(min, max)) {
            m_cubeIndexies.push_back(i);
        }

        cullParams.bbMin[i] = min;
        cullParams.bbMax[i] = max;
    }
    cullParams.numShapes = XMINT4(m_cubesCount, 0, 0, 0);

    context->UpdateSubresource(m_pCullParams, 0, nullptr, &cullParams, 0, 0);

    // Update transparent world matrix
    WorldMatrixBuffer worldMatrixBuffer;
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
    HRESULT hr = context->Map(m_pSceneConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    assert(SUCCEEDED(hr));

    if (SUCCEEDED(hr)) {
        SceneConstantBuffer& sceneBuffer = *reinterpret_cast<SceneConstantBuffer*>(subresource.pData);
        sceneBuffer.mViewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
        XMFLOAT4* planes = m_pFrustum->GetPlanes();
        for (int i = 0; i < 6; i++) {
            sceneBuffer.planes[i] = planes[i];
        }
        context->Unmap(m_pSceneConstantBuffer, 0);
    }

    if (!m_computeCull) {
        XMINT4 indexBuffer[MAX_CUBE];
        for (int i = 0; i < m_cubeIndexies.size(); i++) {
            indexBuffer[i] = XMINT4(m_cubeIndexies[i], 0, 0, 0);
        }
        context->UpdateSubresource(m_pGeomBufferInstVis, 0, nullptr, &indexBuffer, 0, 0);
    }

    // Update Light buffer
    hr = context->Map(m_pLightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    assert(SUCCEEDED(hr));

    if (SUCCEEDED(hr)) {
        LightConstantBuffer& lightBuffer = *reinterpret_cast<LightConstantBuffer*>(subresource.pData);
        lightBuffer.cameraPos = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
        lightBuffer.ambientColor = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
        auto& lightPosColorVector = m_pLight->GetLightVector();
        lightBuffer.lightCount = XMINT4(int(lightPosColorVector.size()), m_useNormalMap ? 1 : 0, m_showNormals ? 1 : 0, 0);
        for (int i = 0; i < lightPosColorVector.size(); i++) {
            lightBuffer.lightPos[i] = XMFLOAT4(lightPosColorVector[i].first.x, lightPosColorVector[i].first.y, lightPosColorVector[i].first.z, 1.0f);
            lightBuffer.lightColor[i] = XMFLOAT4(lightPosColorVector[i].second.x, lightPosColorVector[i].second.y, lightPosColorVector[i].second.z, 1.0f);
        }
        context->Unmap(m_pLightConstantBuffer, 0);
    }

    // GPU Culling
    D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS args;
    args.IndexCountPerInstance = 36;
    args.InstanceCount = 0;
    args.StartInstanceLocation = 0;
    args.BaseVertexLocation = 0;
    args.StartIndexLocation = 0;
    context->UpdateSubresource(m_pInderectArgsSrc, 0, nullptr, &args, 0, 0);
    UINT groupNumber = m_cubesCount / 64u + !!(m_cubesCount % 64u);
    context->CSSetConstantBuffers(0, 1, &m_pCullParams);
    context->CSSetConstantBuffers(1, 1, &m_pSceneConstantBuffer);
    context->CSSetUnorderedAccessViews(0, 1, &m_pInderectArgsUAV, nullptr);
    context->CSSetUnorderedAccessViews(1, 1, &m_pGeomBufferInstVisGpu_UAV, nullptr);
    context->CSSetShader(m_pCullShader, nullptr, 0);
    context->Dispatch(groupNumber, 1, 1);

    context->CopyResource(m_pGeomBufferInstVis, m_pGeomBufferInstVisGpu);
    context->CopyResource(m_pInderectArgs, m_pInderectArgsSrc);

    m_pLight->Frame(context, viewMatrix, projectionMatrix);
    m_pCubeMap->Frame(context, viewMatrix, projectionMatrix, cameraPos);

    return SUCCEEDED(hr);
}

void Scene::CreateNewLight() {
    auto& lightPosColorVector = m_pLight->GetLightVector();
    if (lightPosColorVector.size() < MAX_LIGHT) {
        lightPosColorVector.push_back(std::pair<XMFLOAT3, XMFLOAT3>(
            XMFLOAT3(0.0f, 1.0f, 0.0f),
            XMFLOAT3(1.0f, 1.0f, 1.0f)));
    }
}

void Scene::DeleteLight() {
    auto& lightPosColorVector = m_pLight->GetLightVector();
    if (!lightPosColorVector.empty()) {
        lightPosColorVector.pop_back();
    }
};

void Scene::CreateNewCube() {
    if (m_cubesCount < MAX_CUBE) {
        m_cubesCount++;
    }
}

void Scene::DeleteCube() {
    if (m_cubesCount > 0) {
        m_cubesCount--;
    }
}

// Function to get info from Queries
void Scene::ReadQueries(ID3D11DeviceContext* context) {
    D3D11_QUERY_DATA_PIPELINE_STATISTICS stats;
    while (m_lastCompletedFrame < m_curFrame) {
        HRESULT hr = context->GetData(m_queries[m_lastCompletedFrame % MAX_QUERY], &stats, sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS), 0);
        if (hr == S_OK) {
            m_cubesCountGPU = int(stats.IAPrimitives / 12);
            m_lastCompletedFrame++;
        }
        else {
            break;
        }
    }
}

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
    context->VSSetConstantBuffers(0, 1, &m_pGeomBufferInst);
    context->VSSetConstantBuffers(1, 1, &m_pSceneConstantBuffer);
    context->VSSetConstantBuffers(2, 1, &m_pGeomBufferInstVis);
    context->PSSetShader(m_pPixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &m_pGeomBufferInst);
    context->PSSetConstantBuffers(1, 1, &m_pSceneConstantBuffer);
    context->PSSetConstantBuffers(2, 1, &m_pLightConstantBuffer);

    if (m_isCullingOn) {
        if (m_computeCull) {
            context->Begin(m_queries[m_curFrame % MAX_QUERY]);
            context->DrawIndexedInstancedIndirect(m_pInderectArgs, 0);
            context->End(m_queries[m_curFrame % MAX_QUERY]);
            m_curFrame++;
        }
        else {
            context->DrawIndexedInstanced(36, (UINT)m_cubeIndexies.size(), 0, 0, 0);
        }
    }
    else {
        context->DrawIndexedInstanced(36, MAX_CUBE, 0, 0, 0);
    }
    ReadQueries(context);

    // Render Spheres
    if (m_isSpheresOn) {
        m_pLight->Render(context);
    }
    m_pCubeMap->Render(context);

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
    context->VSSetConstantBuffers(1, 1, &m_pSceneConstantBuffer);
    context->PSSetConstantBuffers(2, 1, &m_pLightConstantBuffer);

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
