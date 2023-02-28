#include "texture.h"

// Function to initialize texture
HRESULT Texture::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename) {
    HRESULT hr = S_OK;
    // Load the Texture
    hr = DirectX::CreateDDSTextureFromFile(device, filename, nullptr, &m_pTextureView);
    if (SUCCEEDED(hr)) {
        // Generate mipmaps for this texture.
        deviceContext->GenerateMips(m_pTextureView);
    }

    return hr;
}

// Function to realese texture
void Texture::Shutdown() {
    // Release the texture view resource.
    if (m_pTextureView) {
        m_pTextureView->Release();
        m_pTextureView = nullptr;
    }
}
