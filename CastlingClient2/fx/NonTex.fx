//
//
// Default.fx

#include "Light.fx"

//VS 출력 구조체.
struct VSOutput
{
    float4 pos		: SV_POSITION;
    float4 diff		: COLOR0;
	float4 spec		: COLOR1;
	float4 nrm		: TEXCOORD100;
};


////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main
//
VSOutput VS_Main(
				  float4 pos : POSITION,    //[입력] 정점좌표. Vertex Position (Model Space, 3D)
				  float4 nrm : NORMAL,      //[입력] 노멀 normal 
				  float2 uv  : TEXCOORD0
				)
{

	VSOutput o = (VSOutput)0;
	pos.w = 1.0f;      nrm.w = 0.0f;

    //pos = mul(pos, mWVP);	 
	pos = mul(pos, mWorld);

	// Light
	float4 diff = Light(nrm);

	pos = mul(pos, mView);

	float4 spec = Specular(pos, nrm);

	pos = mul(pos, mProj);

	o.pos = pos;
	o.diff = diff;
	o.spec = spec;
	o.nrm = nrm;

	return o;
}


////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main
//
float4 PS_Main(
	float4 pos : SV_POSITION,    //[입력] 정점좌표. (Screen, 2D)
	float4 diff : COLOR0,
	float4 spec : COLOR1
	//float4 nrm : TEXCOORD100
) : SV_TARGET
{
	//float4 lightDiff = Light(nrm);

	float4 col = diff;// +spec;

	return col;
}

