#pragma once
#pragma warning(disable:4996)

#include "D3D11.h"
#include "DirectXMath.h"
#include "..\\Extern\\DXTK\\Inc\\DirectXTK.h"
#include "AlignedAllocationPolicy.h"

#include <tchar.h>

#include "d3dcompiler.h"				//DX 셰이더 컴파일러 헤더.
#pragma comment(lib, "d3dcompiler")		//DX 셰이더 컴파일러 라이브러리.  D3DCompiler.dll 필요.

#pragma comment(lib, "D3D11")
#pragma comment(lib, "dxgi")


using namespace DirectX;
using namespace SimpleMath;

typedef ID3D11Buffer* LPXBUFFER;			//DX 공통 메모리 버퍼. (통상적으로 Vertex/Index/Constant 데이터를 저장)
typedef LPXBUFFER				LPVERTEXBUFFER;		//정점 버퍼.
typedef LPXBUFFER				LPINDEXBUFFER;		//인덱스 버퍼
typedef LPXBUFFER				LPCONSTBUFFER;		//상수 버퍼.

typedef XMMATRIX		MATRIXA;
typedef XMFLOAT4X4		MATRIX;
typedef XMVECTOR		VECTOR;
typedef XMFLOAT4		VECTOR4;
typedef XMFLOAT3		VECTOR3;
typedef XMFLOAT2		VECTOR2;

//색상 타입: 2가지.
//typedef XMCOLOR		COLOR;	
typedef XMFLOAT4A		COLOR;

#define D3DXToRadian(degree) ((degree) * (XM_PI / 180.0f))
#define D3DXToDegree(radian) ((radian) * (180.0f / XM_PI))

#ifndef IsKeyDown
#define IsKeyDown(key)	((GetAsyncKeyState(key)&0x8000) == 0x8000)
#define IsKeyUp(key)	((GetAsyncKeyState(key)&0x8001) == 0x8001)
#endif 

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(pBuff) if((pBuff)){ (pBuff)->Release(); (pBuff) = NULL; }
#define SAFE_DELETE(pBuff)	if((pBuff)){ delete (pBuff); (pBuff) = NULL; }
#define SAFE_DELARRY(pBuff) if((pBuff)){ delete [] (pBuff); (pBuff) = NULL; }
#endif

#ifndef IsKeyDown
#define IsKeyDown(key)	((GetAsyncKeyState(key)&0x8000) == 0x8000)
#define IsKeyUp(key)	((GetAsyncKeyState(key)&0x8001) == 0x8001)
#endif 

#define MS_STANDBY   0x0000		// 대기중
#define MS_LBTNDOWN  0x0100		// 버튼 눌림
#define MS_LBTNUP	 0x8100		// 버튼 눌린후 떨어짐
#define MS_RBTNDOWN  0x0001		// 버튼 눌림
#define MS_RBTNUP	 0x0081		// 버튼 눌린후 떨어짐

#ifndef YES_
#define YES_ TRUE
#define NO_  FALSE
#endif

//////////////////////////////////////////////////////////////////////////
//
// 문자열 비교 매크로
//
//////////////////////////////////////////////////////////////////////////
#ifndef EQUAL
#define EQUAL( a, b ) ( _tcsicmp((a), (b)) == 0)
#define NOT_EQUAL( a, b ) ( _tcsicmp((a), (b)) != 0)
#endif

#ifndef CHK_OK
#define CHK_OK		0x00000000					// 특별한 문제 없음. 1 이상은 에러코드로 판정.
//#define CHK_OK		0x00000001					// 특별한 문제 없음. 1 이상은 에러코드로 판정.
#define CHK_ERROR	-1
#define CHK_FAIL		CHK_ERROR
#define CHECKFAILED(a)  ( (a) != 0 )				// 에러, 실패!
#define CHKOK(a)  ( (a) == 0 )					// 에러 없음, 성공!
#endif


const int g_ScreenWidth = 1920;
const int g_ScreenHeight = 1080;
