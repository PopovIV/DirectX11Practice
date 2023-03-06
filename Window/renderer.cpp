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
        m_pScene = new Scene;
        if (!m_pScene) {
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
        hr = m_pScene->Init(m_pDevice, m_pContext);
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
    
    // Get the world matrix
    XMMATRIX mWorld = XMMatrixRotationY(t) * XMMatrixTranslation(m_cubePos.x, m_cubePos.y, m_cubePos.z);

    // Get the view matrix
    XMMATRIX mView;
    m_pCamera->GetBaseViewMatrix(mView);

    // Get the projection matrix
    XMMATRIX mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_width / (FLOAT)m_height, 0.01f, 100.0f);

    m_pScene->Frame(m_pContext, mWorld, mView, mProjection);
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

    // Render scene
    m_pScene->Render(m_pContext);

    HRESULT hr = m_pSwapChain->Present(0, 0);
    assert(SUCCEEDED(hr));

    return SUCCEEDED(hr);
}

// Clean up all the objects we've created
void Renderer::Cleanup() {
    SAFE_RELEASE(m_pBackBufferRTV);
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pContext);
    SAFE_RELEASE(m_pCamera);
    SAFE_RELEASE(m_pScene);
    SAFE_RELEASE(m_pCubeMap);
    SAFE_RELEASE(m_pInput);

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
