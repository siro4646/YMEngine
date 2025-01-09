#include "2dSprite.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};


cbuffer cbScene : register(b0)
{
    matrix mat;
    float4 color;
    float4 rect;
}
cbuffer SceneData : register(b1)
{
    matrix view; //ビュー行列
    matrix proj; //プロジェクション行列
    float3 eye; //カメラの位置
};


PSInput main(VSInput input)
{
    PSInput output;
    float4 localPos = float4(input.position,1.0f); // 頂点座標
    float4 worldPos = mul(mat, localPos); // ワールド座標に変換
    float4 viewPos = mul(view, worldPos); // ビュー座標に変換
    float4 projPos = mul(proj, viewPos); // 投影変換
    output.position = projPos; // クリップ座標に変換
    output.texCoord.x = rect.x + input.texCoord.x * rect.z;
    output.texCoord.y = rect.y + input.texCoord.y * rect.w;
    output.color = color;
    return output;
}