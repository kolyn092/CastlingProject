
#include "Light.fx"


//VS 출력 구조체.
struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};




////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : 정점 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
    float4 pos : POSITION,    //[입력] 정점좌표. Vertex Position (Model Space, 3D)
    float2 uv : TEXCOORD0     //[입력] 텍스처 좌표 Texture Coordiates.
    )
{
    //* 아래의 테스트를 수행하기 전에  
    //* VS 에 상수 버퍼가 설정되어 있어야 합니다.    
    pos.w = 1;

    //월드 변환.(World Transform) 
    pos = mul(pos, mWorld);

    //시야-뷰 변환 (View Transform)
    pos = mul(pos, mView);

    //원근 투영 변환 (Projection Transform)
    pos = mul(pos, mProj);


    //정보 출력.
    VSOutput o = (VSOutput)0;
    o.pos = pos;
    o.col = float4(1, 1, 1, 1);//g_Mtrl.Diffuse;        //외부 지정 색으로 출력
    o.uv = uv;


    return o;
}







////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader 전역 데이터
//

//텍스처 객체 변수: 엔진에서 공급
Texture2D texDiffuse;

//텍스처 셈플러
SamplerState smpLinear;

////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : 픽셀 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 


float4 PS_Main(
    float4 pos  : SV_POSITION,  //[입력] (보간된) 픽셀별 좌표. (Screen, 2D)
    float4 diff : COLOR0,       //[입력] (보간된) 픽셀별 색상 
    float2 uv : TEXCOORD0       //[입력] 텍스처 좌표
    ) : SV_TARGET               //[출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
{

    //텍스쳐 셈플링
   float4 tex = texDiffuse.Sample(smpLinear, uv);

   //알파테스트 Alpha-Test
   clip(tex.a == 0.0f ? -1 : 1);        //Alpha 0% 이하의 픽셀 버리기

   //텍스처 혼합 출력 (곱셈, 기본)
   float4 col = tex * diff;

   //색상 보정
   //col.rgb *= 5.0f;
   //알파 보정
   col.a = tex.a * diff.a;

   //알파선행곱셈 : Pre-multiplyed Alpha 처리
   col *= tex.a;

   return col;
}



////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main 2 : 픽셀 셰이더 메인 함수. (WireFrame 전용)
//
////////////////////////////////////////////////////////////////////////////// 


float4 PS_Main2(
    float4 pos : SV_POSITION, //[입력] (보간된) 픽셀별 좌표. (Screen, 2D)
    float4 diff : COLOR0,     //[입력] (보간된) 픽셀별 색상 
    float2 uv : TEXCOORD0     //[입력] 텍스처 좌표
    ) : SV_TARGET             //[출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
{

    //텍스처 없이 바로 출력.
    //return diff;
    return float4(1, 1, 1, 1);

}
