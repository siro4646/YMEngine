struct PSInput
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0; //�@���x�N�g�� 
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};


struct VSInput
{
    float3 pos : POSITION; // ���_���W
    float3 normal : NORMAL; // �@��
    float2 uv : TEXCOORD; // UV
    float3 tangent : TANGENT; // �ڋ��
    float4 color : COLOR; // ���_�F
};

cbuffer cbScene : register(b0)
{
    matrix mat;
}

cbuffer SceneData : register(b1)
{
    matrix view; //�r���[�s��
    matrix proj; //�v���W�F�N�V�����s��
    float3 eye; //�J�����̈ʒu
};

//struct 

PSInput main(VSInput Input)
{
    PSInput output;
    
    float4 localPos = float4(Input.pos, 1.0f); // ���_���W
    float4 worldPos = mul(mat, localPos); // ���[���h���W�ɕϊ�
    float4 viewPos = mul(view, worldPos); // �r���[���W�ɕϊ�
    float4 projPos = mul(proj, viewPos); // ���e�ϊ�
    output.pos = projPos; // �N���b�v���W�ɕϊ�
    
    
    // // ������
    //output.pos = float4(Input.pos, 1.0f);

    //// �s��K�p (�����𖾊m��)
    //output.pos = mul(output.pos, mat); // ���[���h�s��K�p
    //output.pos = mul(output.pos, view); // �r���[�s��K�p
    //output.pos = mul(output.pos, proj); // �v���W�F�N�V�����s��K�p
    
    //Input.normal.
    output.normal = mul(mat, Input.normal); //float4(Input.normal, 1.0f); 
    output.color = Input.color;
    output.uv = Input.uv;
    output.ray = normalize(Input.pos.xyz - mul(view, eye));
    return output;
}