// frustum.h - class for culling
#pragma once

#include <directxmath.h>
using namespace DirectX;

class Frustum {
public:
    // Function to initialize frustum class
    void Init(float screenDepth) { m_screenDepth = screenDepth; };
    // Release function
    void Release() {};
    // Function to build frustum
    void ConstructFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

    // Functions to check if rectengle is in frustum
    bool CheckRectangle(XMFLOAT4 bbMin, XMFLOAT4 bbMax);
    XMFLOAT4* GetPlanes() { return m_planes; };
private:
    float m_screenDepth;
    XMFLOAT4 m_planes[6];
};
