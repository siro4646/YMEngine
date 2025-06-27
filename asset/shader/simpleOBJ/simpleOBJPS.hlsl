#include "../Header/func.hlsli"
#include "../Header/modelStruct.hlsli"



struct PSInput
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION0;
    float4 normal : NORMAL0; //法線ベクトル 
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
Texture2D g_specularTexture : register(t1);
Texture2D g_maskTexture : register(t2);

struct PSOutput
{
    float4 color : SV_TARGET;
    float4 normal : SV_TARGET1;
    float4 highLum : SV_TARGET2;
    float viewZ : SV_TARGET3;
    
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
    //PSOutput output;
    
    // // テクスチャカラー
    //float3 albedo = g_texture.Sample(g_sampler, input.uv).rgb;

    //// ライトの設定
    //float3 lightDir = normalize(float3(0.0, 1.0, -1.0));
    //float3 lightColor = float3(1.0, 1.0, 1.0);
    //float lightIntensity = 5.0;

    //// 法線
    //float3 N = normalize(input.normal);

    //// 視線方向
    //float3 V = normalize(input.ray);

    //// ライト方向
    //float3 L = normalize(lightDir);

    //// フレネル反射率の初期値
    //float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, 0);

    //// Cook-Torrance BRDF
    //float3 specular = BRDFCookTorrance(N, V, L, F0, 0.0);

    //// ディフューズ反射
    //float NdotL = max(dot(N, L), 0.0);
    //float3 diffuse = (1.0 - F0) * albedo / 3.14;

    //// 環境光の追加
    //float3 ambient = float3(0.1, 0.1, 0.1) * albedo;

    //// 合成
    //float3 color = ambient + (diffuse + specular) * lightColor * lightIntensity * NdotL;
    
    //output.color = float4(color, 1);
    ////output.color = input.normal;
    
    //output.normal = float4(N,1);
    //float luminance = dot(float3(0.299f, 0.587f, 0.114f), pow(output.color.xyz, 2.2));
    //output.highLum = luminance > 0.98 ? output.color : 0.0f;
    //output.worldPos = input.worldPos;    
    
    PSOutput output;
    
    float4 maskTex = g_maskTexture.Sample(g_sampler, input.uv);
    float mask = (maskTex.a + maskTex.r + maskTex.g + maskTex.b) / 4;
    if (mask < 0.5)
    {
        discard;
        //return float4(0,0,0,0);
    }
    
    //if(input.ray.x >0)
    //{
    //    discard;
    //}
    //if(input.normal.y < 0.9)
    //{
    //    discard;
    //} 
    
    output.color = g_texture.Sample(g_sampler, input.uv);
    output.normal = normalize(input.normal);
    output.normal.a = 1.0f;
    //output.worldPos = input.worldPos;
    //output.worldPos.a = 0;
    output.viewZ = 0;

    
    float luminance = dot(float3(0.299f, 0.587f, 0.114f), pow(output.color.xyz, 2.2));
    //output.worldPos.w = luminance > 0.8 ? 1 : 0.0f;
    //output.worldPos.w = g_specularTexture.Sample(g_sampler, input.uv).r;
    
    output.highLum = 0.0f;
    output.highLum.w = g_specularTexture.Sample(g_sampler, input.uv).r;
    
    return output;
      
}