// Camera.h - class handles work with camera
#pragma once

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class Camera {
public:
    // Initialize function
    HRESULT Init();
    // Realese function
    void Release() {};
    // Update the frame
    void Frame();
    // Update camera pos by mouse input
    void MouseMoved(float dx, float dy, float wheel);
    // Function to get view matrix
    void GetBaseViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_viewMatrix; };
private:
    XMMATRIX m_viewMatrix;
    XMFLOAT3 m_pointOfInterest;
    float m_distanceToPoint;
    float m_phi;
    float m_theta;
};
