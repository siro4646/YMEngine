struct PSInput
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0; //�@���x�N�g�� 
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};