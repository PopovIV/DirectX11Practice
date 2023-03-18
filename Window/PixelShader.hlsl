#include "LightCalc.h"

Texture2D cubeTexture : register (t0);
Texture2D cubeNormal : register (t1);

SamplerState cubeSampler : register(s0);

cbuffer WorldMatrixBuffer : register (b0)
{
    float4x4 mWorldMatrix;
    float4 shine;// x - specular power
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

float4 main(PS_INPUT input) : SV_TARGET{
    float3 color = cubeTexture.Sample(cubeSampler, input.uv).xyz;
    float3 finalColor = ambientColor.xyz * color;

    float3 norm = float3(0, 0, 0);
    if (lightCount.y > 0) {
        float3 binorm = normalize(cross(input.normal, input.tangent));
        float3 localNorm = cubeNormal.Sample(cubeSampler, input.uv).xyz * 2.0 - 1.0;
        norm = localNorm.x * normalize(input.tangent) + localNorm.y * binorm + localNorm.z * normalize(input.normal);
    } 
    else {
        norm = input.normal;
    }

    return float4(CalculateColor(color, norm, input.worldPos.xyz, shine.x, false), 1.0);
}
