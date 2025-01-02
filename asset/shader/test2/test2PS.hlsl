
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
    float g_size = 2.0;
    
    float4 color = float4(input.uv, 1, 1);
    
    g_windowSize = float2(1280, 720);
    
    float2 pixel = GetPixelPosition(input.uv, g_windowSize);
    
    if (pixel.x < g_size || g_windowSize.x - g_size < pixel.x
        || pixel.y < g_size || g_windowSize.y - g_size < pixel.y
        )
    {
        return float4(0,0, 0, 1);
    }
    else
    {
        return color;
    }
    
	//return float4(input.uv,0,0.5f);
}