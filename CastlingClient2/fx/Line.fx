//
//
// Line.fx

cbuffer ConstBuffer : register(b0) //������� 0���� ��� (�ִ� 14��. 0~13)
{
	matrix mWorld;
	matrix mView;
	matrix mProj;
	matrix mWV;
	matrix mWVP;
}

//VS ��� ����ü.
struct VSOutput
{
    float4 pos		: SV_POSITION;
};


////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main
//
VSOutput VS_Main(
				  float4 pos : POSITION    //[�Է�] ������ǥ. Vertex Position (Model Space, 3D)
				)
{

	VSOutput o = (VSOutput)0;
	pos.w = 1.0f;


	pos = mul(pos, mWorld);

	pos = mul(pos, mView);

	pos = mul(pos, mProj);
	

	o.pos = pos;

	return o;
}



//�ؽ�ó ��ü ����: �������� ���޵�.
Texture2D texDiffuse;
//�������� ���� ����. (�⺻���� t0)
//Texture2D texDiffuse : register(t0);     

//�ؽ�ó ���÷�. (��������)
SamplerState smpLinear;


////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main
//
float4 PS_Main(
	float4 pos : SV_POSITION,    //[�Է�] ������ǥ. (Screen, 2D)
	float4 diff : COLOR0,        //[�Է�] ������. (Vertex Color : "Diffuse")
	float4 spec : COLOR1,
	float2 uv : TEXCOORD0
	) : SV_TARGET
{

	float4 col = float4(0, 1, 0, 1);

	return col;
}

