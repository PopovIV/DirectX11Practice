// scene.h - class for rendering scene's geometry
#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>
#include "cubemap.h"
#include "texture.h"
#include "DDSTextureLoader.h"
#include "utility.h"

using namespace DirectX;

static const XMFLOAT4 Vertices[] = {
    {0, -1, -1, 1},
    {0,  1, -1, 1},
    {0,  1,  1, 1},
    {0, -1,  1, 1}
};

class Scene {
private:

    struct Vertex {
        float x, y, z;
        float u, v;
    };

    struct WorldMatrixBuffer {
        XMMATRIX mWorldMatrix;
        XMFLOAT4 color;
    };

    struct SceneMatrixBuffer {
        XMMATRIX mViewProjectionMatrix;
    };
public:
    // Initialize all needed instances
    HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight);
    // Clean up all the objects we've created
    void Release();
    // Resize function
    void Resize(int screenWidth, int screenHeight) { m_pCubeMap->Resize(screenWidth, screenHeight); };
    // Render function
    void Render(ID3D11DeviceContext* context);
    // Render the frame
    bool Frame(ID3D11DeviceContext* context, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);

private:
    // Function to initialize scene's geometry
    HRESULT InitScene(ID3D11Device* device, ID3D11DeviceContext* context);
    // Function to initialize transperent scene's geometry
    HRESULT InitSceneTransparent(ID3D11Device* device, ID3D11DeviceContext* context);
    // Render transperent part
    void RenderTransparent(ID3D11DeviceContext* context);

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11Buffer* m_pWorldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pWorldMatrixBuffer2 = nullptr;
    ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
    ID3D11RasterizerState* m_pRasterizerState = nullptr;
    ID3D11SamplerState* m_pSampler = nullptr;
    ID3D11DepthStencilState* m_pDepthState = nullptr;

    ID3D11Buffer* m_pTransVertexBuffer = nullptr;
    ID3D11Buffer* m_pTransIndexBuffer = nullptr;
    ID3D11Buffer* m_pTransWorldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pTransWorldMatrixBuffer2 = nullptr;
    ID3D11RasterizerState* m_pTransRasterizerState = nullptr;
    ID3D11DepthStencilState* m_pTransDepthState = nullptr;
    ID3D11BlendState* m_pTransBlendState = nullptr;

    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    ID3D11InputLayout* m_pTransInputLayout = nullptr;
    ID3D11VertexShader* m_pTransVertexShader = nullptr;
    ID3D11PixelShader* m_pTransPixelShader = nullptr;

    CubeMap* m_pCubeMap = nullptr;
    std::vector<Texture> m_textureArray;

    // flag to know what rect to draw
    bool m_yellowRect = false;
};
