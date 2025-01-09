struct PSInput
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0; //法線ベクトル 
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};


struct VSInput
{
    float3 pos : POSITION; // 頂点座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // UV
    float3 tangent : TANGENT; // 接空間
    float4 color : COLOR; // 頂点色
};

cbuffer cbScene : register(b0)
{
    matrix mat;
}

cbuffer SceneData : register(b1)
{
    matrix view; //ビュー行列
    matrix proj; //プロジェクション行列
    float3 eye; //カメラの位置
};

//struct 

PSInput main(VSInput Input)
{
    PSInput output;
    
    float4 localPos = float4(Input.pos, 1.0f); // 頂点座標
    float4 worldPos = mul(mat, localPos); // ワールド座標に変換
    float4 viewPos = mul(view, worldPos); // ビュー座標に変換
    float4 projPos = mul(proj, viewPos); // 投影変換
    output.pos = projPos; // クリップ座標に変換
    
    
    // // 初期化
    //output.pos = float4(Input.pos, 1.0f);

    //// 行列適用 (順序を明確化)
    //output.pos = mul(output.pos, mat); // ワールド行列適用
    //output.pos = mul(output.pos, view); // ビュー行列適用
    //output.pos = mul(output.pos, proj); // プロジェクション行列適用
    
    //Input.normal.
    output.normal = mul(mat, Input.normal); //float4(Input.normal, 1.0f); 
    output.color = Input.color;
    output.uv = Input.uv;
    output.ray = normalize(Input.pos.xyz - mul(view, eye));
    return output;
}