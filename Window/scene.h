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
#include "light.h"
#include "DDSTextureLoader.h"
#include "utility.h"
#include "defines.h"
#include "frustum.h"

using namespace DirectX;

static const XMFLOAT4 AABB[] = {
    {-0.5, -0.5, -0.5, 1.0},
    {0.5,  0.5, 0.5, 1.0}
};

static const XMFLOAT4 Vertices[] = {
    {0, -1, -1, 1},
    {0,  1, -1, 1},
    {0,  1,  1, 1},
    {0, -1,  1, 1}
};

class Scene {
private:

    struct Vertex {
        XMFLOAT3 pos;
        XMFLOAT2 uv;
        XMFLOAT3 normal;
        XMFLOAT3 tangent;
    };

    struct GeomBuffer {
        XMMATRIX mWorldMatrix;
        XMMATRIX norm;
        XMFLOAT4 shineSpeedTexIdNM;
    };

    struct CubeModel {
        XMFLOAT4 pos;
        XMFLOAT4 shineSpeedIdNM;
    };

    struct WorldMatrixBuffer {
        XMMATRIX mWorldMatrix;
        XMFLOAT4 color;
    };

    struct SceneConstantBuffer {
        XMMATRIX mViewProjectionMatrix;
        XMFLOAT4 planes[6];
    };

    struct CullParams {
        XMINT4 numShapes; // x - objects count;
        XMFLOAT4 bbMin[MAX_CUBE];
        XMFLOAT4 bbMax[MAX_CUBE];
    };

    struct LightConstantBuffer {
        XMFLOAT4 cameraPos;
        XMINT4 lightCount;
        XMFLOAT4 lightPos[MAX_LIGHT];
        XMFLOAT4 lightColor[MAX_LIGHT];
        XMFLOAT4 ambientColor;
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

    // ImGui Light change
    void CreateNewLight();
    void DeleteLight();
    // ImGui Cube change
    void CreateNewCube();
    void DeleteCube();

    // Switch flags functions
    void ToggleSpheres() { m_isSpheresOn = !m_isSpheresOn; };
    void ToggleNormalMaps() { m_useNormalMap = !m_useNormalMap; };
    void ToggleShowNormals() { m_showNormals = !m_showNormals; };
    void ToggleCulling() { m_isCullingOn = !m_isCullingOn; };
    void ToggleGPUCulling() { m_computeCull = !m_computeCull; };
    void GPUCullingOFF() { m_computeCull = false; };
    // Get light info vector
    std::vector<std::pair<XMFLOAT3, XMFLOAT3>>& GetLightVector() { return  m_pLight->GetLightVector(); };
    // Get cube count
    int GetCubeCount() { return m_cubesCount; };
    int GetCubeRendered() { return m_computeCull ? m_cubesCountGPU : (int)m_cubeIndexies.size(); };
    int GetCubeCulled() { return m_computeCull ? m_cubesCount - m_cubesCountGPU : m_cubesCount - (int)m_cubeIndexies.size(); };
private:
    std::vector<CubeModel> m_cubeModelVector;
    std::vector<int> m_cubeIndexies;
    int m_cubesCount = MAX_CUBE;
    int m_cubesCountGPU = MAX_CUBE;
    // Function to initialize scene's geometry
    HRESULT InitScene(ID3D11Device* device, ID3D11DeviceContext* context);
    // Function to initialize transperent scene's geometry
    HRESULT InitSceneTransparent(ID3D11Device* device, ID3D11DeviceContext* context);
    // Render transperent part
    void RenderTransparent(ID3D11DeviceContext* context);
    // Function to get info from Queries
    void ReadQueries(ID3D11DeviceContext* context);

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11Buffer* m_pGeomBufferInst = nullptr;
    ID3D11Buffer* m_pSceneConstantBuffer = nullptr;
    ID3D11Buffer* m_pCullParams = nullptr;
    ID3D11Buffer* m_pLightConstantBuffer = nullptr;
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
    ID3D11ComputeShader* m_pCullShader = nullptr;

    ID3D11InputLayout* m_pTransInputLayout = nullptr;
    ID3D11VertexShader* m_pTransVertexShader = nullptr;
    ID3D11PixelShader* m_pTransPixelShader = nullptr;

    ID3D11Buffer* m_pInderectArgsSrc = nullptr;
    ID3D11Buffer* m_pInderectArgs = nullptr;
    ID3D11UnorderedAccessView* m_pInderectArgsUAV = nullptr;
    ID3D11Buffer* m_pGeomBufferInstVis = nullptr;
    ID3D11Buffer* m_pGeomBufferInstVisGpu = nullptr;
    ID3D11UnorderedAccessView* m_pGeomBufferInstVisGpu_UAV = nullptr;

    CubeMap* m_pCubeMap = nullptr;
    Light* m_pLight = nullptr;
    Frustum* m_pFrustum = nullptr;

    std::vector<Texture> m_textureArray;

    ID3D11Query* m_queries[MAX_QUERY];
    unsigned int m_curFrame = 0;
    unsigned int m_lastCompletedFrame = 0;

    // flag to know what rect to draw
    bool m_yellowRect = false;
    // flag to render light spheres
    bool m_isSpheresOn = true;
    // flag to use normal maps on cubes
    bool m_useNormalMap = true;
    // flag to show normals
    bool m_showNormals = false;
    // flag to turn culling
    bool m_isCullingOn = true;
    // flag to turn gpu culling
    bool m_computeCull = true;
};
