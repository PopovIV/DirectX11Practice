// Texture.h - class for storing texture data
#pragma once

#include <d3d11.h>
#include <stdio.h>
#include <vector>
#include "DDSTextureLoader.h"

class Texture {
public:
    // Function to initialize texture
    HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename);
    // Funnction to initialize texture array
    HRESULT InitArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<const wchar_t*> filenames);
    // Function to realese texture
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture() { return m_pTextureView; };

private:
    ID3D11ShaderResourceView* m_pTextureView = nullptr;
};
