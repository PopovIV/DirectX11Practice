#include "CBTrans.h"

struct VS_INPUT
{
    float4 position : POSITION;
    uint instanceId : SV_InstanceID;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    uint instanceId : SV_InstanceID;
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;

#ifdef USE_LIGHTS
    output.worldPos = mul(geomBuffer.mWorldMatrix, input.position);
#else
    unsigned int idx = input.instanceId;
    output.worldPos = mul(geomBuffer[idx].mWorldMatrix, input.position);
    output.instanceId = idx;
#endif // !USE_LIGHTS
    output.position = mul(mViewProjectionMatrix, output.worldPos);

    return output;
}
