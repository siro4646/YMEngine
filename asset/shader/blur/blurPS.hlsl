struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};


float4 main(PSInput input) : SV_TARGET
{
    float4 result;
    float4 color = float4(input.uv, 1, 1);
    int _sampleCount = 12;
    float _strength = 10;
    
    float2 cenUV = input.uv - 0.5f;
    float dist = length(cenUV);
    for(int i = 0; i < _sampleCount; i++)
    {
        float uvOff = 1 - _strength * i / _sampleCount * dist;
        result += float4(color.r * cenUV.x * uvOff + 0.5, color.g * cenUV.y * uvOff + 0.5, color.ba);
    }
    return result / _sampleCount;    
}