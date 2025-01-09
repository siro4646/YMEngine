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
    matrix view; //�r���[�s��
    matrix proj; //�v���W�F�N�V�����s��
    float3 eye; //�J�����̈ʒu
};


PSInput main(VSInput input)
{
    PSInput output;
    float4 localPos = float4(input.position,1.0f); // ���_���W
    float4 worldPos = mul(mat, localPos); // ���[���h���W�ɕϊ�
    float4 viewPos = mul(view, worldPos); // �r���[���W�ɕϊ�
    float4 projPos = mul(proj, viewPos); // ���e�ϊ�
    output.position = projPos; // �N���b�v���W�ɕϊ�
    output.texCoord.x = rect.x + input.texCoord.x * rect.z;
    output.texCoord.y = rect.y + input.texCoord.y * rect.w;
    output.color = color;
    return output;
}