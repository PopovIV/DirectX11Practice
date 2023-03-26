#include "CBTrans.h"

struct PS_INPUT {
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    uint instanceId : SV_InstanceID;
};

float4 main(PS_INPUT input) : SV_TARGET{
#ifdef USE_LIGHTS
    return float4(CalculateColor(geomBuffer.color.xyz, float3(1, 0, 0), input.worldPos.xyz, 0.0, true), geomBuffer.color.w);
#else
    return geomBuffer[input.instanceId].color;
#endif // !USE_LIGHTS
}
