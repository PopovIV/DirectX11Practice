#include "LightCalc.h"

cbuffer WorldMatrixBuffer : register (b0)
{
    float4x4 mWorldMatrix;
    float4 color;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET {
#ifdef USE_LIGHTS
    return float4(CalculateColor(color.xyz, float3(1, 0, 0), input.worldPos.xyz, 0.0, true), color.w);
#else
    return color;
#endif // !USE_LIGHTS
}
