cbuffer WorldMatrixBuffer : register (b0)
{
    float4x4 mWorldMatrix;
};

cbuffer SceneMatrixBuffer : register (b1)
{
    float4x4 mViewProjectionMatrix;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;

    output.position = mul(mViewProjectionMatrix, mul(mWorldMatrix, float4(input.position, 1.0f)));
    output.uv = input.uv;

    return output;
}
