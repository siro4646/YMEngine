
Texture2D<float4> tex : register(t0);

SamplerState smp : register(s0);

struct Output
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};



float4 main(Output input) : SV_TARGET
{
    float4 texColor = tex.Sample(smp, input.uv);
    float4 result = texColor;
    
    //float gray = dot(result.rgb, float3(0.299, 0.587, 0.114));
    //result.rgb = gray;
    
    return result;
}