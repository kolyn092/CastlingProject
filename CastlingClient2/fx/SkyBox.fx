


// �⺻ �������
cbuffer cbDEFAULT : register(b0)
{
    matrix mViewInv;     //���� ���. 
    matrix mProjInv;   //���� ��ȯ ���. 
};

cbuffer cbExtra : register(b1)
{
    float4 diffuse;
    float z_far;     //ī�޶� �ִ�Ÿ� 
};


//VS ��� ����ü.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float3 uvw  : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : ���� ���̴� ���� �Լ�.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
    float2 pos : POSITION //[�Է�] �ؽ�ó ��ǥ Texture Coordiates.
)
{

    float4 uvw = float4(pos.xy, 1, 1);


    uvw = mul(uvw, mProjInv);
    uvw = uvw / uvw.w;
    uvw = mul(uvw, mViewInv);
    uvw = normalize(uvw);



    //���� ���.
    VSOutput o = (VSOutput)0;
    o.pos = float4(pos.xy, 1.0f, 1.0f);
    o.uvw = uvw.xyz;
    return o;
}


////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader ���� ������.
//

TextureCube texSkyBox : register(t0);     //�������� ���� ����. 

//�ؽ�ó ���÷�.(��������)
SamplerState smpLinear;
/*
SamplerState smpLinear;
{
    Filter = MIN_MAG_MIP_LINEAR; //���߼��� ���͸� (Tril-Linear Filter)
    AddressU = Wrap;
    AddressV = Wrap;
};
*/

////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : �ȼ� ���̴� ���� �Լ�.
//
////////////////////////////////////////////////////////////////////////////// 

float4 PS_Main(
    float4 pos : SV_POSITION,  //[�Է�] (������) �ȼ��� ��ǥ. (Screen, 2D)
    float3 uvw : TEXCOORD0   //[�Է�] �ؽ�ó ��ǥ.
) : SV_TARGET               //[���] ����.(�ʼ�), "����Ÿ��" ���� ����մϴ�.
{
   float4 tex = 1;
   tex = texSkyBox.Sample(smpLinear, normalize(uvw));

   float4 col = tex * diffuse;

   return col;

}


