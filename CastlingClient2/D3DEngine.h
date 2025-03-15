#pragma once
#define _WINSOCK2API_
#include <Windows.h>
#include "DXEngineDefine.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"

///
///
///  DX11 엔진 클래스
///
///  [8/11/2020 Kolyn]

// 깊이/스텐실 테스트 상태 객체
enum {
	DS_DEPTH_ON,		//깊이버퍼 ON! (기본값), 스텐실버퍼 OFF.
	DS_DEPTH_OFF,		//깊이버퍼 OFF!, 스텐실 OFF.
	DS_DEPTH_WRITE_OFF,	//깊이버퍼 쓰기 끄기, 스텐실 OFF. 

	//스텐실 버퍼 연산용.
	DS_DEPTH_ON_STENCIL_ON,				//깊이버퍼 On (Write On), 스텐실버퍼 ON (참조값 쓰기) : "깊이/스텐실 기록".
	DS_DEPTH_ON_STENCIL_EQUAL_KEEP,		//깊이버퍼 On (Write On), 스텐실버퍼 ON (동일비교, 성공시 유지) : "지정 위치에만 그리기".
	DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP,	//깊이버퍼 On (Write On), 스텐실버퍼 ON (다름비교, 성공시 유지) : "지정 위치 이외에 그리기".

	//DS_DEPTH_OFF_STENCIL_ON,			//깊이버퍼 Off, 스텐실버퍼 ON (참조값 쓰기) : "스텐실만 기록"
	//DS_DEPTH_ON_STENCIL_EQUAL_INCR,	//깊이버퍼 On (Write On), 스텐실버퍼 ON (동일비교, 성공시 증가) : "이중그리기 방지".
	DS_DEPTH_WRITE_OFF_STENCIL_ON,		//깊이버퍼 On (Write Off), 스텐실버퍼 ON (항상기록, 성공시 증가) :" 스텐실만 기록".
	DS_DEPTH_OFF_STENCIL_EQUAL_INCR,	//깊이버퍼 Off (Write Off), 스텐실버퍼 ON (동일비교, 성공시 증가) : "이중그리기 방지.


	//열거 상수 최대값.
	DS_MAX_,

	//상수 재정의. MAX 값 뒷쪽에 정의해야 함. 주의.
	DS_DEPTH_ON_STENCIL_OFF = DS_DEPTH_ON,			//깊이버퍼 ON, 스텐실버퍼 OFF
	DS_DEPTH_ONLY = DS_DEPTH_ON_STENCIL_OFF,		//깊이버퍼 ON, 스텐실버퍼 OFF
	//DS_STENCIL_ONLY = DS_DEPTH_OFF_STENCIL_ON,	//깊이버퍼 OFF, 스텐실버퍼 ON
};

// 레스터라이져 상태 객체
enum {
	RS_SOLID,				//삼각형 채우기 : Fill Mode - Soild.
	RS_WIREFRAME,				//삼각형 채우기 : Fill Mode - Wireframe.
	RS_CULLBACK,			//뒷면 컬링 (ON) : Back-Face Culling - "CCW" 
	RS_CULLFRONT,			//정면 컬링 (ON) : Front-Face Culling - "CW" 	
	RS_WIRECULLBACK,		//와이어 프레임 + 뒷면 컬링 (ON) 
	RS_WIRECULLFRONT,		//와이어 프레임 + 앞면 컬링 (ON) 	

	//열거 상수 최대값.
	RS_MAX_,

	//상수 재정의. MAX 값 뒷쪽에 정의해야 함. 주의.
	RS_CULL_CCW = RS_CULLBACK,
	RS_CULL_CW = RS_CULLFRONT,
	RS_WIRE_CULL_CCW = RS_WIRECULLBACK,
	RS_WIRE_CULL_CW = RS_WIRECULLFRONT,

};

// 텍스처 셈플러 상태 객체
enum {
	SS_CLAMP,			//텍스처 어드레스 모드 : 클램핑 (DX 기본값) 
	SS_WRAP,			//텍스처 어드레스 모드 : 렙핑 
	//SS_BOARDER,
	//SS_MIRROR,
	//SS_MIRRROONCE,

	SS_MAX,
	SS_DEFAULT = SS_WRAP,	//기본 셈플러

};

// 색상/알파 혼합 상태 객체
enum {
	BS_DEFAULT,				//기본혼합 객체.
	BS_ALPHA_BLEND,			//알파 블랜딩. (Src.a + (1-Src.a))
	BS_COLOR_BLEND,			//색상 블랜딩. ( Src.Color + (1-Src.Color))
	BS_COLOR_BLEND_ONE,		//1:1 블랜딩. ( Src.Color + Dest.Color) 

	BS_MAX_,

	BS_AB_OFF = BS_DEFAULT,
	BS_AB_ON = BS_ALPHA_BLEND,
	//BS_AB_ALPHA = BS_ALPHA_BLEND,
	//BS_AB_COLOR = BS_COLOR_BLEND,
};

enum {
	RM_SOLID = 0x0000,		// 삼각형채우기 : ON, Solid
	RM_WIREFRAME = 0x0001,		// 삼각형채우기 : OFF, Wire-frame
	RM_CULLBACK = 0x0002,		// 뒷면 컬링 : ON, "CCW"

	//렌더링 기본모드 : Solid + Cull-On.
	RM_DEFAULT = RM_SOLID | RM_CULLBACK,

};

_declspec(align(16)) struct cbDEFAULT
{
	XMMATRIX mTM;		//"World" 변환 행렬 : DirectXMath, 16바이트 정렬 버전. 
	XMMATRIX mView;		//"View" 변환 행렬
	XMMATRIX mProj;		//"Projection" 변환 행렬
	XMMATRIX mWV;		// World+view 혼합행렬. 
	XMMATRIX mWVP;		// World+view+Proj 혼합행렬. 
};

_declspec(align(16)) struct cbLIGHT
{
	XMVECTOR Direction;
	XMVECTOR Diffuse;
	XMVECTOR Ambient;
	XMVECTOR Specular;
	BOOL bOn;
};

_declspec(align(16)) struct cbMATERIAL
{
	COLOR    Diffuse;		//주 광량(확산광) 의 반사율(%) (일반+정렬형)
	COLOR    Ambient;		//보조 광량(주변광) 의 반사율(%) (일반+정렬형)
	COLOR    Specular;		//정반사광 반사율(%)
	float	 Power;			//정반사 보정값 : "거칠기"
};

class D3DEngine : public AlignedAllocationPolicy<16>
{

private:
	// D3D 관련 개체 인터페이스
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDXDC;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRTView;

	// 깊이, 스텐실 버퍼
	ID3D11Texture2D* m_pDS;
	ID3D11DepthStencilView* m_pDSView;

private:
	// 장치 설정 기본 정보
	DXGI_MODE_DESC m_Mode;
	// 현재 렌더링 모드
	DWORD m_RMode;

	// AA & AF Option
	DWORD m_dwAA;
	DWORD m_dwAF;
	BOOL m_bMipMap;

	// D3D 기능 레벨
	D3D_FEATURE_LEVEL m_FeatureLevels;
	const TCHAR* m_strFeatureLevel;

	// 장치 정보
	DXGI_ADAPTER_DESC1 m_Adc;

private:
	// 전체화면 사용여부
	BOOL m_bWindowMode;
	// 수직동기화 사용여부
	BOOL m_bVSync;
	// 뒷면 제거
	BOOL m_bCullBack;
	// 와이어 프레임
	BOOL m_bWireFrame;
	// 깊이 버퍼 연산
	BOOL m_bZEnable;
	// 깊이 버퍼 쓰기
	BOOL m_bZWrite;

private:
	const TCHAR* m_FontName;
	SpriteBatch* m_pFontBatch;
	SpriteFont* m_pFont;

private:
	XMFLOAT4 m_ClearColor;

public:
	Shader* m_DefaultShader;
	Shader* m_VertexLightShader;
	Shader* m_PixelLightShader;
	Shader* m_TexShader;
	Shader* m_TexLambertShader;
	Shader* m_NonTexShader;
	Shader* m_LineShader;

	Light* m_DirLight;

	// 기본 상수버퍼 갱신용
	cbDEFAULT		m_cbDef;
	cbLIGHT			m_cbLit;
	cbMATERIAL		m_cbMtrl;

	//셰이더 상수 버퍼.
	ID3D11Buffer* m_pCBDef;
	ID3D11Buffer* m_pCBLit;
	ID3D11Buffer* m_pCBMtrl;

public:
	Camera* m_Camera;

private:
	D3DEngine();
	~D3DEngine();

private:
	static D3DEngine* m_D3DInstance;

public:
	static D3DEngine* GetIns()
	{
		if (m_D3DInstance == nullptr)
		{
			m_D3DInstance = new D3DEngine();
		}
		return m_D3DInstance;
	}

public:
	int DXSetUp(HWND hwnd);
	int DataLoading();
	float Update();
	void SceneUpdate(float dTime);
	void RTClear(XMFLOAT4& col);
	void SceneRenderStart();
	void SceneRenderEnd();
	void DataRelease();
	void DXRelease();

	float GetEngineTime();

	int Flip();

public:
	void Mode_2D();
	void Mode_3D();

private:
	HRESULT CreateDeviceSwapChain(HWND hwnd);
	HRESULT CreateRenderTarget();
	HRESULT CreateDepthStencil();
	void SetViewPort();

	void GetDeviceInfo();
	void GetFeatureLevel();
	HRESULT GetAdapterInfo(DXGI_ADAPTER_DESC1* pAd);

	int RenderStateObjectCreate();
	void RenderStateObjectRelease();
	void RenderStateUpdate();
	void RenderModeUpdate();

	ID3D11DepthStencilState* m_DSState[DS_MAX_];
	int DepthStencilStateCreate();
	void DepthStencilStateRelease();

	ID3D11RasterizerState* m_RState[RS_MAX_] = { nullptr, };
	void RasterStateCreate();
	void RasterStateRelease();

	ID3D11SamplerState* m_pSampler[SS_MAX] = { nullptr, };
	void SamplerCreate();
	void SamplerRelease();

	ID3D11BlendState* m_BState[BS_MAX_] = { nullptr, };
	void BlendStateCreate();
	void BlendStateRelease();

	void ShaderSetup();
	void CreateShaderLayout();
	void ShaderUpdate();
	void ShaderRelease();
	void LightsUpdate(float dTime);

public:
	HRESULT LoadTexture(const TCHAR* filename, ID3D11ShaderResourceView** ppTexRV);
	HRESULT GetTextureDesc(ID3D11ShaderResourceView* pRV, D3D11_TEXTURE2D_DESC* pDesc);

public:
	HRESULT CreateVB(void* pBuff, UINT size, LPVERTEXBUFFER* ppVB);
	HRESULT CreateIB(void* pBuff, UINT size, LPINDEXBUFFER* ppIB);

public:
	HRESULT CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB);
	HRESULT UpdateDynamicConstantBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size);

public:
	int PrintError(BOOL bMBox, const TCHAR* msg, HRESULT hr, ID3DBlob* pBlob,
		const TCHAR* filename, const char* EntryPoint, const char* ShaderModel);
	int PrintError(BOOL bMBox, const TCHAR* msg, ...);

	void _TT(int x, int y, XMFLOAT4 col, const TCHAR* msg, ...);

public:
	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetDXDC() { return m_pDXDC; }
	ID3D11SamplerState* GetSampler(int index) { return m_pSampler[index]; }
	ID3D11BlendState* GetBlend(int index) { return m_BState[index]; }
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	DXGI_MODE_DESC GetDisplayMode() { return m_Mode; }
};

