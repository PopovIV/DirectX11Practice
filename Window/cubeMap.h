// CubemMap.h - class for rendering cubemap
#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>
#include "DDSTextureLoader.h"
#include "utility.h"

using namespace DirectX;

class CubeMap {
private:
    struct Vertex {
        float x, y, z;
    };
    struct WorldMatrixBuffer {
        XMMATRIX mWorldMatrix;
        XMFLOAT4 size;
    };
    struct SceneMatrixBuffer {
        XMMATRIX mViewProjectionMatrix;
        XMFLOAT4 cameraPos;
    };
public:
    // Initialize all needed instances
    HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight);
    // Clean up all the objects we've created
    void Release();
    // Resize function
    void Resize(int screenWidth, int screenHeight);
    void Render(ID3D11DeviceContext* context);

    // Render the frame
    bool Frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);

private:
    // Function to initialize scene's geometry
    HRESULT InitScene(ID3D11Device* device, ID3D11DeviceContext* context, UINT LatLines, UINT LongLines);

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11Buffer* m_pWorldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
    ID3D11RasterizerState* m_pRasterizerState = nullptr;
    ID3D11SamplerState* m_pSampler = nullptr;

    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    ID3D11ShaderResourceView* m_pTexture;

    int m_numSphereVertices = 0;
    int m_numSphereFaces = 0;
    float m_radius = 1.0f;
};
