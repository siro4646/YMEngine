#include "../Header/modelStruct.hlsli" 
#include "../Header/func.hlsli"

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION0;
    float4 normal : NORMAL0; //法線ベクトル 
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
    float3 viewPos : POSITION1;

};

struct PSOutput
{
    float4 color : SV_TARGET;
    float4 normal : SV_TARGET1;
    float4 highLum : SV_TARGET2;
    float4 worldPos : SV_TARGET3;
    float viewZ : SV_TARGET4;
    
};
cbuffer Test : register(b2)
{
    float3 testValue;
    //float testValue1;
    float psd[1];
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;

    //output.color = input.color; // 色をそのまま出力
    //output.color = g_texture.Sample(g_sampler, input.uv); // テクスチャとモデルの色を掛け合わせる
    //output.color = float4(testValue, 1.0f)*testValue1;
    output.color = float4(testValue, 1.0f);    
    output.normal = normalize(input.normal);
    output.normal.a = 1.0f;
    output.viewZ = -input.viewPos.z;
    output.worldPos = input.worldPos;
    //output.viewZ = 1;
    
    
    output.highLum = 0.0f;
    
    return output;
}