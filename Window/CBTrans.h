#include "defines.h"
#include "LightCalc.h"

struct GeomBuffer {
    float4x4 mWorldMatrix;
    float4 color;
};

#ifdef USE_LIGHTS
cbuffer WorldMatrixBuffer : register (b0)
{
    GeomBuffer geomBuffer;
};
#else
cbuffer GeomxBufferInst : register (b0)
{
    GeomBuffer geomBuffer[MAX_LIGHT];
};
#endif // !USE_LIGHTS

cbuffer LightConstantBuffer : register (b1)
{
    float4x4 mViewProjectionMatrix;
};
