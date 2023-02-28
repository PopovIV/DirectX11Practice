// Renderer.h - class for initializing dx11 and basic rendering
#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>
#include "camera.h"
#include "input.h"
#include "texture.h"
#include "utility.h"
#include "cubeMap.h"

using namespace DirectX;

struct Vertex {
    float x, y, z;
    float u, v;
};

struct WorldMatrixBuffer {
    XMMATRIX mWorldMatrix;
};

struct SceneMatrixBuffer {
    XMMATRIX mViewProjectionMatrix;
};

class Renderer {
  public:
    // Initialize all needed instances
    bool Init(HINSTANCE hInstance, HWND hWnd);
    // Update the frame
    bool Frame();
    // Render the frame
    bool Render();
    // Clean up all the objects we've created
    void Cleanup();
    // Resize window
    bool Resize(UINT width, UINT height);

  private:
    // Function to calculate forward speed and movement
    void MoveForward(bool keydown);
    // Function to calculate backward speed and movement
    void MoveBackward(bool keydown);
    // Function to calculate left move speed and movement
    void MoveLeft(bool keydown);
    // Function to calculate right move speed and movement
    void MoveRight(bool keydown);
    // Function to calculate left turn speed and movement

    // Function to handle user input from keyboard/mouse
    void HandleMovementInput();
    HRESULT SetupBackBuffer();
    // Function to initialize scene's geometry
    HRESULT InitScene();

    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11Buffer* m_pWorldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
    ID3D11RasterizerState* m_pRasterizerState = nullptr;
    ID3D11SamplerState* m_pSampler = nullptr;

    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;


    Camera* m_pCamera = nullptr;
    Input* m_pInput = nullptr;
    CubeMap* m_pCubeMap = nullptr;
    std::vector<Texture> m_textureArray;

    XMFLOAT3 m_cubePos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    float m_forwardSpeed = 0.0f;
    float m_backwardSpeed = 0.0f;
    float m_leftSpeed = 0.0f;
    float m_rightSpeed = 0.0f;
    // constant for good movement
    float m_frameMove = 0.00001f;

    UINT m_width = 0;
    UINT m_height = 0;
};
