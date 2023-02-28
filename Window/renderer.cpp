#include "renderer.h"
#include <assert.h>

// Create Direct3D device and swap chain
bool Renderer::Init(HINSTANCE hInstance, HWND hWnd) {
    HRESULT hr;

    // Create a DirectX graphics interface factory.
    IDXGIFactory* pFactory = nullptr;
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

    // Select hardware adapter
    IDXGIAdapter* pSelectedAdapter = NULL;
    if (SUCCEEDED(hr)) {
        IDXGIAdapter* pAdapter = NULL;
        UINT adapterIdx = 0;
        while (SUCCEEDED(pFactory->EnumAdapters(adapterIdx, &pAdapter))) {
            DXGI_ADAPTER_DESC desc;
            pAdapter->GetDesc(&desc);

            // Skip
            if (wcscmp(desc.Description, L"Microsoft Basic Render Driver") != 0) {
                pSelectedAdapter = pAdapter;
                break;
            }

            pAdapter->Release();
            adapterIdx++;
        }
    }
    assert(pSelectedAdapter != NULL);

    // Create DirectX 11 device
    D3D_FEATURE_LEVEL level;
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
    if (SUCCEEDED(hr)) {
        UINT flags = 0;
    #ifdef _DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif
        hr = D3D11CreateDevice(pSelectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
            flags, levels, 1, D3D11_SDK_VERSION, &m_pDevice, &level, &m_pContext);
        assert(level == D3D_FEATURE_LEVEL_11_0);
        assert(SUCCEEDED(hr));
    }

    // Create swapchain
    if (SUCCEEDED(hr)) {
        RECT rc;
        GetClientRect(hWnd, &rc);
        m_width = rc.right - rc.left;
        m_height = rc.bottom - rc.top;

        DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Width = m_width;
        swapChainDesc.BufferDesc.Height = m_height;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = hWnd;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Windowed = true;
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Flags = 0;

        hr = pFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        hr = SetupBackBuffer();
    }

    if (SUCCEEDED(hr)) {
        hr = InitScene();
    }

    SAFE_RELEASE(pSelectedAdapter);
    SAFE_RELEASE(pFactory);

    if (SUCCEEDED(hr)) {
        m_pCamera = new Camera;
        if (!m_pCamera) {
            hr = S_FALSE;
        }
    }

    if (SUCCEEDED(hr)) {
        m_pInput = new Input;
        if (!m_pInput) {
            hr = S_FALSE;
        }
    }

    if (SUCCEEDED(hr)) {
        m_pCubeMap = new CubeMap;
        if (!m_pCubeMap) {
            hr = S_FALSE;
        }
    }

    if (SUCCEEDED(hr)) {
        hr = m_pCamera->Init();
    }

    if (SUCCEEDED(hr)) {
        hr = m_pInput->Init(hInstance, hWnd, m_width, m_height);
    }

    if (SUCCEEDED(hr)) {
        hr = m_pCubeMap->Init(m_pDevice, m_pContext, m_width, m_height);
    }

    if (FAILED(hr)) {
        Cleanup();
    }

    return SUCCEEDED(hr);
}

// Function to handle user input from keyboard/mouse
void Renderer::HandleMovementInput() {
    bool keyDown;
    XMFLOAT3 mouseMove = m_pInput->IsMouseUsed();
    m_pCamera->MouseMoved(mouseMove.x, mouseMove.y, mouseMove.z);
    keyDown = m_pInput->IsLeftPressed();
    MoveLeft(keyDown);
    keyDown = m_pInput->IsRightPressed();
    MoveRight(keyDown);
    keyDown = m_pInput->IsUpPressed();
    MoveForward(keyDown);
    keyDown = m_pInput->IsDownPressed();
    MoveBackward(keyDown);
}


// Function to calculate forward speed and movement
void Renderer::MoveForward(bool keydown) {
    // Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_forwardSpeed += m_frameMove * 1.0f;
        if (m_forwardSpeed > m_frameMove * 50.0f) {
            m_forwardSpeed = m_frameMove * 50.0f;
        }
    }
    else {
        m_forwardSpeed -= m_frameMove * 0.5f;
        if (m_forwardSpeed < 0.0f) {
            m_forwardSpeed = 0.0f;
        }
    }

    m_cubePos.x +=  m_forwardSpeed;

}

// Function to calculate backward speed and movement
void Renderer::MoveBackward(bool keydown) {
    // Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_backwardSpeed += m_frameMove * 1.0f;
        if (m_backwardSpeed > (m_frameMove * 50.0f)) {
            m_backwardSpeed = m_frameMove * 50.0f;
        }
    }
    else {
        m_backwardSpeed -= m_frameMove * 0.5f;
        if (m_backwardSpeed < 0.0f) {
            m_backwardSpeed = 0.0f;
        }
    }

    m_cubePos.x -= m_backwardSpeed;
}

// Function to calculate left move speed and movement
void Renderer::MoveLeft(bool keydown) {
    // Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_leftSpeed += m_frameMove * 1.0f;
        if (m_leftSpeed > (m_frameMove * 50.0f)) {
            m_leftSpeed = m_frameMove * 50.0f;
        }
    }
    else {
        m_leftSpeed -= m_frameMove * 0.5f;
        if (m_leftSpeed < 0.0f) {
            m_leftSpeed = 0.0f;
        }
    }

    m_cubePos.z += m_leftSpeed;
}

// Function to calculate right move speed and movement
void Renderer::MoveRight(bool keydown) {
    // Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_rightSpeed += m_frameMove * 1.0f;
        if (m_rightSpeed > (m_frameMove * 50.0f)) {
            m_rightSpeed = m_frameMove * 50.0f;
        }
    }
    else {
        m_rightSpeed -= m_frameMove * 0.5f;
        if (m_rightSpeed < 0.0f) {
            m_rightSpeed = 0.0f;
        }
    }

    m_cubePos.z -= m_rightSpeed;
};

// Update the frame
bool Renderer::Frame() {
    HRESULT hr = S_OK;

    m_pCamera->Frame();
    m_pInput->Frame();

    HandleMovementInput();

    // Update our time
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0) {
        timeStart = timeCur;
    }
    t = (timeCur - timeStart) / 1000.0f;
    
    // Update world matrix
    WorldMatrixBuffer worldMatrixBuffer;

    worldMatrixBuffer.mWorldMatrix = XMMatrixRotationY(t) * XMMatrixTranslation(m_cubePos.x, m_cubePos.y, m_cubePos.z);

    m_pContext->UpdateSubresource(m_pWorldMatrixBuffer, 0, nullptr, &worldMatrixBuffer, 0, 0);

    // Get the view matrix
    XMMATRIX mView;
    m_pCamera->GetBaseViewMatrix(mView);

    // Get the projection matrix
    XMMATRIX mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_width / (FLOAT)m_height, 0.01f, 100.0f);
    
    // Update Scene matrix
    D3D11_MAPPED_SUBRESOURCE subresource;
    hr = m_pContext->Map(m_pSceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    assert(SUCCEEDED(hr));
    if (SUCCEEDED(hr)) {
        SceneMatrixBuffer& sceneBuffer = *reinterpret_cast<SceneMatrixBuffer*>(subresource.pData);
        sceneBuffer.mViewProjectionMatrix = XMMatrixMultiply(mView, mProjection);
        m_pContext->Unmap(m_pSceneMatrixBuffer, 0);
    }

    m_pCubeMap->Frame(m_pContext, mView, mProjection, m_pCamera->GetCameraPosition());

    return SUCCEEDED(hr);
}

// Render the frame
bool Renderer::Render() {
    m_pContext->ClearState();

    ID3D11RenderTargetView* views[] = { m_pBackBufferRTV };
    m_pContext->OMSetRenderTargets(1, views, nullptr);
    static const FLOAT BackColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    m_pContext->ClearRenderTargetView(m_pBackBufferRTV, BackColor);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)m_width;
    viewport.Height = (FLOAT)m_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_pContext->RSSetViewports(1, &viewport);

    D3D11_RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = m_width;
    rect.bottom = m_height;
    m_pContext->RSSetScissorRects(1, &rect);

    // Render cube map
    m_pCubeMap->Render(m_pContext);

    m_pContext->RSSetState(m_pRasterizerState);

    ID3D11SamplerState* samplers[] = { m_pSampler };
    m_pContext->PSSetSamplers(0, 1, samplers);

    ID3D11ShaderResourceView* resources[] = { m_textureArray[0].GetTexture() };
    m_pContext->PSSetShaderResources(0, 1, resources);

    m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    ID3D11Buffer* vertexBuffers[] = { m_pVertexBuffer };
    UINT strides[] = { 20 };
    UINT offsets[] = { 0 };
    m_pContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    m_pContext->IASetInputLayout(m_pInputLayout);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pContext->VSSetConstantBuffers(0, 1, &m_pWorldMatrixBuffer);
    m_pContext->VSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);
    m_pContext->PSSetShader(m_pPixelShader, nullptr, 0);
    m_pContext->DrawIndexed(36, 0, 0);

    HRESULT hr = m_pSwapChain->Present(0, 0);
    assert(SUCCEEDED(hr));

    return SUCCEEDED(hr);
}

// Clean up all the objects we've created
void Renderer::Cleanup() {
    SAFE_RELEASE(m_pSampler);
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pRasterizerState);
    SAFE_RELEASE(m_pSceneMatrixBuffer);
    SAFE_RELEASE(m_pWorldMatrixBuffer);
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pBackBufferRTV);
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pContext);
    
    if (m_pCamera) {
        m_pCamera->Release();
        delete m_pCamera;
        m_pCamera = nullptr;
    }

    for (auto t : m_textureArray) {
        t.Shutdown();
    }
    m_textureArray.clear();

    if (m_pCubeMap) {
        m_pCubeMap->Realese();
        delete m_pCubeMap;
        m_pCubeMap = nullptr;
    }

    #ifdef _DEBUG
    if (m_pDevice != nullptr) {
        ID3D11Debug* pDebug = nullptr;
        HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&pDebug);
        assert(SUCCEEDED(hr));
        if (pDebug != nullptr) {
            if (pDebug->AddRef() != 3) {
                pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
            }
            pDebug->Release();
            SAFE_RELEASE(pDebug);
        }
    }
    #endif

    SAFE_RELEASE(m_pDevice);
}

// Resize window
bool Renderer::Resize(UINT width, UINT height) {
    if (width != m_width || height != m_height) {
        SAFE_RELEASE(m_pBackBufferRTV);

        HRESULT hr = m_pSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        assert(SUCCEEDED(hr));
        if (SUCCEEDED(hr)) {
            m_width = width;
            m_height = height;

            hr = SetupBackBuffer();
            m_pInput->Resize(width, height);
            m_pCubeMap->Resize(width, height);
        }
        return SUCCEEDED(hr);
    }

    return true;
}

HRESULT Renderer::SetupBackBuffer() {
    ID3D11Texture2D* pBackBuffer = NULL;
    HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    assert(SUCCEEDED(hr));
    if (SUCCEEDED(hr)) {
        hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackBufferRTV);
        assert(SUCCEEDED(hr));

        SAFE_RELEASE(pBackBuffer);
    }

    return hr;
}

// Function to initialize scene's geometry
HRESULT Renderer::InitScene() {
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

        hr = m_pDevice->CreateBuffer(&desc, &data, &m_pVertexBuffer);
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

        hr = m_pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer);
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
        hr = m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
    }
    if (SUCCEEDED(hr)) {
        hr = D3DCompileFromFile(L"PixelShader.hlsl", NULL, NULL, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, NULL);
        hr = m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
    }
    if (SUCCEEDED(hr)) {
        int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
        hr = m_pDevice->CreateInputLayout(InputDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout);
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

        hr = m_pDevice->CreateBuffer(&desc, &data, &m_pWorldMatrixBuffer);
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

        hr = m_pDevice->CreateBuffer(&desc, nullptr, &m_pSceneMatrixBuffer);
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

        hr = m_pDevice->CreateRasterizerState(&desc, &m_pRasterizerState);
        assert(SUCCEEDED(hr));
    }

    if (SUCCEEDED(hr)) {
        Texture tmp;
        hr = tmp.Init(m_pDevice, m_pContext, L"data/morgana.dds");
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

        hr = m_pDevice->CreateSamplerState(&desc, &m_pSampler);
        assert(SUCCEEDED(hr));
    }

    return hr;
}
