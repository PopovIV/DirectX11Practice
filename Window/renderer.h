// Renderer.h - class for initializing dx11 and basic rendering
#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>

using namespace DirectX;

struct Vertex {
    float x, y, z;
    COLORREF color;
};

class Renderer {
  public:
    // Create Direct3D device and swap chain
    bool InitDevice(HWND hWnd);
    // Render the frame
    bool Render();
    // Clean up all the objects we've created
    void Cleanup();
    // Resize window
    bool Resize(UINT width, UINT height);

  private:
    HRESULT SetupBackBuffer();
    // Function to initialize scene's geometry
    HRESULT InitScene();

    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    UINT m_width = 0;
    UINT m_height = 0;
};
