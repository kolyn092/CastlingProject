//
//
// Light.fx


// ��� ���� Constant Buffer Variables
cbuffer ConstBuffer : register(b0) //������� 0���� ��� (�ִ� 14��. 0~13)
{
	matrix mWorld;
	matrix mView;
	matrix mProj;
	matrix mWV;
	matrix mWVP;
}

// ���� ������ ����ü
struct LIGHT
{
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;
	float4 Specular;
	bool bOn;
};

// ���� ������ ����ü
struct MATERIAL
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float  Power;
};

//���� ������ �������
cbuffer cbLIGHT : register(b1)
{
	LIGHT g_Lit;
};

//���� ������ �������
cbuffer cbMATERIAL : register(b2)
{
	MATERIAL g_Mtrl;
};

// ���� ó�� �Լ�
float4 Light(float4 nrm);
float4 MaterialLight(float4 nrm);
float4 Specular(float4 pos, float4 nrm);

////////////////////////////////////////////////////////////////////////////// 
//
// ���� ��� : ����Ʈ ������ �� ����. Lambert Lighting Model
//
float4 Light(float4 nrm)
{
	float4 N = nrm;    
	N.w = 0;
	float4 L = g_Lit.Direction;

	//��������� ������ ��ȯ.
	///N = mul(N, mWV);
	///L = mul(L, mView);

	L = mul(L, mWorld);

	//�� ���� ��ֶ�����.
	N = normalize(N);
	L = normalize(L);

	//���� ��� 
	float4 diff = max(dot(N, L), 0) * g_Lit.Diffuse;
	float4 amb = g_Lit.Ambient;

	return diff + amb;
}

float4 MaterialLight(float4 nrm)
{
	float4 N = nrm;
	N.w = 0;
	float4 L = g_Lit.Direction;

	//��������� ������ ��ȯ.
	///N = mul(N, mWV);
	///L = mul(L, mView);

	L = mul(L, mWorld);

	//�� ���� ��ֶ�����.
	N = normalize(N);
	L = normalize(L);

	//���� ��� 
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