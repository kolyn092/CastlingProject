//
//
// Tex.fx

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

    //pos = mul(pos, mWVP);	 
	pos = mul(pos, mWorld);

	// Light
	//float4 diff = Light(nrm);
	float4 diff = MaterialLight(nrm);
	//float4 diff = 1;

	pos = mul(pos, mView);

	float4 spec = 0;// Specular(pos, nrm);

	pos = mul(pos, mProj);

	o.pos = pos;
	o.diff = diff;
	o.spec = spec;
	o.nrm = nrm;
	o.uv = uv;

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
	float4 tex = texDiffuse.Sample(smpLinear, uv);

	// �����ڷ���(���ڵ�->���Ͼ�: ��Ӱ�)
	tex = tex * tex;

	//float4 lightDiff = Light(nrm);

	//float4 specCol = Specular(pos, nrm);

	// per-pixel direction light
	//float4 col = tex * lightDiff + spec;

	// per-vertex direction light
	// per-vertex specular light
	//float4 col = tex * diff + spec;

	// per-vertex direction light
	// per-pixel specular light
	//float4 col = (tex * diff * g_Mtrl.Diffuse) + g_Mtrl.Ambient;// +specCol;
	float4 col = tex * diff;// +specCol;
	//float4 col = tex + (tex * diff);
	//float4 col = tex * (g_Mtrl.Diffuse * g_Mtrl.Ambient);// +specCol;

	if (col.a < 0.3f)
		discard;

	// �����ڷ���(���Ͼ�->���ڵ�: ���)
	col = pow(col, 0.5f);

	// just color
	//float4 col = diff;

	return col;
}

