#include "CBScene.h"

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint instanceId : SV_InstanceID;
};

struct PS_INPUT 
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    nointerpolation uint instanceId : INST_ID;
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;

    unsigned int idx = objectIDs[input.instanceId].x;
    output.worldPos = mul(geomBuffer[idx].mWorldMatrix, float4(input.position, 1.0f));
    output.position = mul(mViewProjectionMatrix, output.worldPos);
    output.uv = input.uv;
    output.normal = mul(geomBuffer[idx].norm, float4(input.normal, 0.0f)).xyz;
    output.tangent = mul(geomBuffer[idx].norm, float4(input.tangent, 0.0f)).xyz;
    output.instanceId = idx;

    return output;
}
