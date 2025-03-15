#include "D3DEngine.h"
#include <tchar.h>

D3DEngine* D3DEngine::m_D3DInstance = nullptr;

D3DEngine::D3DEngine()
{
	m_pDevice = nullptr;
	m_pDXDC = nullptr;
	m_pSwapChain = nullptr;
	m_pRTView = nullptr;

	m_pDS = nullptr;
	m_pDSView = nullptr;

	m_Mode = { g_ScreenWidth, g_ScreenHeight, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM };

	m_RMode = RM_DEFAULT;

	m_dwAA = 4;
	m_dwAF = 8;
	m_bMipMap = TRUE;

	m_FeatureLevels = D3D_FEATURE_LEVEL_11_0;
	m_strFeatureLevel = L"N/A";

	m_bWindowMode = TRUE;
	m_bVSync = FALSE;
	m_bCullBack = FALSE;
	m_bWireFrame = FALSE;
	m_bZEnable = TRUE;
	m_bZWrite = TRUE;

	m_ClearColor = XMFLOAT4(0, 0.125f, 0.3f, 1.0f);
}

D3DEngine::~D3DEngine()
{

}

int D3DEngine::DXSetUp(HWND hwnd)
{
	// 렌더링 장치 디바이스 및 스왑 체인 생성
	CreateDeviceSwapChain(hwnd);

	// 장치 및 스왑체인의 렌더타겟 (백버퍼) 획득
	CreateRenderTarget();

	// 깊이/스텐실 버퍼 생성
	CreateDepthStencil();

	// Output Merger에 렌더링 타겟 및 깊이/스텐실 버퍼 등록
	// 렌더타겟 개수, 렌더타겟 등록, 깊이/스텐실 버퍼 등록
	m_pDXDC->OMSetRenderTargets(1, &m_pRTView, m_pDSView);

	// 뷰포트 설정
	SetViewPort();

	// 장치 정보 획득
	GetDeviceInfo();

	//////////////////////////////////////////////////////////////////////////

	// 렌더링 상태 객체 생성
	RenderStateObjectCreate();

	// 기본 셰이더 설정
	ShaderSetup();

	// 기본 카메라 설정
	m_Camera = new Camera(g_ScreenWidth, g_ScreenHeight);

	// 전체모드로 강제전환
	//m_pSwapChain->SetFullscreenState(TRUE, NULL);

	/// 폰트설정
	m_FontName = L"../Extern/Font/굴림9k.sfont";
	m_pFontBatch = new SpriteBatch(m_pDXDC);
	m_pFont = new SpriteFont(m_pDevice, m_FontName);

	return TRUE;
}

int D3DEngine::DataLoading()
{
	/// 여기에 있는 부분은 게임엔진에서 돌려야 한다.

	return TRUE;
}

float D3DEngine::Update()
{
	// 엔진 시간, 타이머 얻기
	float dTime = GetEngineTime();

	// 엔진, 시스템 장치 갱신
	SceneUpdate(dTime);
	//SystemUpdate(dTime);

	// 렌더링 상태 갱신
	RenderStateUpdate();

	// 프로그램 종료 처리
	///if (IsKeyUp(VK_ESCAPE)) PostQuitMessage(WM_QUIT);

	return dTime;
}

void D3DEngine::SceneUpdate(float dTime)
{
	// 기본셰이더 갱신 / 상수버퍼 갱신
	ShaderUpdate();

	if (m_Camera->m_mode == Camera::eCameraMode::Default)
	{
		m_Camera->UpdateDefault(dTime);
	}
	else if (m_Camera->m_mode == Camera::eCameraMode::Follow)
	{
		m_Camera->UpdateFollow(dTime);
	}

	m_cbDef.mView = m_Camera->mView;
	m_cbDef.mProj = m_Camera->mProj;

	// 라이트 갱신
	LightsUpdate(dTime);
}

void D3DEngine::RTClear(XMFLOAT4& col)
{
	// 렌더타겟 초기화. 컬러
	m_pDXDC->ClearRenderTargetView(m_pRTView, (float*)&col);
	
	// 초기화 플래그 . 깊이 버퍼 초기값 . 스텐실 버퍼 초기값
	m_pDXDC->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3DEngine::SceneRenderStart()
{
	RTClear(m_ClearColor);
}

void D3DEngine::SceneRenderEnd()
{
	// 디버그용 정보 출력
	// 그리드, 그래픽스 엔진 관련 디버깅 정보들


	// 장면 그리기 종료
	Flip();
}

void D3DEngine::DataRelease()
{
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_DefaultShader);
	SAFE_DELETE(m_LineShader);
	SAFE_DELETE(m_NonTexShader);
	SAFE_DELETE(m_PixelLightShader);
	SAFE_DELETE(m_TexShader);
	SAFE_DELETE(m_TexLambertShader);
	SAFE_DELETE(m_VertexLightShader);
	SAFE_DELETE(m_DirLight);
}

void D3DEngine::DXRelease()
{
	// 장치 상태 리셋 : 제거 전에 초기화
	if (m_pDXDC)
	{
		m_pDXDC->ClearState();
	}

	// 상태 객체 제거
	RenderStateObjectRelease();

	// 기본 셰이더 제거
	ShaderRelease();

	//
	SAFE_RELEASE(m_pDSView);
	SAFE_RELEASE(m_pDS);
	SAFE_RELEASE(m_pRTView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDXDC);
	SAFE_RELEASE(m_pDevice);
	SAFE_DELETE(m_pFontBatch);
	SAFE_DELETE(m_pFont);

	SAFE_DELETE(m_D3DInstance);
}

float D3DEngine::GetEngineTime()
{
	static int oldtime = GetTickCount();
	int nowtime = GetTickCount();
	float dTime = (nowtime - oldtime) * 0.001f;
	oldtime = nowtime;

	return dTime;
}

int D3DEngine::Flip()
{
	m_pSwapChain->Present(m_bVSync, 0);	

	return TRUE;
}

void D3DEngine::Mode_2D()
{
	// 2D 카메라 설정
	// 뷰 행렬 설정 : 단위 행렬로 설정이 필요.
	m_cbDef.mView = XMMatrixIdentity();
	// 프로젝션 행렬 설정 : Ortho-Off Projection 행렬 설정
	m_cbDef.mProj = XMMatrixOrthographicOffCenterLH(0, (float)m_Mode.Width, (float)m_Mode.Height, 0.0f, 1.0f, 100.0f);
	UpdateDynamicConstantBuffer(m_pCBDef, &m_cbDef, sizeof(cbDEFAULT));

	m_pDXDC->RSSetState(m_RState[RS_SOLID]);

	// 깊이/스텐실 버퍼 연산 설정
	m_pDXDC->OMSetDepthStencilState(m_DSState[DS_DEPTH_WRITE_OFF], 0);
}

void D3DEngine::Mode_3D()
{
	m_cbDef.mView = m_Camera->mView;
	m_cbDef.mProj = m_Camera->mProj;
	UpdateDynamicConstantBuffer(m_pCBDef, &m_cbDef, sizeof(cbDEFAULT));

	// 렌더링 상태 설정
	// 조명 켬

	// 랜더링 옵션 복구
	RenderModeUpdate();
	//m_pDXDC->RSSetState(m_RState[RS_CULL_CCW]);

	// 깊이/스텐실 연산 복구
	m_pDXDC->OMSetDepthStencilState(m_DSState[DS_DEPTH_ON], 0);
}

HRESULT D3DEngine::CreateDeviceSwapChain(HWND hwnd)
{
	HRESULT hr = S_OK;

	// 장치 (Device) 및 스왑체인(SwapChain) 정보 구성.
	// 스왑체인은 다중버퍼링 시스템을 말하며
	// 고전적인 '플립핑Flipping' 체인과 동일한 의미입니다.  
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = m_bWindowMode;				//풀스크린 또는 창모드 선택.
	sd.OutputWindow = hwnd;						//출력할 윈도우 핸들.
	sd.BufferCount = 1;							//백버퍼 개수.
	sd.BufferDesc.Width = m_Mode.Width;			//해상도 결정.(백버퍼 크기)
	sd.BufferDesc.Height = m_Mode.Height;
	sd.BufferDesc.Format = m_Mode.Format;		//백버퍼 색상규격 (A8R8G8B8) 창모드에서는 생략 가능 
	sd.BufferDesc.RefreshRate.Numerator = m_bVSync ? 60 : 0;   //버퍼 갱신율.(수직동기화 VSync 활성화시 표준갱신율 적용 : 60hz)
	//sd.BufferDesc.RefreshRate.Numerator = 0;	//버퍼 갱신율.(수직동기화 VSync Off)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//용도 설정: '렌더타겟' 
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = 0;
	sd.SampleDesc.Count = m_dwAA;		//AA 설정
	sd.SampleDesc.Quality = 0;



	// D3D 렌더링 '장치(Device)' 및 스왑체인 생성. 
	//
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,						//비디오 어뎁터 포인터 (기본장치는 NULL)
		D3D_DRIVER_TYPE_HARDWARE,	//HW 가속 
		NULL,						//SW Rasterizer DLL 핸들.  HW 가속시에는 NULL.
		0,							//디바이스 생성 플래그.(기본값)
		//D3D11_CREATE_DEVICE_DEBUG,//디바이스 생성 플래그.(디버그)
		&m_FeatureLevels,			//(생성할) 디바이스 기능 레벨(Feature Level) 배열
		1,							//(생성할) 디바이스 기능 레벨(Feature Level) 배열 크기.
		D3D11_SDK_VERSION,			//DX SDK 버전.
		&sd,						//디바이스 생성 상세 옵션.
		&m_pSwapChain,				//[출력] 스왑체인 인터페이스 얻기.
		&m_pDevice,					//[출력] 디바이스 인터페이스 얻기.
		NULL,						//[출력] (생성된) 디바이스 기능 레벨. 필요없다면 NULL 설정. 
		&m_pDXDC					//[출력] 디바이스 컨텍스트 얻기.
	);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"디바이스 / 스왑체인 생성 실패.", L"", MB_OK);
	}

	return hr;
}

HRESULT D3DEngine::CreateRenderTarget()
{
	HRESULT hr = S_OK;

	// 백버퍼 획득.
	ID3D11Texture2D* pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);		// 백버퍼 얻기, 2D 텍스쳐 행태, 화면출력용
	if (FAILED(hr))	return hr;

	//획득한 백버퍼에 렌더타겟 뷰 생성.(렌더타겟'형'으로 설정함)
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTView);		// '렌더타겟뷰'를 생성.	
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"백버퍼-렌더타겟뷰 생성 실패.", L"", MB_OK);
		//return hr;
	}

	//리소스 뷰 생성 후, 불필요한 DX 핸들은 해제해야 합니다.(메모리 누수 방지)
	SAFE_RELEASE(pBackBuffer);

	return hr;
}

HRESULT D3DEngine::CreateDepthStencil()
{
	HRESULT hr = S_OK;

	//깊이/스텐실 버퍼용 정보 구성.
	D3D11_TEXTURE2D_DESC   td;
	ZeroMemory(&td, sizeof(td));
	td.Width = m_Mode.Width;
	td.Height = m_Mode.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	//td.Format = DXGI_FORMAT_D32_FLOAT;			// 32BIT. 깊이 버퍼.
	//td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 깊이 버퍼 (24bit) + 스텐실 (8bit) / 구형 하드웨어 (DX9)
	td.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;	// 깊이 버퍼 (32bit) + 스텐실 (8bit) / 신형 하드웨어 (DX11)
	td.SampleDesc.Count = m_dwAA;				// AA 설정 - RT 과 동일 규격 준수.
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// 깊이-스텐실 버퍼용으로 설정.
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	// 깊이 버퍼 생성.
	//ID3D11Texture2D* pDS = NULL;						 
	hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pDS);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"깊이/스텐실 버퍼용 CreateTexture 실패.", L"", MB_OK);
		return hr;
	}


	// 깊이-스텐실버퍼용 리소스 뷰 정보 설정. 
	D3D11_DEPTH_STENCIL_VIEW_DESC  dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	//dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //AA 없음.
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	//+AA 설정 "MSAA"
	dd.Texture2D.MipSlice = 0;

	//깊이-스텐실 버퍼 뷰 생성.
	hr = m_pDevice->CreateDepthStencilView(m_pDS, &dd, &m_pDSView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"깊이/스텐실뷰 생성 실패.", L"", MB_OK);
		return hr;
	}

	//리소스 뷰 생성 후, 불필요한 DX 핸들은 해제해야 합니다.(메모리 누수 방지)
	//SAFE_RELEASE(g_pDS);

	return hr;
}

void D3DEngine::SetViewPort()
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)m_Mode.Width;
	vp.Height = (FLOAT)m_Mode.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pDXDC->RSSetViewports(1, &vp);
}

void D3DEngine::GetDeviceInfo()
{
	// 장치 기능 레벨 확인
	GetFeatureLevel();

	// GPU 정보 얻기
	GetAdapterInfo(&m_Adc);

}

void D3DEngine::GetFeatureLevel()
{
	const TCHAR* strFeature[4][4] =
	{
		{ L"DX9",   L"DX9.1",  L"DX9.2", L"DX9.3" },
		{ L"DX10",  L"DX10.1", L"N/A",   L"N/A" },
		{ L"DX11",  L"DX11.1", L"N/A",   L"N/A" },
		{ L"DX12",  L"DX12.1"  L"N/A",   L"N/A" }
	};

	UINT feat = m_FeatureLevels;
	UINT ver = 0;
	UINT sub = 0;

#define OFFSET 0x9;

	ver = ((feat & 0xf000) >> 12) - OFFSET;	//메인 버전 산출.   	
	sub = ((feat & 0x0f00) >> 8);			//하위 버전 산출.

	m_strFeatureLevel = strFeature[ver][sub];
}

HRESULT D3DEngine::GetAdapterInfo(DXGI_ADAPTER_DESC1* pAd)
{
	IDXGIAdapter1* pAdapter;
	IDXGIFactory1* pFactory = NULL;
	//DXGI_ADAPTER_DESC ad;

	//DXGIFactory 개체 생성. (DXGI.lib 필요)
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory)))
	{
		return E_FAIL;
	}

	pFactory->EnumAdapters1(0, &pAdapter);		//어뎁터 획득.
	pAdapter->GetDesc1(pAd);					//어뎁터 정보 획득.
	//*pAd = ad;								//외부로 복사.

	//정보 취득후, 접근한 인터페이스를 해제합니다. (메모리 누수 방지)
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

	return S_OK;
}

int D3DEngine::RenderStateObjectCreate()
{
	// 깊이/스텐실 상태 객체 생성
	DepthStencilStateCreate();

	// 레스터 상태 개체 생성
	RasterStateCreate();

	// 셈플러 상태 개체 생성
	SamplerCreate();

	// 블렌드 상태 객체 생성
	BlendStateCreate();


	return TRUE;
}

void D3DEngine::RenderStateObjectRelease()
{
	DepthStencilStateRelease();
	RasterStateRelease();
	SamplerRelease();
	BlendStateRelease();
}

void D3DEngine::RenderModeUpdate()
{

#define CheckRMode(k, v) if((k)) m_RMode |= (v); else m_RMode &= ~(v);

	// 렌더링 모드 체크 : 사용자가 지정한 렌더링 상태 조합.
	CheckRMode(m_bCullBack, RM_CULLBACK);
	CheckRMode(m_bWireFrame, RM_WIREFRAME);

	// 레스터 모드 전환 : 지정된 모드의 조합을 통해 렌더링 상태를 조절.
	switch (m_RMode)
	{
	default:
	case RM_SOLID:
		m_pDXDC->RSSetState(m_RState[RS_SOLID]);
		break;
	case RM_WIREFRAME:
		m_pDXDC->RSSetState(m_RState[RS_WIREFRAME]);
		break;
	case RM_CULLBACK:
		m_pDXDC->RSSetState(m_RState[RS_CULLBACK]);
		break;
	case RM_WIREFRAME | RM_CULLBACK:
		m_pDXDC->RSSetState(m_RState[RS_WIRECULLBACK]);
		break;
	}
}

int D3DEngine::DepthStencilStateCreate()
{
	//----------------------------
	// 깊이/스텐실 상태 개체 생성.: "출력병합기 Output Merger" 상태 조절. 
	//----------------------------
	//...	 
	D3D11_DEPTH_STENCIL_DESC  ds;
	//깊이 버퍼 설정 (기본값)
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//스텐실 버퍼 설정 (기본값) 
	ds.StencilEnable = FALSE;								//스텐실 버퍼 OFF.
	ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;	//스텐실 읽기 마스크 (8bit: 0xff)
	ds.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK; //스텐실 쓰기 마스크 (8bit: 0xff)
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	//[앞면] 스텐실 비교 함수 : "Always" 즉, 항상 성공 (통과, pass)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	//[앞면] 스텐실 비교 성공시 동작 : 기존값 유지.
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	//[앞면] 스텐실 비교 실패시 동작 : 기존값 유지.	
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	//[앞면] 스텐실/깊이 비교 실패시 동작 : 기존값 유지.
	ds.BackFace = ds.FrontFace;									//[뒷면] 설정 동일. 필요시 개별 설정이 가능.


	//----------------------------------------------------------------------
	// 깊이 버퍼 연산 객체들.
	//----------------------------------------------------------------------
	// DS 상태 객체 #0 : Z-Test ON! (기본값)
	ds.DepthEnable = TRUE;					//깊이 버퍼 On.
	ds.StencilEnable = FALSE;				//스텐실 버퍼 Off!
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON]);

	// DS 상태 객체 #1 : Z-Test OFF 상태.
	ds.DepthEnable = FALSE;
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF]);

	// DS 상태 객체 #2 : Z-Test On + Z-Write OFF.
	// Z-Test (ZEnable, DepthEnable) 이 꺼지면, Z-Write 역시 비활성화 됩니다.
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//깊이값 쓰기 끔.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_WRITE_OFF]);


	//----------------------------------------------------------------------
	// 스텐실 버퍼 연산 객체들 생성.
	//----------------------------------------------------------------------
	// 스텐실 버퍼 비트 연산 공식.
	// (Stencil.Ref & Stencil.Mask) Comparison-Func ( StencilBuffer.Value & Stencil.Mask)
	//
	// *StencilBufferValue : 현재 검사할 픽셀의 스텐실값.
	// *ComFunc : 비교 함수. ( > < >= <= ==  Always Never)
	//----------------------------------------------------------------------
	// DS 상태객체 #4 :  깊이버퍼 On, 스텐실버퍼 ON (항상, 참조값 쓰기) : "깊이/스텐실 기록" 
	ds.DepthEnable = TRUE;										//깊이버퍼 ON! (기본값)
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//비교함수 : "항상 통과" (성공)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//성공시 : 참조값(Stencil Reference Value) 로 교체.
	//ds.FrontFace.StencilFailOp	  = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.(기본값, 생략)
	//ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.(기본값, 생략)
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_ON]);


	// DS 상태객체 #5 : 깊이버퍼 On, 스텐실버퍼 ON (동일비교, 성공시 유지) : "지정 위치에만 그리기" 
	//ds.DepthEnable	= TRUE;										//깊이버퍼 ON! (기본값)(생략)
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//비교함수 : "동일한가?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//성공시 : 유지.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_EQUAL_KEEP]);


	// DS 상태객체 #6 : 깊이버퍼 On, 스텐실버퍼 ON (다름비교, 성공시 유지) : "지정 위치 이외에 그리기" 
	//ds.DepthEnable	= TRUE;										//깊이버퍼 ON! (기본값)(생략)
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON!
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	//비교함수 : "같이 않은가?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//성공시 : 유지.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP]);


	/*// DS 상태객체 #7 : 깊이버퍼 Off, 스텐실버퍼 ON (참조값 쓰기) : "스텐실만 기록"
	ds.DepthEnable	  = FALSE;										//깊이버퍼 OFF!
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON!
	ds.FrontFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;		//비교함수 : "항상 통과" (성공)
	ds.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_REPLACE;		//성공시 : 참조값(Stencil Reference Value) 로 교체.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF_STENCIL_ON]);
	*/

	/*// DS 상태객체 #8 : 깊이버퍼 On, 스텐실버퍼 ON (동일비교, 성시 증가) : "이중그리기 방지용"
	//ds.DepthEnable	= TRUE;										//깊이버퍼 ON! (기본값)(생략)
	//ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//ds.DepthFunc		= D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON!
	ds.FrontFace.StencilFunc		= D3D11_COMPARISON_EQUAL;		//비교함수 : "동일한가?"
	ds.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_INCR;		//성공시 : 증가 (+1)
	ds.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_EQUAL_INCR]);
	*/

	// DS 상태객체 #9 : 깊이버퍼 On, 스텐실버퍼 ON (항상, 성공시 증가) : "스텐실만 기록" 
	ds.DepthEnable = TRUE;										//깊이버퍼 ON! (기본값)
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//깊이버퍼 쓰기 OFF.
	ds.DepthFunc = D3D11_COMPARISON_LESS;						//깊이연산 ON. (기본값)
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//비교함수 : "항상 통과" (성공)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//성공시 : 참조값(Stencil Reference Value) 로 교체.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_WRITE_OFF_STENCIL_ON]);


	//깊이버퍼 Off (Write Off), 스텐실버퍼 ON (동일비교, 성공시 증가) : "이중그리기 방지.
	ds.DepthEnable = FALSE;										//깊이테스트 Off!
	//ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//깊이값 쓰기 Off.
	//ds.DepthFunc		= D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//비교함수 : "동일한가?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;		//성공시 : 증가 (+1) 
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF_STENCIL_EQUAL_INCR]);


	return TRUE;
}

void D3DEngine::DepthStencilStateRelease()
{
	for (int i = 0; i < DS_MAX_; i++)
	{
		SAFE_RELEASE(m_DSState[i]);
	}
}

void D3DEngine::RasterStateCreate()
{
	//[상태객체 1] 기본 렌더링 상태 개체.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		//삼각형 색상 채우기.(기본값)
	rd.CullMode = D3D11_CULL_NONE;		//컬링 없음. (기본값은 컬링 Back)		
	rd.FrontCounterClockwise = false;   //이하 기본값...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;		//AA 적용.
	rd.AntialiasedLineEnable = false;
	//레스터라이져 상태 객체 생성.
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_SOLID]);


	//[상태객체2] 와이어 프레임 그리기. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	//레스터라이져 객체 생성.
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIREFRAME]);

	//[상태객체3] 후면 컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_CULLBACK]);

	//[상태객체4] 와이어 프레임 + 후면컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLBACK]);

	//[상태객체5] 정면 컬링 On! "CW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_CULLFRONT]);

	//[상태객체6] 와이어 프레임 + 정면컬링 On! "CW" 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_FRONT;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLFRONT]);
}

void D3DEngine::RasterStateRelease()
{
	for (int i = 0; i < RS_MAX_; i++)
	{
		SAFE_RELEASE(m_RState[i]);
	}
}

void D3DEngine::RenderStateUpdate()
{
	// 렌더링 옵션 조절 	 
	//if (IsKeyUp(VK_SPACE))	m_bWireFrame ^= TRUE;
	//if (IsKeyUp(VK_F4))		m_bCullBack ^= TRUE;

	// 배경색 설정
	if (m_bWireFrame) m_ClearColor = XMFLOAT4(0, 0.125f, 0.3f, 1.0f);
	else			  m_ClearColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	// 렌더링 모드 전환.	  
	RenderModeUpdate();

	//깊이 버퍼 동작 (기본값)
	m_pDXDC->OMSetDepthStencilState(m_DSState[DS_DEPTH_ON], 0);

	//셈플러 설정.(기본설정)
	m_pDXDC->PSSetSamplers(0, 1, &m_pSampler[SS_DEFAULT]);
}

void D3DEngine::SamplerCreate()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

	//텍스처 필터 : 비등방 필터링
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = m_dwAF;
	//밉멥핑	
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	sd.MipLODBias = 0;
	//이하 기본값 처리..
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 1;
	sd.BorderColor[1] = 1;
	sd.BorderColor[2] = 1;
	sd.BorderColor[3] = 1;

	//샘플러 객체1 생성. (DX 기본값)
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = m_pDevice->CreateSamplerState(&sd, &m_pSampler[SS_CLAMP]);

	//샘플러 객체2 생성.
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = m_pDevice->CreateSamplerState(&sd, &m_pSampler[SS_WRAP]);
}

void D3DEngine::SamplerRelease()
{
	for (int i = 0; i < SS_MAX; i++)
	{
		SAFE_RELEASE(m_pSampler[i]);
	}
}

void D3DEngine::BlendStateCreate()
{
	//------------------------------
// 혼합객체1 : 색상혼합 없음. "Blending OFF"
//------------------------------
//블렌딩 상태 객체 구성 옵션 : 이하 기본값들.
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.RenderTarget[0].BlendEnable = FALSE;							//블렌딩 동작 결정. 기본값은 FALSE(OFF)		
	//색상 성분 혼합 : Color Blending.(기본값)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산(Color - Operation), 기본값은 덧셈 : 최종색 = Src.Color + Dest.Color 	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//소스(원본) 혼합 비율, 원본 100% : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;				//타겟(대상) 혼합 비율, 대상 0%   : Dest.Color = Dest * 0;  통상 0번 RT 는 "백버퍼"를 의미합니다.	
	//알파 성분 혼합 : Alpha Blending.(기본값)
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;			//알파 혼합 함수(Alpha - Opertion), 기본값은 덧셈.
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;				//소스(원본) 알파 혼합 비율.
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;			//타겟(대상) 알파 혼합 비율.
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//렌더타겟에 쓰기 옵션.
	//bd.AlphaToCoverageEnable = FALSE;								//이하 추가 옵션.(기본값, 생략)
	//bd.IndependentBlendEnable = FALSE;
	m_pDevice->CreateBlendState(&bd, &m_BState[BS_DEFAULT]);		//상태 개체.생성.


	//------------------------------
	// 혼합객체2 : 알파 혼합 "Alpha Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 (덧셈, 기본값) : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//소스(원본) 혼합 비율, 원본 알파비율  : Src.Color = Src * Src.a;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		//타겟(대상) 혼합 비율, 원본 알파비율 반전 : Dest.Color = Dest * (1-Src.a);			 		
	m_pDevice->CreateBlendState(&bd, &m_BState[BS_ALPHA_BLEND]);


	//------------------------------
	// 혼합객체3 : 색상 혼합 "Color Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 (덧셈, 기본값) : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;			//소스(원본) 혼합 비율, 원본 색상  : Src.Color = Src * Src.Color;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;		//타겟(대상) 혼합 비율, 원본 색상 반전 : Dest.Color = Dest * (1-Src.Color);			 	
	m_pDevice->CreateBlendState(&bd, &m_BState[BS_COLOR_BLEND]);


	//------------------------------
	// 혼합객체3 : 색상 혼합 "밝기 강조 Blending 1:1" 
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 "밝기강조" : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//소스(원본) 혼합 비율, 원본 색상  : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;					//타겟(대상) 혼합 비율, 원본 색상 반전 : Dest.Color = Dest * 1;			 	
	m_pDevice->CreateBlendState(&bd, &m_BState[BS_COLOR_BLEND_ONE]);

}

void D3DEngine::BlendStateRelease()
{
	for (int i = 0; i < BS_MAX_; i++)
	{
		SAFE_RELEASE(m_BState[i]);
	}
}

void D3DEngine::ShaderSetup()
{
	m_DefaultShader = new Shader();
	m_LineShader = new Shader();
	m_NonTexShader = new Shader();
	m_PixelLightShader = new Shader();
	m_TexShader = new Shader();
	m_TexLambertShader = new Shader();
	m_VertexLightShader = new Shader();

	m_DirLight = new Light();

	// 셰이더 생성
	m_DefaultShader->ShaderLoad(L"./fx/Default.fx");
	m_LineShader->ShaderLoad(L"./fx/Line.fx");
	m_NonTexShader->ShaderLoad(L"./fx/NonTex.fx");
	m_PixelLightShader->ShaderLoad(L"./fx/PixelLight.fx");
	m_TexShader->ShaderLoad(L"./fx/Tex.fx");
	m_TexLambertShader->ShaderLoad(L"./fx/TexLambert.fx");
	m_VertexLightShader->ShaderLoad(L"./fx/VertexLight.fx");

	CreateShaderLayout();

	// 기본 상수버퍼 생성.(전역,공유)
	ZeroMemory(&m_cbDef, sizeof(cbDEFAULT));
	CreateDynamicConstantBuffer(sizeof(cbDEFAULT), &m_cbDef, &m_pCBDef);
	//셰이더 상수버퍼 갱신.(동적버퍼)
	//UpdateDynamicConstantBuffer(g_pDXDC, g_pCB, &g_CBuffer, sizeof(ConstBuffer));

	// 조명 상수버퍼 생성.
	ZeroMemory(&m_cbLit, sizeof(cbLIGHT));
	CreateDynamicConstantBuffer(sizeof(cbLIGHT), &m_cbLit, &m_pCBLit);

	// 재질 상수버퍼 생성.
	ZeroMemory(&m_cbMtrl, sizeof(cbMATERIAL));
	CreateDynamicConstantBuffer(sizeof(cbMATERIAL), &m_cbMtrl, &m_pCBMtrl);

	//기본 셰이더 설정.
	m_pDXDC->VSSetShader(m_TexLambertShader->m_pVS, nullptr, 0);
	m_pDXDC->PSSetShader(m_TexLambertShader->m_pPS, nullptr, 0);

	//기본 셰이더 상수버퍼 설정.
	m_pDXDC->VSSetConstantBuffers(0, 1, &m_pCBDef);
	///m_pDXDC->VSSetConstantBuffers(1, 1, &m_pCBLit);
	///m_pDXDC->VSSetConstantBuffers(2, 1, &m_pCBMtrl);
}

void D3DEngine::CreateShaderLayout()
{
	// 기본 정점 입력구조 생성
	/// 모델마다 있어야 할지도..? 아니면 엔진에서 종류별로 가지고 있어도.
	D3D11_INPUT_ELEMENT_DESC layoutDefault[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		  0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC layoutLine[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_DefaultShader->CreateVBLayout(layoutDefault, ARRAYSIZE(layoutDefault));
	m_LineShader->CreateVBLayout(layoutLine, ARRAYSIZE(layoutLine));
	m_NonTexShader->CreateVBLayout(layoutDefault, ARRAYSIZE(layoutDefault));
	m_PixelLightShader->CreateVBLayout(layoutDefault, ARRAYSIZE(layoutDefault));
	m_TexShader->CreateVBLayout(layoutDefault, ARRAYSIZE(layoutDefault));
	m_TexLambertShader->CreateVBLayout(layoutDefault, ARRAYSIZE(layoutDefault));
	m_VertexLightShader->CreateVBLayout(layoutDefault, ARRAYSIZE(layoutDefault));
}

void D3DEngine::ShaderUpdate()
{
	// 최종 행렬 결합
	m_cbDef.mWV = m_cbDef.mTM * m_cbDef.mView;
	m_cbDef.mWVP = m_cbDef.mTM * m_cbDef.mView * m_cbDef.mProj;

	//셰이더 상수 버퍼 갱신
	UpdateDynamicConstantBuffer(m_pCBDef, &m_cbDef, sizeof(cbDEFAULT));

	//기본 셰이더 상수버퍼 설정.
	m_pDXDC->VSSetConstantBuffers(0, 1, &m_pCBDef);
}

void D3DEngine::ShaderRelease()
{
	delete m_DefaultShader;
	delete m_LineShader;
	delete m_NonTexShader;
	delete m_PixelLightShader;
	delete m_TexShader;
	delete m_TexLambertShader;
	delete m_VertexLightShader;

	SAFE_RELEASE(m_pCBDef);
	SAFE_RELEASE(m_pCBLit);
	SAFE_RELEASE(m_pCBMtrl);
}

void D3DEngine::LightsUpdate(float dTime)
{
	m_cbLit.Direction = m_DirLight->m_Direction;
	m_cbLit.Diffuse = m_DirLight->m_Diffuse;
	m_cbLit.Ambient = m_DirLight->m_Ambient;
	m_cbLit.Specular = m_DirLight->m_Specular;
	m_cbLit.bOn = m_DirLight->m_bSpecOn;

	UpdateDynamicConstantBuffer(m_pCBLit, &m_cbLit, sizeof(cbLIGHT));
}

HRESULT D3DEngine::LoadTexture(const TCHAR* filename, ID3D11ShaderResourceView** ppTexRV)
{
	HRESULT hr = S_OK;

	ID3D11ShaderResourceView* pTexRV = nullptr;

	//밉멥+텍스처 로드.
	hr = DirectX::CreateWICTextureFromFileEx(m_pDevice, m_pDXDC, filename, 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT,
		nullptr, &pTexRV);

	if (FAILED(hr))
	{
		hr = DirectX::CreateDDSTextureFromFile(m_pDevice, filename, nullptr, &pTexRV);
		if (FAILED(hr))
		{
			PrintError(TRUE, L"텍스처 로드 실패 \nFile=%s", filename);
			return hr;
		}
	}

	//외부로 리턴.
	*ppTexRV = pTexRV;

	return hr;
}

HRESULT D3DEngine::GetTextureDesc(ID3D11ShaderResourceView* pRV, D3D11_TEXTURE2D_DESC* pDesc)
{
	HRESULT res = S_OK;

	//셰이더 리소스 뷰 정보 획득.
	D3D11_SHADER_RESOURCE_VIEW_DESC dc;
	pRV->GetDesc(&dc);

	//텍스처 정보 획득.
	ID3D11Resource* pRes = nullptr;
	pRV->GetResource(&pRes);
	ID3D11Texture2D* pTex = nullptr;
	pRes->QueryInterface<ID3D11Texture2D>(&pTex);
	if (pTex)
	{
		pTex->GetDesc(pDesc);

		//정보 획득후 인터페이스 제거.
		pTex->Release();
	}

	//정보 획득후 인터페이스 제거.
	//SAFE_RELEASE(pRes);

	return res;
}


HRESULT D3DEngine::CreateVB(void* pBuff, UINT size, LPVERTEXBUFFER* ppVB)
{
	LPVERTEXBUFFER pVB = nullptr;

	//정점 버퍼 정보 구성.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//버퍼 사용방식
	bd.ByteWidth = size;							//버퍼 크기
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//버퍼 용도 : "정점 버퍼" 용로 설정 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;								//버퍼에 들어갈 데이터 설정 : "정점들"..

	//정점 버퍼 생성.
	HRESULT hr = m_pDevice->CreateBuffer(&bd, &rd, &pVB);
	if (FAILED(hr))
	{
		//에러 검증 및 예외처리..
		//...
		PrintError(hr, L"CreateVB : 정점버퍼 생성 실패");
		return hr;
	}

	//외부로 리턴.
	*ppVB = pVB;

	return hr;
}

HRESULT D3DEngine::CreateIB(void* pBuff, UINT size, LPINDEXBUFFER* ppIB)
{
	LPINDEXBUFFER pIB = nullptr;

	// 인덱스 버퍼 정보 구성.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//버퍼 사용방식
	bd.ByteWidth = size;							//버퍼 크기
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;			//버퍼 용도 : "색인 버퍼" 용로 설정 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;								//버퍼에 들어갈 데이터 설정 : "색인 정보"..

	//색인 버퍼 생성.
	HRESULT hr = m_pDevice->CreateBuffer(&bd, &rd, &pIB);
	if (FAILED(hr))
	{
		//에러 검증 및 예외처리..
		//...
		///PrintError(hr, L"CreateIB : 인덱스버퍼 생성 실패");
		return hr;
	}

	//외부로 리턴.
	*ppIB = pIB;

	return hr;
}

HRESULT D3DEngine::CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;


	//상수 버퍼 생성전, 크기 테스트.
	//상수버퍼는 16바이트 정렬된 크기로 생성해야 합니다.
	if (size % 16)
	{
		// 16바이트 정렬실패... 메세지 출력..
		PrintError(TRUE, L"[경고] 상수버퍼 : 16바이트 미정렬. 버퍼생성이 실패합니다. \n"
			L"CB = 0x%X \n"
			L"CB.Size = %d bytes",
			pData, size);
	}
	else
	{
		//16바이트 정렬 OK.메세지 생략...
		///PrintError(TRUE, L"[알림] 상수버퍼 : 16바이트 정렬됨. \n"
		///	L"CB = 0x%X \n"
		///	L"CB.Size = %d bytes",
		///	pData, size);
	}


	//상수 버퍼 정보 설정.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;				//동적 정점버퍼 설정.
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//CPU 접근 설정.

	//서브리소스 설정.
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pData;				//(외부) 상수 데이터 설정.
	//sd.SysMemPitch = 0;
	//sd.SysMemSlicePitch = 0;

	//상수 버퍼 생성.
	hr = m_pDevice->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
	{
		PrintError(hr, L"동적 상수버퍼 생성 실패");
		return hr;
	}

	//외부로 전달.
	*ppCB = pCB;

	return hr;
}


HRESULT D3DEngine::UpdateDynamicConstantBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	//상수버퍼 접근
	hr = m_pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	if (FAILED(hr))
	{
		PrintError(hr, L"UpdateDynamicConstantBuffer : Map 실패");
		return hr;
	}

	//상수 버퍼 갱신.
	memcpy(mr.pData, pData, size);

	//상수버퍼 닫기.
	m_pDXDC->Unmap(pBuff, 0);


	return hr;
}

int D3DEngine::PrintError(BOOL bMBox, const TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, const TCHAR* filename, const char* EntryPoint, const char* ShaderModel)
{
	//파라미터, 유니코드로 전환.
	TCHAR func[80] = L"";
	::mbstowcs(func, EntryPoint, strlen(EntryPoint));
	TCHAR sm[20] = L"";
	::mbstowcs(sm, ShaderModel, strlen(ShaderModel));


	//셰이더 오류 메세지 읽기.
	TCHAR errw[4096] = L"";
	::mbstowcs(errw, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());


	//HRESULT 에서 에러 메세지 얻기 
	//시스템으로 부터 얻는 셰이더 오류메세지는 부정확하므로 생략.
	TCHAR herr[1024] = L"아래의 오류를 확인하십시오.";
	/*FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		herr, 1024, NULL);
	*/


	//에러 메세지 붙이기.
	TCHAR errmsg[1024];
	_stprintf(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n에러코드(0x%08X) : %s \n\n%s",
		msg, filename, func, sm,
		hr, herr, errw);


	//(디버깅 중) VS 출력창으로 출력..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);
	//OutputDebugString(errw);


	//로그파일로 출력.
	//...


	//메세지 창 출력..
	if (bMBox)
	{
		MessageBox(NULL, errmsg, L"Error", MB_OK | MB_ICONERROR);
		//MessageBox(NULL, errw, L"Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;
}

int D3DEngine::PrintError(BOOL bMBox, const TCHAR* msg, ...)
{
	TCHAR msgva[2048] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);


	//(디버깅 중) VS 출력창으로 출력..
	OutputDebugString(L"\n");
	OutputDebugString(msgva);

	//로그파일로 출력.
	//...

	//메세지 창 출력..
	if (bMBox)
	{
		MessageBox(NULL, msgva, L"Error", MB_OK | MB_ICONEXCLAMATION);
		//GetLastError(hr);
	}

	return TRUE;
}

void D3DEngine::_TT(int x, int y, XMFLOAT4 col, const TCHAR* msg, ...)
{
	va_list vl;
	va_start(vl, msg);
	
	int len = _vscwprintf(msg, vl) + 1;
	TCHAR* buff = new TCHAR[len];
	
	_vstprintf_s(buff, len, msg, vl);
	va_end(vl);

	m_pFontBatch->Begin();
	m_pFont->DrawString(m_pFontBatch, buff, Vector2((float)x, (float)y), Vector4(col));
	//m_pFont->DrawString(m_pFontBatch, buff, Vector2((float)x, (float)y), Vector4(col), 0, 0, 20, SpriteEffects_None, 0.0f);
	m_pFontBatch->End();

	delete[] buff;
}

