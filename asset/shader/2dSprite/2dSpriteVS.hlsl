#include "2dSprite.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};


cbuffer cbScene : register(b0)
{
    matrix mat;
    float4 color;
    float4 rect;
}

PSInput main(VSInput input)
{
    PSInput output;
    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, mat);
    output.texCoord.x = rect.x + input.texCoord.x * rect.z;
    output.texCoord.y = rect.y + input.texCoord.y * rect.w;
    output.color = color;
    return output;
}