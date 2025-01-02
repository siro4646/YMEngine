#include "test.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};


cbuffer cbScene : register(b0)
{
    matrix mat;
}

PSInput main(VSInput input)
{
    PSInput output;
    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, mat);
    output.texCoord = input.texCoord;
	
	return output;
}