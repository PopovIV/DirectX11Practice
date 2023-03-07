cbuffer WorldMatrixBuffer : register (b0)
{
    float4x4 mWorldMatrix;
    float4 color;
};

cbuffer SceneMatrixBuffer : register (b1)
{
    float4x4 mViewProjectionMatrix;
};

struct VS_INPUT
{
    float4 position : POSITION;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;

    output.position = mul(mWorldMatrix, input.position);
    output.position = mul(mViewProjectionMatrix, output.position);

    return output;
}
