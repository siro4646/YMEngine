#include "../Header/modelStruct.hlsli"
#include "../Header/func.hlsli"

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION0;
    float4 normal : NORMAL0; //�@���x�N�g�� 
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};
PSInput main(VSInput Input)
{
    PSInput output;
    
    float4 localPos = float4(Input.pos, 1.0f); // ���_���W
    float4 worldPos = mul(mat, localPos); // ���[���h���W�ɕϊ�
    float4 viewPos = mul(view, worldPos); // �r���[���W�ɕϊ�
    float4 projPos = mul(proj, viewPos); // ���e�ϊ�
    output.pos = projPos; // �N���b�v���W�ɕϊ�
    output.worldPos = worldPos;
    
    
    // // ������
    //output.pos = float4(Input.pos, 1.0f);

    //// �s��K�p (�����𖾊m��)
    //output.pos = mul(output.pos, mat); // ���[���h�s��K�p
    //output.pos = mul(output.pos, view); // �r���[�s��K�p
    //output.pos = mul(output.pos, proj); // �v���W�F�N�V�����s��K�p
    
    float4x4 worldInvTrans = transpose(MyInverse(mat)); // mat = Scale * Rotation * Translation
    float3x3 normalMatrix = (float3x3) worldInvTrans; // ��3�~3�𔲂��o��

    float3 nWorld = mul(normalMatrix, Input.normal);
    nWorld = normalize(nWorld);

// �@���x�N�g���Ȃ̂� w=0
    output.normal = float4(nWorld, 0.0f);
    
    output.color = Input.color;
    output.uv = Input.uv;
    output.ray = normalize(worldPos.xyz - eye); // �J�����ʒu���烏�[���h���W�ւ̕���
    return output;
}