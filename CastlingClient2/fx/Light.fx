//
//
// Light.fx


// 상수 버퍼 Constant Buffer Variables
cbuffer ConstBuffer : register(b0) //상수버퍼 0번에 등록 (최대 14개. 0~13)
{
	matrix mWorld;
	matrix mView;
	matrix mProj;
	matrix mWV;
	matrix mWVP;
}

// 조명 정보용 구조체
struct LIGHT
{
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;
	float4 Specular;
	bool bOn;
};

// 재질 정보용 구조체
struct MATERIAL
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float  Power;
};

//조명 정보용 상수버퍼
cbuffer cbLIGHT : register(b1)
{
	LIGHT g_Lit;
};

//재질 정보용 상수버퍼
cbuffer cbMATERIAL : register(b2)
{
	MATERIAL g_Mtrl;
};

// 조명 처리 함수
float4 Light(float4 nrm);
float4 MaterialLight(float4 nrm);
float4 Specular(float4 pos, float4 nrm);

////////////////////////////////////////////////////////////////////////////// 
//
// 조명 계산 : 램버트 라이팅 모델 적용. Lambert Lighting Model
//
float4 Light(float4 nrm)
{
	float4 N = nrm;    
	N.w = 0;
	float4 L = g_Lit.Direction;

	//뷰공간으로 정보를 변환.
	///N = mul(N, mWV);
	///L = mul(L, mView);

	L = mul(L, mWorld);

	//각 벡터 노멀라이즈.
	N = normalize(N);
	L = normalize(L);

	//조명 계산 
	float4 diff = max(dot(N, L), 0) * g_Lit.Diffuse;
	float4 amb = g_Lit.Ambient;

	return diff + amb;
}

float4 MaterialLight(float4 nrm)
{
	float4 N = nrm;
	N.w = 0;
	float4 L = g_Lit.Direction;

	//뷰공간으로 정보를 변환.
	///N = mul(N, mWV);
	///L = mul(L, mView);

	L = mul(L, mWorld);

	//각 벡터 노멀라이즈.
	N = normalize(N);
	L = normalize(L);

	//조명 계산 
	float4 diff = max(dot(N, L), 0) * g_Lit.Diffuse * g_Mtrl.Diffuse;
	float4 amb = g_Lit.Ambient * g_Mtrl.Ambient;

	return diff + amb;
}


float4 Specular(float4 pos, float4 nrm)
{
	float4 V = -pos;
	float4 L = g_Lit.Direction;     L.w = 0;
	float4 N = nrm; N.w = 0;

	L = mul(L, mView);
	N = mul(N, mWV);

	float4 H = normalize(normalize(V) + normalize(L));

	float4 spec = pow(max(dot(H, N), 0), g_Mtrl.Power) * (g_Mtrl.Specular) * g_Lit.Specular;

	spec = saturate(spec);

	return spec;
}