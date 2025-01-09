
struct VSInput
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0; 
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(VSInput input)
{
	VSOutput output;
    output.svpos = input.pos;
    output.uv = input.uv;
    return output;	
}