struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 result = 0;
    float4 color = g_texture.Sample(g_sampler, input.uv);
    int _sampleCount = 12;
    float _strength = 10;
    
    float2 cenUV = input.uv - 0.5f;
    float dist = length(cenUV);
    for(int i = 0; i < _sampleCount; i++)
    {
        float uvOff = 1 - _strength * i / _sampleCount * dist;
        result += float4(
            color.r * dist * uvOff + 0.5, // R
            color.g * dist * uvOff + 0.5, // G
            color.b * dist * uvOff + 0.5, // B
            color.a                       // A‚Í‚»‚Ì‚Ü‚Ü
        );
    }
    return result / _sampleCount;    
}