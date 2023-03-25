#include "defines.h"

cbuffer LightConstantBuffer : register(b2) {
    float4 cameraPos;
    int4 lightCount; // x - count, y - use normals, z - show normals
    float4 lightPos[MAX_LIGHT];
    float4 lightColor[MAX_LIGHT];
    float4 ambientColor;
}
