#include "CBScene.h"

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct PS_INPUT 
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

cbuffer WorldMatrixBuffer : register (b0)
{
    float4x4 mWorldMatrix;
    float4 shine;// x - specular power
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;

    output.worldPos = mul(mWorldMatrix, float4(input.position, 1.0f));
    output.position = mul(mViewProjectionMatrix, output.worldPos);
    output.uv = input.uv;
    output.normal = mul(mWorldMatrix, input.normal);
    output.tangent = mul(mWorldMatrix, input.tangent);

    return output;
}
