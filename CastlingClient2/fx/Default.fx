//
//
// Default.fx

#include "Light.fx"

//VS ��� ����ü.
struct VSOutput
{
    float4 pos		: SV_POSITION;
    float4 diff		: COLOR0;
	float4 spec		: COLOR1;
	float4 nrm		: TEXCOORD100;
	float2 uv		: TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main
//
VSOutput VS_Main(
				  float4 pos : POSITION,    //[�Է�] ������ǥ. Vertex Position (Model Space, 3D)
				  float4 nrm : NORMAL,      //[�Է�] ��� normal 
				  float2 uv  : TEXCOORD0
				)
{

	VSOutput o = (VSOutput)0;
	pos.w = 1.0f;      nrm.w = 0.0f;
 
	pos = mul(pos, mWorld);

	pos = mul(pos, mView);

	pos = mul(pos, mProj);

	o.pos = pos;
	o.nrm = nrm;

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
	float4 nrm : TEXCOORD100,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	return float4(1,1,1,1);
}

