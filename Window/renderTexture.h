// RenderTexture.h - class for rendering to render target
#pragma once

#include <d3d11.h>

class RenderTexture {
public:
    // Function to initialize render texture class
    HRESULT Init(ID3D11Device* device, int textureWidth, int textureHeight);
    // Function to realese render texture class
    void Release();
    // Resize Function
    void Resize(ID3D11Device* device, int width, int height);

    // Function to set render target
    void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView) { deviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, depthStencilView); };
    // Function to clear render target
    void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha);
    // Function to get texture from render texture
    ID3D11Texture2D* GetRenderTarget() { return m_pRenderTargetTexture; };
    // Function to get render target view
    ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; };
    // Function to get shader resource view
    ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView; };
    // Function to get view port 
    D3D11_VIEWPORT GetViewPort() { return m_viewport; };

private:
    ID3D11Texture2D* m_pRenderTargetTexture = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
    ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
    D3D11_VIEWPORT m_viewport;
};
