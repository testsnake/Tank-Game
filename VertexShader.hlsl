// VertexShader.hlsl

cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
}

struct VertexInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = mul(float4(input.position, 1.0f), WorldViewProjection);
    output.texCoord = input.texCoord;
    return output;
}