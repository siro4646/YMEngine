struct PSInput
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0; //法線ベクトル 
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};
struct PSOutput
{
    float4 color : SV_TARGET;
    float4 normal : SV_TARGET1;
    float4 highLum : SV_TARGET2;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
Texture2D g_specularTexture : register(t1);

Texture2D g_maskTexture : register(t2);


PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;
    
    float3 light = normalize(float3(-1, -1, 1)); //光の向かうベクトル(平行光線)
    
    //ディフューズ計算
    float diffuseB = saturate(dot(-light, input.normal.xyz));
    //光の反射ベクトル
    
    float4 specular = g_specularTexture.Sample(g_sampler, input.uv);
    float3 refLight = normalize(reflect(light, input.normal.xyz));
    float specularB = pow(saturate(dot(refLight, input.ray)), specular.a);
    
    float4 maskTex = g_maskTexture.Sample(g_sampler, input.uv);
    float mask = (maskTex.a + maskTex.r + maskTex.g + maskTex.b) / 4;
    if (mask < 0.5)
    {
        discard;
        //return float4(0,0,0,0);
    }
    
    float4 texColor = g_texture.Sample(g_sampler, input.uv);
    
    float4 color = texColor + saturate(float4(specularB * specular.rgb, 1));
    output.color = color;
    output.normal = input.normal;
    
    float luminance = dot(float3(0.299f, 0.587f, 0.114f), output.color.xyz);
    
    output.highLum = luminance > 0.9 ? output.color : 0.0f;
    
    
    
    
    
    
    
    return output;
      
}