//
//
// Tex.fx

#include "Light.fx"

//VS 출력 구조체.
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



//텍스처 객체 변수: 엔진에서 공급됨.
Texture2D texDiffuse;
//레지스터 직접 지정. (기본값은 t0)
//Texture2D texDiffuse : register(t0);     

//텍스처 셈플러. (엔진지정)
SamplerState smpLinear;


////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main
//
float4 PS_Main(
	float4 pos : SV_POSITION,    //[입력] 정점좌표. (Screen, 2D)
	float4 diff : COLOR0,        //[입력] 정점색. (Vertex Color : "Diffuse")
	float4 spec : COLOR1,
	float4 nrm : TEXCOORD100,
	float2 uv : TEXCOORD0
	) : SV_TARGET
{
	float4 tex = texDiffuse.Sample(smpLinear, uv);

	// 감마코렉션(인코딩->리니어: 어둡게)
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

	// 감마코렉션(리니어->인코딩: 밝게)
	col = pow(col, 0.5f);

	// just color
	//float4 col = diff;

	return col;
}

