#include "simpleFBX.hlsli"
#include "../Header/func.hlsli"


Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
Texture2D g_specularTexture : register(t1);
Texture2D g_maskTexture : register(t2);

struct PSOutput
{
    float4 color : SV_TARGET;
    float4 normal : SV_TARGET1;
    float4 highLum : SV_TARGET2;
};

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
    float ggx2 = NdotL / (NdotL * (1.0 - k) + k);

    return ggx1 * ggx2;
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14 * denom * denom;

    return num / denom;
}

float3 BRDFCookTorrance(float3 N, float3 V, float3 L, float3 F0, float roughness)
{
    float3 H = normalize(V + L);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(max(dot(N, V), 0.0), max(dot(N, L), 0.0), roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    return (NDF * G * F) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001);
}



PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;
    
    float3 light = normalize(float3(-1, -1, 1)); //光の向かうベクトル(平行光線)
    
    //ディフューズ計算
    float diffuseB = saturate(dot(-light, input.normal.xyz));
    //光の反射ベクトル
    float4 specular = g_specularTexture.Sample(g_sampler, input.uv);
    float3 refLight = normalize(reflect(light, input.normal.xyz));
    float specularB = pow(saturate(dot(refLight, -input.ray)), specular.a);
    
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
    
    output.highLum = luminance > 0.98 ? output.color : 0.0f;
    output.highLum = 0.0f;
    
    return output;
      
}