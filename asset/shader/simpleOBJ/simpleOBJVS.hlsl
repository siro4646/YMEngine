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
};
PSInput main(VSInput Input)
{
    PSInput output;
    
    float4 localPos = float4(Input.pos, 1.0f); // 頂点座標
    float4 worldPos = mul(mat, localPos); // ワールド座標に変換
    float4 viewPos = mul(view, worldPos); // ビュー座標に変換
    float4 projPos = mul(proj, viewPos); // 投影変換
    output.pos = projPos; // クリップ座標に変換
    output.worldPos = worldPos;
    
    
    // // 初期化
    //output.pos = float4(Input.pos, 1.0f);

    //// 行列適用 (順序を明確化)
    //output.pos = mul(output.pos, mat); // ワールド行列適用
    //output.pos = mul(output.pos, view); // ビュー行列適用
    //output.pos = mul(output.pos, proj); // プロジェクション行列適用
    
    float4x4 worldInvTrans = transpose(MyInverse(mat)); // mat = Scale * Rotation * Translation
    float3x3 normalMatrix = (float3x3) worldInvTrans; // 上3×3を抜き出す

    float3 nWorld = mul(normalMatrix, Input.normal);
    nWorld = normalize(nWorld);

// 法線ベクトルなので w=0
    output.normal = float4(nWorld, 0.0f);
    
    output.color = Input.color;
    output.uv = Input.uv;
    output.ray = normalize(worldPos.xyz - eye); // カメラ位置からワールド座標への方向
    return output;
}