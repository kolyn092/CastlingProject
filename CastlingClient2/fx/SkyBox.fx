


// 기본 상수버퍼
cbuffer cbDEFAULT : register(b0)
{
    matrix mViewInv;     //월드 행렬. 
    matrix mProjInv;   //투영 변환 행렬. 
};

cbuffer cbExtra : register(b1)
{
    float4 diffuse;
    float z_far;     //카메라 최대거리 
};


//VS 출력 구조체.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float3 uvw  : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : 정점 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
    float2 pos : POSITION //[입력] 텍스처 좌표 Texture Coordiates.
)
{

    float4 uvw = float4(pos.xy, 1, 1);


    uvw = mul(uvw, mProjInv);
    uvw = uvw / uvw.w;
    uvw = mul(uvw, mViewInv);
    uvw = normalize(uvw);



    //정보 출력.
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
// Pixel Shader 전역 데이터.
//

TextureCube texSkyBox : register(t0);     //레지스터 직접 지정. 

//텍스처 셈플러.(엔진지정)
SamplerState smpLinear;
/*
SamplerState smpLinear;
{
    Filter = MIN_MAG_MIP_LINEAR; //삼중선형 필터링 (Tril-Linear Filter)
    AddressU = Wrap;
    AddressV = Wrap;
};
*/

////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : 픽셀 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

float4 PS_Main(
    float4 pos : SV_POSITION,  //[입력] (보간된) 픽셀별 좌표. (Screen, 2D)
    float3 uvw : TEXCOORD0   //[입력] 텍스처 좌표.
) : SV_TARGET               //[출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
{
   float4 tex = 1;
   tex = texSkyBox.Sample(smpLinear, normalize(uvw));

   float4 col = tex * diffuse;

   return col;

}


