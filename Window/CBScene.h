#include "defines.h"
#include "LightCalc.h"

struct GeomBuffer
{
    float4x4 mWorldMatrix;
    float4x4 norm;
    float4 shineSpeedTexIdNM; // x - specular power, y - rotation speed, z - texture id, w - normal map presence
};

cbuffer GeomxBufferInst : register (b0)
{
    GeomBuffer geomBuffer[MAX_CUBE];
};

cbuffer SceneConstantBuffer : register (b1)
{
    float4x4 mViewProjectionMatrix;
    float4 planes[6];
};

cbuffer IndexBuffer : register(b2)
{
    uint4 objectIDs[MAX_CUBE];
}