#include "simpleFBX.hlsli"


Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);


float4 main(PSInput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv);
    float4 color = float4(input.uv, 1, 1);
    return color;
    return input.color;
}