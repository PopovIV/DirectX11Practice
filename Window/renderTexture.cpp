#include "renderTexture.h"

// Function to initialize render texture class
HRESULT RenderTexture::Init(ID3D11Device* device, int textureWidth, int textureHeight) {
    // Initialize the render target texture description.
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &m_pRenderTargetTexture);
    if (FAILED(hr)) {
        return hr;
    }

    // Setup the description of the render target view.
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target view.
    hr = device->CreateRenderTargetView(m_pRenderTargetTexture, &renderTargetViewDesc, &m_pRenderTargetView);
    if (FAILED(hr)) {
        return hr;
    }

    // Setup the description of the shader resource view.
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    hr = device->CreateShaderResourceView(m_pRenderTargetTexture, &shaderResourceViewDesc, &m_pShaderResourceView);
    if (FAILED(hr)) {
        return hr;
    }

    m_viewport.Width = (FLOAT)textureWidth;
    m_viewport.Height = (FLOAT)textureHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;

    return S_OK;
}

// Function to realese render texture class
void RenderTexture::Release() {
    if (m_pShaderResourceView) {
        m_pShaderResourceView->Release();
        m_pShaderResourceView = nullptr;
    }

    if (m_pRenderTargetView) {
        m_pRenderTargetView->Release();
        m_pRenderTargetView = nullptr;
    }

    if (m_pRenderTargetTexture) {
        m_pRenderTargetTexture->Release();
        m_pRenderTargetTexture = nullptr;
    }

    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
}

void RenderTexture::Resize(ID3D11Device* device, int width, int height) {
    Release();
    Init(device, width, height);
}

// Function to clear render target
void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha) {
    // Setup the color to clear the buffer to.
    float color[4];
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    deviceContext->ClearRenderTargetView(m_pRenderTargetView, color);

    // Clear the depth buffer.
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 0.0f, 0);
}
