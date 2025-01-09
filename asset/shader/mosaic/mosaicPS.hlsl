Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float2 GetPixelPosition(float2 uv, float2 g_windowSize)
{
    return g_windowSize * uv;
}

float4 main(PSInput input) : SV_TARGET
{
    
    float2 g_windowSize;
    float4 g_area = float4(0, 0, 1280, 720);
    float g_size = 5.0;
    
    g_windowSize = float2(1280, 720);
     
    float2 pixelPos = GetPixelPosition(input.uv, g_windowSize);
    if (
        g_area.x < pixelPos.x &&
        g_area.x + g_area.z > pixelPos.x &&
        g_area.y < pixelPos.y &&
        g_area.y + g_area.w > pixelPos.y)
    {
        float2 uv = input.uv;
        uv.x = floor(uv.x * g_windowSize.x / g_size) * g_size / g_windowSize.x;
        uv.y = floor(uv.y * g_windowSize.y / g_size) * g_size / g_windowSize.y;
        return g_texture.Sample(g_sampler,uv);
        //return float4(1, 0, 0, 1);
    }
    else
    {
       return g_texture.Sample(g_sampler, input.uv);
       //return float4(1, 1, 0, 1);
        
    }
}