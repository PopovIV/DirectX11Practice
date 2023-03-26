// Light.h - class represents light source
#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include "D3DInclude.h"
#include "utility.h"
#include "defines.h"

using namespace DirectX;

class Light {
private:
    struct Vertex {
        float x, y, z;
    };
    struct GeomBuffer {
        XMMATRIX mWorldMatrix;
        XMFLOAT4 color;
    };
    struct SceneMatrixBuffer {
        XMMATRIX mViewProjectionMatrix;
    };
public:
    // Initialize all needed instances
    HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context);
    // Clean up all the objects we've created
    void Release();
    // Render the frame
    void Render(ID3D11DeviceContext* context);
    bool Frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

    // Get light info vector
   std::vector<std::pair<XMFLOAT3, XMFLOAT3>>& GetLightVector() { return  m_posColorVector; };
  private:
    std::vector<std::pair<XMFLOAT3, XMFLOAT3>> m_posColorVector;
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11Buffer* m_pWorldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
    ID3D11RasterizerState* m_pRasterizerState = nullptr;

    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    int m_numSphereVertices = 0;
    int m_numSphereFaces = 0;
    float m_radius = 1.0f;
};
