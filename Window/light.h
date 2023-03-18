// Light.h - class represents light source
#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include "D3DInclude.h"
#include "utility.h"
using namespace DirectX;

class Light {

private:
    struct Vertex {
        float x, y, z;
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
    HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4 position = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));
    // Clean up all the objects we've created
    void Shutdown();
    // Render the frame
    void Render(ID3D11DeviceContext* context);
    bool Frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

    // Function to set ambient light
    void SetColor(float red, float green, float blue, float alpha) { m_color = XMFLOAT4(red, green, blue, alpha); };
    // Function to set position of light
    void SetPosition(float x, float y, float z, float w) { m_position = XMFLOAT4(x, y, z, w); };

    // Function to get specular light
    XMFLOAT4 GetColor() { return m_color; };
    // Function to get position of light
    XMFLOAT4 GetPosition() { return m_position; };

  private:
    XMFLOAT4 m_color = { 1.0f, 1.0f, 1.0f, 0.0f };
    XMFLOAT4 m_position = { 0.0f, 1.0f, 0.0f, 0.0f };

  private:
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
