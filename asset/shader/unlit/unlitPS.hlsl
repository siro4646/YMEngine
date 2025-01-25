struct PSInput
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0; //法線ベクトル 
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};


Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);


float4 main(PSInput input) : SV_TARGET
{
    //uv値を反転させる
    float2 invertUv = float2(1.0 - input.uv.x,input.uv.y);
    
    return g_texture.Sample(g_sampler, invertUv);
}