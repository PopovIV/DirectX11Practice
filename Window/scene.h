// scene.h - class for rendering scene's geometry
#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>
#include "texture.h"
#include "DDSTextureLoader.h"
#include "utility.h"

using namespace DirectX;

class Scene {
private:
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
public:
    // Initialize all needed instances
    HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context);
    // Clean up all the objects we've created
    void Release();
    // Render function
    void Render(ID3D11DeviceContext* context);
    // Render the frame
    bool Frame(ID3D11DeviceContext* context, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

private:
    // Function to initialize scene's geometry
    HRESULT InitScene(ID3D11Device* device, ID3D11DeviceContext* context);

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11Buffer* m_pWorldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
    ID3D11RasterizerState* m_pRasterizerState = nullptr;
    ID3D11SamplerState* m_pSampler = nullptr;

    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    std::vector<Texture> m_textureArray;
};
