#include "test.hlsli"

//sampler
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);




float4 main(PSInput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.texCoord);
    //return color;
	//return float4(input.uv,0,0.5f);
}