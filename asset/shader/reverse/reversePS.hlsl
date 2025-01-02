Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};


float4 main(PSInput input) : SV_TARGET
{
   
    float4 color = g_texture.Sample(g_sampler,input.uv);

        
    float4 reverse = float4(float3(1, 1, 1) - color.rgb,color.a);
   // reverse.a = color.a;
    
    return reverse;
}