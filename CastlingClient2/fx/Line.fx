//
//
// Line.fx

cbuffer ConstBuffer : register(b0) //상수버퍼 0번에 등록 (최대 14개. 0~13)
{
	matrix mWorld;
	matrix mView;
	matrix mProj;
	matrix mWV;
	matrix mWVP;
}

//VS 출력 구조체.
struct VSOutput
{
    float4 pos		: SV_POSITION;
};


////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main
//
VSOutput VS_Main(
				  float4 pos : POSITION    //[입력] 정점좌표. Vertex Position (Model Space, 3D)
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
	float2 uv : TEXCOORD0
	) : SV_TARGET
{

	float4 col = float4(0, 1, 0, 1);

	return col;
}

