// PostEffect.h - class for applying post-effects to rendering result
#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "utility.h"
#include <directxmath.h>

using namespace DirectX;

class PostEffect {
private:
    struct PostEffectConstantBuffer {
        XMINT4 params;
    };
public:
    // Function to initialize
    HRESULT Init(ID3D11Device* device, HWND hwnd);
    // Function to realese
    void Release();
    // Render function
    void Process(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, ID3D11RenderTargetView* renderTarget, D3D11_VIEWPORT viewport);
    // Switch flags functions
    void ToggleGrayScale(ID3D11DeviceContext* deviceContext);
private:
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11SamplerState* m_pSamplerState = nullptr;
    ID3D11Buffer* m_pPostEffectConstantBuffer = nullptr;
    // flag to turn on post effect
    bool m_isGrayScale = true;
};
