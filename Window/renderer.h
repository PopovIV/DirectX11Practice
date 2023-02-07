// Renderer.h - class for initializing dx11 and basic rendering
#pragma once

#include <d3d11.h>
#include <dxgi.h>

class Renderer {
  public:
    // Create Direct3D device and swap chain
    bool InitDevice(HWND hWnd);
    // Render the frame
    bool Render();
    // Clean up the objects we've created
    void CleanupDevice();
    // Resize window
    bool Resize(UINT width, UINT height);

  private:
    HRESULT SetupBackBuffer();

    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
    UINT m_width = 0;
    UINT m_height = 0;
};
