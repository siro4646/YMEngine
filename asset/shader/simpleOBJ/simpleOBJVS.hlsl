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
    
    //Input.normal.
    float3x3 normalMatrix = (float3x3) mat; // 上3x3を取り出す
    output.normal = float4(normalize(mul(normalMatrix, Input.normal)), 1);
    output.color = Input.color;
    output.uv = Input.uv;
    output.ray = normalize(worldPos.xyz - eye); // カメラ位置からワールド座標への方向
    return output;
}