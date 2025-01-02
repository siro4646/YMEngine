struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};


float4 main(PSInput input) : SV_TARGET
{
   
    float4 coler = float4(input.uv, 1, 1);
    
    float mono = 0.299f * coler.r + 0.587f * coler.g + 0.114f * coler.b;
    return float4(mono, mono, mono, 1);
}