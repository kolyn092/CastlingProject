#pragma once
#pragma warning(disable:4996)

#include "D3D11.h"
#include "DirectXMath.h"
#include "..\\Extern\\DXTK\\Inc\\DirectXTK.h"
#include "AlignedAllocationPolicy.h"

#include <tchar.h>

#include "d3dcompiler.h"				//DX ���̴� �����Ϸ� ���.
#pragma comment(lib, "d3dcompiler")		//DX ���̴� �����Ϸ� ���̺귯��.  D3DCompiler.dll �ʿ�.

#pragma comment(lib, "D3D11")
#pragma comment(lib, "dxgi")


using namespace DirectX;
using namespace SimpleMath;

typedef ID3D11Buffer* LPXBUFFER;			//DX ���� �޸� ����. (��������� Vertex/Index/Constant �����͸� ����)
typedef LPXBUFFER				LPVERTEXBUFFER;		//���� ����.
typedef LPXBUFFER				LPINDEXBUFFER;		//�ε��� ����
typedef LPXBUFFER				LPCONSTBUFFER;		//��� ����.

typedef XMMATRIX		MATRIXA;
typedef XMFLOAT4X4		MATRIX;
typedef XMVECTOR		VECTOR;
typedef XMFLOAT4		VECTOR4;
typedef XMFLOAT3		VECTOR3;
typedef XMFLOAT2		VECTOR2;

//���� Ÿ��: 2����.
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

#define MS_STANDBY   0x0000		// �����
#define MS_LBTNDOWN  0x0100		// ��ư ����
#define MS_LBTNUP	 0x8100		// ��ư ������ ������
#define MS_RBTNDOWN  0x0001		// ��ư ����
#define MS_RBTNUP	 0x0081		// ��ư ������ ������

#ifndef YES_
#define YES_ TRUE
#define NO_  FALSE
#endif

//////////////////////////////////////////////////////////////////////////
//
// ���ڿ� �� ��ũ��
//
//////////////////////////////////////////////////////////////////////////
#ifndef EQUAL
#define EQUAL( a, b ) ( _tcsicmp((a), (b)) == 0)
#define NOT_EQUAL( a, b ) ( _tcsicmp((a), (b)) != 0)
#endif

#ifndef CHK_OK
#define CHK_OK		0x00000000					// Ư���� ���� ����. 1 �̻��� �����ڵ�� ����.
//#define CHK_OK		0x00000001					// Ư���� ���� ����. 1 �̻��� �����ڵ�� ����.
#define CHK_ERROR	-1
#define CHK_FAIL		CHK_ERROR
#define CHECKFAILED(a)  ( (a) != 0 )				// ����, ����!
#define CHKOK(a)  ( (a) == 0 )					// ���� ����, ����!
#endif


const int g_ScreenWidth = 1920;
const int g_ScreenHeight = 1080;
