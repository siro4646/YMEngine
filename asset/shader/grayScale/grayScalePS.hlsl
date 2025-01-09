struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
   
    float4 coler = g_texture.Sample(g_sampler, input.uv);
    float mono = 0.299f * coler.r + 0.587f * coler.g + 0.114f * coler.b;
    return float4(mono, mono, mono, 1);
}