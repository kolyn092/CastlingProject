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
	// ������ ��ġ ����̽� �� ���� ü�� ����
	CreateDeviceSwapChain(hwnd);

	// ��ġ �� ����ü���� ����Ÿ�� (�����) ȹ��
	CreateRenderTarget();

	// ����/���ٽ� ���� ����
	CreateDepthStencil();

	// Output Merger�� ������ Ÿ�� �� ����/���ٽ� ���� ���
	// ����Ÿ�� ����, ����Ÿ�� ���, ����/���ٽ� ���� ���
	m_pDXDC->OMSetRenderTargets(1, &m_pRTView, m_pDSView);

	// ����Ʈ ����
	SetViewPort();

	// ��ġ ���� ȹ��
	GetDeviceInfo();

	//////////////////////////////////////////////////////////////////////////

	// ������ ���� ��ü ����
	RenderStateObjectCreate();

	// �⺻ ���̴� ����
	ShaderSetup();

	// �⺻ ī�޶� ����
	m_Camera = new Camera(g_ScreenWidth, g_ScreenHeight);

	// ��ü���� ������ȯ
	//m_pSwapChain->SetFullscreenState(TRUE, NULL);

	/// ��Ʈ����
	m_FontName = L"../Extern/Font/����9k.sfont";
	m_pFontBatch = new SpriteBatch(m_pDXDC);
	m_pFont = new SpriteFont(m_pDevice, m_FontName);

	return TRUE;
}

int D3DEngine::DataLoading()
{
	/// ���⿡ �ִ� �κ��� ���ӿ������� ������ �Ѵ�.

	return TRUE;
}

float D3DEngine::Update()
{
	// ���� �ð�, Ÿ�̸� ���
	float dTime = GetEngineTime();

	// ����, �ý��� ��ġ ����
	SceneUpdate(dTime);
	//SystemUpdate(dTime);

	// ������ ���� ����
	RenderStateUpdate();

	// ���α׷� ���� ó��
	///if (IsKeyUp(VK_ESCAPE)) PostQuitMessage(WM_QUIT);

	return dTime;
}

void D3DEngine::SceneUpdate(float dTime)
{
	// �⺻���̴� ���� / ������� ����
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

	// ����Ʈ ����
	LightsUpdate(dTime);
}

void D3DEngine::RTClear(XMFLOAT4& col)
{
	// ����Ÿ�� �ʱ�ȭ. �÷�
	m_pDXDC->ClearRenderTargetView(m_pRTView, (float*)&col);
	
	// �ʱ�ȭ �÷��� . ���� ���� �ʱⰪ . ���ٽ� ���� �ʱⰪ
	m_pDXDC->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3DEngine::SceneRenderStart()
{
	RTClear(m_ClearColor);
}

void D3DEngine::SceneRenderEnd()
{
	// ����׿� ���� ���
	// �׸���, �׷��Ƚ� ���� ���� ����� ������


	// ��� �׸��� ����
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
	// ��ġ ���� ���� : ���� ���� �ʱ�ȭ
	if (m_pDXDC)
	{
		m_pDXDC->ClearState();
	}

	// ���� ��ü ����
	RenderStateObjectRelease();

	// �⺻ ���̴� ����
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
	// 2D ī�޶� ����
	// �� ��� ���� : ���� ��ķ� ������ �ʿ�.
	m_cbDef.mView = XMMatrixIdentity();
	// �������� ��� ���� : Ortho-Off Projection ��� ����
	m_cbDef.mProj = XMMatrixOrthographicOffCenterLH(0, (float)m_Mode.Width, (float)m_Mode.Height, 0.0f, 1.0f, 100.0f);
	UpdateDynamicConstantBuffer(m_pCBDef, &m_cbDef, sizeof(cbDEFAULT));

	m_pDXDC->RSSetState(m_RState[RS_SOLID]);

	// ����/���ٽ� ���� ���� ����
	m_pDXDC->OMSetDepthStencilState(m_DSState[DS_DEPTH_WRITE_OFF], 0);
}

void D3DEngine::Mode_3D()
{
	m_cbDef.mView = m_Camera->mView;
	m_cbDef.mProj = m_Camera->mProj;
	UpdateDynamicConstantBuffer(m_pCBDef, &m_cbDef, sizeof(cbDEFAULT));

	// ������ ���� ����
	// ���� ��

	// ������ �ɼ� ����
	RenderModeUpdate();
	//m_pDXDC->RSSetState(m_RState[RS_CULL_CCW]);

	// ����/���ٽ� ���� ����
	m_pDXDC->OMSetDepthStencilState(m_DSState[DS_DEPTH_ON], 0);
}

HRESULT D3DEngine::CreateDeviceSwapChain(HWND hwnd)
{
	HRESULT hr = S_OK;

	// ��ġ (Device) �� ����ü��(SwapChain) ���� ����.
	// ����ü���� ���߹��۸� �ý����� ���ϸ�
	// �������� '�ø���Flipping' ü�ΰ� ������ �ǹ��Դϴ�.  
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = m_bWindowMode;				//Ǯ��ũ�� �Ǵ� â��� ����.
	sd.OutputWindow = hwnd;						//����� ������ �ڵ�.
	sd.BufferCount = 1;							//����� ����.
	sd.BufferDesc.Width = m_Mode.Width;			//�ػ� ����.(����� ũ��)
	sd.BufferDesc.Height = m_Mode.Height;
	sd.BufferDesc.Format = m_Mode.Format;		//����� ����԰� (A8R8G8B8) â��忡���� ���� ���� 
	sd.BufferDesc.RefreshRate.Numerator = m_bVSync ? 60 : 0;   //���� ������.(��������ȭ VSync Ȱ��ȭ�� ǥ�ذ����� ���� : 60hz)
	//sd.BufferDesc.RefreshRate.Numerator = 0;	//���� ������.(��������ȭ VSync Off)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//�뵵 ����: '����Ÿ��' 
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = 0;
	sd.SampleDesc.Count = m_dwAA;		//AA ����
	sd.SampleDesc.Quality = 0;



	// D3D ������ '��ġ(Device)' �� ����ü�� ����. 
	//
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,						//���� ��� ������ (�⺻��ġ�� NULL)
		D3D_DRIVER_TYPE_HARDWARE,	//HW ���� 
		NULL,						//SW Rasterizer DLL �ڵ�.  HW ���ӽÿ��� NULL.
		0,							//����̽� ���� �÷���.(�⺻��)
		//D3D11_CREATE_DEVICE_DEBUG,//����̽� ���� �÷���.(�����)
		&m_FeatureLevels,			//(������) ����̽� ��� ����(Feature Level) �迭
		1,							//(������) ����̽� ��� ����(Feature Level) �迭 ũ��.
		D3D11_SDK_VERSION,			//DX SDK ����.
		&sd,						//����̽� ���� �� �ɼ�.
		&m_pSwapChain,				//[���] ����ü�� �������̽� ���.
		&m_pDevice,					//[���] ����̽� �������̽� ���.
		NULL,						//[���] (������) ����̽� ��� ����. �ʿ���ٸ� NULL ����. 
		&m_pDXDC					//[���] ����̽� ���ؽ�Ʈ ���.
	);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"����̽� / ����ü�� ���� ����.", L"", MB_OK);
	}

	return hr;
}

HRESULT D3DEngine::CreateRenderTarget()
{
	HRESULT hr = S_OK;

	// ����� ȹ��.
	ID3D11Texture2D* pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);		// ����� ���, 2D �ؽ��� ����, ȭ����¿�
	if (FAILED(hr))	return hr;

	//ȹ���� ����ۿ� ����Ÿ�� �� ����.(����Ÿ��'��'���� ������)
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTView);		// '����Ÿ�ٺ�'�� ����.	
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"�����-����Ÿ�ٺ� ���� ����.", L"", MB_OK);
		//return hr;
	}

	//���ҽ� �� ���� ��, ���ʿ��� DX �ڵ��� �����ؾ� �մϴ�.(�޸� ���� ����)
	SAFE_RELEASE(pBackBuffer);

	return hr;
}

HRESULT D3DEngine::CreateDepthStencil()
{
	HRESULT hr = S_OK;

	//����/���ٽ� ���ۿ� ���� ����.
	D3D11_TEXTURE2D_DESC   td;
	ZeroMemory(&td, sizeof(td));
	td.Width = m_Mode.Width;
	td.Height = m_Mode.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	//td.Format = DXGI_FORMAT_D32_FLOAT;			// 32BIT. ���� ����.
	//td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// ���� ���� (24bit) + ���ٽ� (8bit) / ���� �ϵ���� (DX9)
	td.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;	// ���� ���� (32bit) + ���ٽ� (8bit) / ���� �ϵ���� (DX11)
	td.SampleDesc.Count = m_dwAA;				// AA ���� - RT �� ���� �԰� �ؼ�.
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// ����-���ٽ� ���ۿ����� ����.
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	// ���� ���� ����.
	//ID3D11Texture2D* pDS = NULL;						 
	hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pDS);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"����/���ٽ� ���ۿ� CreateTexture ����.", L"", MB_OK);
		return hr;
	}


	// ����-���ٽǹ��ۿ� ���ҽ� �� ���� ����. 
	D3D11_DEPTH_STENCIL_VIEW_DESC  dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	//dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //AA ����.
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	//+AA ���� "MSAA"
	dd.Texture2D.MipSlice = 0;

	//����-���ٽ� ���� �� ����.
	hr = m_pDevice->CreateDepthStencilView(m_pDS, &dd, &m_pDSView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"����/���ٽǺ� ���� ����.", L"", MB_OK);
		return hr;
	}

	//���ҽ� �� ���� ��, ���ʿ��� DX �ڵ��� �����ؾ� �մϴ�.(�޸� ���� ����)
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
	// ��ġ ��� ���� Ȯ��
	GetFeatureLevel();

	// GPU ���� ���
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

	ver = ((feat & 0xf000) >> 12) - OFFSET;	//���� ���� ����.   	
	sub = ((feat & 0x0f00) >> 8);			//���� ���� ����.

	m_strFeatureLevel = strFeature[ver][sub];
}

HRESULT D3DEngine::GetAdapterInfo(DXGI_ADAPTER_DESC1* pAd)
{
	IDXGIAdapter1* pAdapter;
	IDXGIFactory1* pFactory = NULL;
	//DXGI_ADAPTER_DESC ad;

	//DXGIFactory ��ü ����. (DXGI.lib �ʿ�)
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory)))
	{
		return E_FAIL;
	}

	pFactory->EnumAdapters1(0, &pAdapter);		//��� ȹ��.
	pAdapter->GetDesc1(pAd);					//��� ���� ȹ��.
	//*pAd = ad;								//�ܺη� ����.

	//���� �����, ������ �������̽��� �����մϴ�. (�޸� ���� ����)
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

	return S_OK;
}

int D3DEngine::RenderStateObjectCreate()
{
	// ����/���ٽ� ���� ��ü ����
	DepthStencilStateCreate();

	// ������ ���� ��ü ����
	RasterStateCreate();

	// ���÷� ���� ��ü ����
	SamplerCreate();

	// ���� ���� ��ü ����
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

	// ������ ��� üũ : ����ڰ� ������ ������ ���� ����.
	CheckRMode(m_bCullBack, RM_CULLBACK);
	CheckRMode(m_bWireFrame, RM_WIREFRAME);

	// ������ ��� ��ȯ : ������ ����� ������ ���� ������ ���¸� ����.
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
	// ����/���ٽ� ���� ��ü ����.: "��º��ձ� Output Merger" ���� ����. 
	//----------------------------
	//...	 
	D3D11_DEPTH_STENCIL_DESC  ds;
	//���� ���� ���� (�⺻��)
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//���ٽ� ���� ���� (�⺻��) 
	ds.StencilEnable = FALSE;								//���ٽ� ���� OFF.
	ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;	//���ٽ� �б� ����ũ (8bit: 0xff)
	ds.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK; //���ٽ� ���� ����ũ (8bit: 0xff)
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	//[�ո�] ���ٽ� �� �Լ� : "Always" ��, �׻� ���� (���, pass)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	//[�ո�] ���ٽ� �� ������ ���� : ������ ����.
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	//[�ո�] ���ٽ� �� ���н� ���� : ������ ����.	
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	//[�ո�] ���ٽ�/���� �� ���н� ���� : ������ ����.
	ds.BackFace = ds.FrontFace;									//[�޸�] ���� ����. �ʿ�� ���� ������ ����.


	//----------------------------------------------------------------------
	// ���� ���� ���� ��ü��.
	//----------------------------------------------------------------------
	// DS ���� ��ü #0 : Z-Test ON! (�⺻��)
	ds.DepthEnable = TRUE;					//���� ���� On.
	ds.StencilEnable = FALSE;				//���ٽ� ���� Off!
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON]);

	// DS ���� ��ü #1 : Z-Test OFF ����.
	ds.DepthEnable = FALSE;
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF]);

	// DS ���� ��ü #2 : Z-Test On + Z-Write OFF.
	// Z-Test (ZEnable, DepthEnable) �� ������, Z-Write ���� ��Ȱ��ȭ �˴ϴ�.
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//���̰� ���� ��.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_WRITE_OFF]);


	//----------------------------------------------------------------------
	// ���ٽ� ���� ���� ��ü�� ����.
	//----------------------------------------------------------------------
	// ���ٽ� ���� ��Ʈ ���� ����.
	// (Stencil.Ref & Stencil.Mask) Comparison-Func ( StencilBuffer.Value & Stencil.Mask)
	//
	// *StencilBufferValue : ���� �˻��� �ȼ��� ���ٽǰ�.
	// *ComFunc : �� �Լ�. ( > < >= <= ==  Always Never)
	//----------------------------------------------------------------------
	// DS ���°�ü #4 :  ���̹��� On, ���ٽǹ��� ON (�׻�, ������ ����) : "����/���ٽ� ���" 
	ds.DepthEnable = TRUE;										//���̹��� ON! (�⺻��)
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//���Լ� : "�׻� ���" (����)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//������ : ������(Stencil Reference Value) �� ��ü.
	//ds.FrontFace.StencilFailOp	  = D3D11_STENCIL_OP_KEEP;		//���н� : ����.(�⺻��, ����)
	//ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.(�⺻��, ����)
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_ON]);


	// DS ���°�ü #5 : ���̹��� On, ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���� ��ġ���� �׸���" 
	//ds.DepthEnable	= TRUE;										//���̹��� ON! (�⺻��)(����)
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//���Լ� : "�����Ѱ�?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//������ : ����.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_EQUAL_KEEP]);


	// DS ���°�ü #6 : ���̹��� On, ���ٽǹ��� ON (�ٸ���, ������ ����) : "���� ��ġ �̿ܿ� �׸���" 
	//ds.DepthEnable	= TRUE;										//���̹��� ON! (�⺻��)(����)
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON!
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	//���Լ� : "���� ������?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//������ : ����.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP]);


	/*// DS ���°�ü #7 : ���̹��� Off, ���ٽǹ��� ON (������ ����) : "���ٽǸ� ���"
	ds.DepthEnable	  = FALSE;										//���̹��� OFF!
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON!
	ds.FrontFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;		//���Լ� : "�׻� ���" (����)
	ds.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_REPLACE;		//������ : ������(Stencil Reference Value) �� ��ü.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF_STENCIL_ON]);
	*/

	/*// DS ���°�ü #8 : ���̹��� On, ���ٽǹ��� ON (���Ϻ�, ���� ����) : "���߱׸��� ������"
	//ds.DepthEnable	= TRUE;										//���̹��� ON! (�⺻��)(����)
	//ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//ds.DepthFunc		= D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON!
	ds.FrontFace.StencilFunc		= D3D11_COMPARISON_EQUAL;		//���Լ� : "�����Ѱ�?"
	ds.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_INCR;		//������ : ���� (+1)
	ds.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_EQUAL_INCR]);
	*/

	// DS ���°�ü #9 : ���̹��� On, ���ٽǹ��� ON (�׻�, ������ ����) : "���ٽǸ� ���" 
	ds.DepthEnable = TRUE;										//���̹��� ON! (�⺻��)
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//���̹��� ���� OFF.
	ds.DepthFunc = D3D11_COMPARISON_LESS;						//���̿��� ON. (�⺻��)
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//���Լ� : "�׻� ���" (����)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//������ : ������(Stencil Reference Value) �� ��ü.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_WRITE_OFF_STENCIL_ON]);


	//���̹��� Off (Write Off), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���߱׸��� ����.
	ds.DepthEnable = FALSE;										//�����׽�Ʈ Off!
	//ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//���̰� ���� Off.
	//ds.DepthFunc		= D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//���Լ� : "�����Ѱ�?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;		//������ : ���� (+1) 
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
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
	//[���°�ü 1] �⺻ ������ ���� ��ü.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		//�ﰢ�� ���� ä���.(�⺻��)
	rd.CullMode = D3D11_CULL_NONE;		//�ø� ����. (�⺻���� �ø� Back)		
	rd.FrontCounterClockwise = false;   //���� �⺻��...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;		//AA ����.
	rd.AntialiasedLineEnable = false;
	//�����Ͷ����� ���� ��ü ����.
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_SOLID]);


	//[���°�ü2] ���̾� ������ �׸���. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	//�����Ͷ����� ��ü ����.
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIREFRAME]);

	//[���°�ü3] �ĸ� �ø� On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_CULLBACK]);

	//[���°�ü4] ���̾� ������ + �ĸ��ø� On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLBACK]);

	//[���°�ü5] ���� �ø� On! "CW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_CULLFRONT]);

	//[���°�ü6] ���̾� ������ + �����ø� On! "CW" 
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
	// ������ �ɼ� ���� 	 
	//if (IsKeyUp(VK_SPACE))	m_bWireFrame ^= TRUE;
	//if (IsKeyUp(VK_F4))		m_bCullBack ^= TRUE;

	// ���� ����
	if (m_bWireFrame) m_ClearColor = XMFLOAT4(0, 0.125f, 0.3f, 1.0f);
	else			  m_ClearColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	// ������ ��� ��ȯ.	  
	RenderModeUpdate();

	//���� ���� ���� (�⺻��)
	m_pDXDC->OMSetDepthStencilState(m_DSState[DS_DEPTH_ON], 0);

	//���÷� ����.(�⺻����)
	m_pDXDC->PSSetSamplers(0, 1, &m_pSampler[SS_DEFAULT]);
}

void D3DEngine::SamplerCreate()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

	//�ؽ�ó ���� : ���� ���͸�
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = m_dwAF;
	//�Ӹ���	
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	sd.MipLODBias = 0;
	//���� �⺻�� ó��..
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 1;
	sd.BorderColor[1] = 1;
	sd.BorderColor[2] = 1;
	sd.BorderColor[3] = 1;

	//���÷� ��ü1 ����. (DX �⺻��)
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = m_pDevice->CreateSamplerState(&sd, &m_pSampler[SS_CLAMP]);

	//���÷� ��ü2 ����.
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
// ȥ�հ�ü1 : ����ȥ�� ����. "Blending OFF"
//------------------------------
//���� ���� ��ü ���� �ɼ� : ���� �⺻����.
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.RenderTarget[0].BlendEnable = FALSE;							//���� ���� ����. �⺻���� FALSE(OFF)		
	//���� ���� ȥ�� : Color Blending.(�⺻��)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ����(Color - Operation), �⺻���� ���� : ������ = Src.Color + Dest.Color 	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//�ҽ�(����) ȥ�� ����, ���� 100% : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;				//Ÿ��(���) ȥ�� ����, ��� 0%   : Dest.Color = Dest * 0;  ��� 0�� RT �� "�����"�� �ǹ��մϴ�.	
	//���� ���� ȥ�� : Alpha Blending.(�⺻��)
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;			//���� ȥ�� �Լ�(Alpha - Opertion), �⺻���� ����.
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;				//�ҽ�(����) ���� ȥ�� ����.
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;			//Ÿ��(���) ���� ȥ�� ����.
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//����Ÿ�ٿ� ���� �ɼ�.
	//bd.AlphaToCoverageEnable = FALSE;								//���� �߰� �ɼ�.(�⺻��, ����)
	//bd.IndependentBlendEnable = FALSE;
	m_pDevice->CreateBlendState(&bd, &m_BState[BS_DEFAULT]);		//���� ��ü.����.


	//------------------------------
	// ȥ�հ�ü2 : ���� ȥ�� "Alpha Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� (����, �⺻��) : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//�ҽ�(����) ȥ�� ����, ���� ���ĺ���  : Src.Color = Src * Src.a;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		//Ÿ��(���) ȥ�� ����, ���� ���ĺ��� ���� : Dest.Color = Dest * (1-Src.a);			 		
	m_pDevice->CreateBlendState(&bd, &m_BState[BS_ALPHA_BLEND]);


	//------------------------------
	// ȥ�հ�ü3 : ���� ȥ�� "Color Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� (����, �⺻��) : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;			//�ҽ�(����) ȥ�� ����, ���� ����  : Src.Color = Src * Src.Color;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;		//Ÿ��(���) ȥ�� ����, ���� ���� ���� : Dest.Color = Dest * (1-Src.Color);			 	
	m_pDevice->CreateBlendState(&bd, &m_BState[BS_COLOR_BLEND]);


	//------------------------------
	// ȥ�հ�ü3 : ���� ȥ�� "��� ���� Blending 1:1" 
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� "��Ⱝ��" : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//�ҽ�(����) ȥ�� ����, ���� ����  : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;					//Ÿ��(���) ȥ�� ����, ���� ���� ���� : Dest.Color = Dest * 1;			 	
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

	// ���̴� ����
	m_DefaultShader->ShaderLoad(L"./fx/Default.fx");
	m_LineShader->ShaderLoad(L"./fx/Line.fx");
	m_NonTexShader->ShaderLoad(L"./fx/NonTex.fx");
	m_PixelLightShader->ShaderLoad(L"./fx/PixelLight.fx");
	m_TexShader->ShaderLoad(L"./fx/Tex.fx");
	m_TexLambertShader->ShaderLoad(L"./fx/TexLambert.fx");
	m_VertexLightShader->ShaderLoad(L"./fx/VertexLight.fx");

	CreateShaderLayout();

	// �⺻ ������� ����.(����,����)
	ZeroMemory(&m_cbDef, sizeof(cbDEFAULT));
	CreateDynamicConstantBuffer(sizeof(cbDEFAULT), &m_cbDef, &m_pCBDef);
	//���̴� ������� ����.(��������)
	//UpdateDynamicConstantBuffer(g_pDXDC, g_pCB, &g_CBuffer, sizeof(ConstBuffer));

	// ���� ������� ����.
	ZeroMemory(&m_cbLit, sizeof(cbLIGHT));
	CreateDynamicConstantBuffer(sizeof(cbLIGHT), &m_cbLit, &m_pCBLit);

	// ���� ������� ����.
	ZeroMemory(&m_cbMtrl, sizeof(cbMATERIAL));
	CreateDynamicConstantBuffer(sizeof(cbMATERIAL), &m_cbMtrl, &m_pCBMtrl);

	//�⺻ ���̴� ����.
	m_pDXDC->VSSetShader(m_TexLambertShader->m_pVS, nullptr, 0);
	m_pDXDC->PSSetShader(m_TexLambertShader->m_pPS, nullptr, 0);

	//�⺻ ���̴� ������� ����.
	m_pDXDC->VSSetConstantBuffers(0, 1, &m_pCBDef);
	///m_pDXDC->VSSetConstantBuffers(1, 1, &m_pCBLit);
	///m_pDXDC->VSSetConstantBuffers(2, 1, &m_pCBMtrl);
}

void D3DEngine::CreateShaderLayout()
{
	// �⺻ ���� �Է±��� ����
	/// �𵨸��� �־�� ������..? �ƴϸ� �������� �������� ������ �־.
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
	// ���� ��� ����
	m_cbDef.mWV = m_cbDef.mTM * m_cbDef.mView;
	m_cbDef.mWVP = m_cbDef.mTM * m_cbDef.mView * m_cbDef.mProj;

	//���̴� ��� ���� ����
	UpdateDynamicConstantBuffer(m_pCBDef, &m_cbDef, sizeof(cbDEFAULT));

	//�⺻ ���̴� ������� ����.
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

	//�Ӹ�+�ؽ�ó �ε�.
	hr = DirectX::CreateWICTextureFromFileEx(m_pDevice, m_pDXDC, filename, 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT,
		nullptr, &pTexRV);

	if (FAILED(hr))
	{
		hr = DirectX::CreateDDSTextureFromFile(m_pDevice, filename, nullptr, &pTexRV);
		if (FAILED(hr))
		{
			PrintError(TRUE, L"�ؽ�ó �ε� ���� \nFile=%s", filename);
			return hr;
		}
	}

	//�ܺη� ����.
	*ppTexRV = pTexRV;

	return hr;
}

HRESULT D3DEngine::GetTextureDesc(ID3D11ShaderResourceView* pRV, D3D11_TEXTURE2D_DESC* pDesc)
{
	HRESULT res = S_OK;

	//���̴� ���ҽ� �� ���� ȹ��.
	D3D11_SHADER_RESOURCE_VIEW_DESC dc;
	pRV->GetDesc(&dc);

	//�ؽ�ó ���� ȹ��.
	ID3D11Resource* pRes = nullptr;
	pRV->GetResource(&pRes);
	ID3D11Texture2D* pTex = nullptr;
	pRes->QueryInterface<ID3D11Texture2D>(&pTex);
	if (pTex)
	{
		pTex->GetDesc(pDesc);

		//���� ȹ���� �������̽� ����.
		pTex->Release();
	}

	//���� ȹ���� �������̽� ����.
	//SAFE_RELEASE(pRes);

	return res;
}


HRESULT D3DEngine::CreateVB(void* pBuff, UINT size, LPVERTEXBUFFER* ppVB)
{
	LPVERTEXBUFFER pVB = nullptr;

	//���� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//���� �����
	bd.ByteWidth = size;							//���� ũ��
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//���� �뵵 : "���� ����" ��� ���� 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;								//���ۿ� �� ������ ���� : "������"..

	//���� ���� ����.
	HRESULT hr = m_pDevice->CreateBuffer(&bd, &rd, &pVB);
	if (FAILED(hr))
	{
		//���� ���� �� ����ó��..
		//...
		PrintError(hr, L"CreateVB : �������� ���� ����");
		return hr;
	}

	//�ܺη� ����.
	*ppVB = pVB;

	return hr;
}

HRESULT D3DEngine::CreateIB(void* pBuff, UINT size, LPINDEXBUFFER* ppIB)
{
	LPINDEXBUFFER pIB = nullptr;

	// �ε��� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//���� �����
	bd.ByteWidth = size;							//���� ũ��
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;			//���� �뵵 : "���� ����" ��� ���� 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;								//���ۿ� �� ������ ���� : "���� ����"..

	//���� ���� ����.
	HRESULT hr = m_pDevice->CreateBuffer(&bd, &rd, &pIB);
	if (FAILED(hr))
	{
		//���� ���� �� ����ó��..
		//...
		///PrintError(hr, L"CreateIB : �ε������� ���� ����");
		return hr;
	}

	//�ܺη� ����.
	*ppIB = pIB;

	return hr;
}

HRESULT D3DEngine::CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;


	//��� ���� ������, ũ�� �׽�Ʈ.
	//������۴� 16����Ʈ ���ĵ� ũ��� �����ؾ� �մϴ�.
	if (size % 16)
	{
		// 16����Ʈ ���Ľ���... �޼��� ���..
		PrintError(TRUE, L"[���] ������� : 16����Ʈ ������. ���ۻ����� �����մϴ�. \n"
			L"CB = 0x%X \n"
			L"CB.Size = %d bytes",
			pData, size);
	}
	else
	{
		//16����Ʈ ���� OK.�޼��� ����...
		///PrintError(TRUE, L"[�˸�] ������� : 16����Ʈ ���ĵ�. \n"
		///	L"CB = 0x%X \n"
		///	L"CB.Size = %d bytes",
		///	pData, size);
	}


	//��� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;				//���� �������� ����.
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//CPU ���� ����.

	//���긮�ҽ� ����.
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pData;				//(�ܺ�) ��� ������ ����.
	//sd.SysMemPitch = 0;
	//sd.SysMemSlicePitch = 0;

	//��� ���� ����.
	hr = m_pDevice->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
	{
		PrintError(hr, L"���� ������� ���� ����");
		return hr;
	}

	//�ܺη� ����.
	*ppCB = pCB;

	return hr;
}


HRESULT D3DEngine::UpdateDynamicConstantBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	//������� ����
	hr = m_pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	if (FAILED(hr))
	{
		PrintError(hr, L"UpdateDynamicConstantBuffer : Map ����");
		return hr;
	}

	//��� ���� ����.
	memcpy(mr.pData, pData, size);

	//������� �ݱ�.
	m_pDXDC->Unmap(pBuff, 0);


	return hr;
}

int D3DEngine::PrintError(BOOL bMBox, const TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, const TCHAR* filename, const char* EntryPoint, const char* ShaderModel)
{
	//�Ķ����, �����ڵ�� ��ȯ.
	TCHAR func[80] = L"";
	::mbstowcs(func, EntryPoint, strlen(EntryPoint));
	TCHAR sm[20] = L"";
	::mbstowcs(sm, ShaderModel, strlen(ShaderModel));


	//���̴� ���� �޼��� �б�.
	TCHAR errw[4096] = L"";
	::mbstowcs(errw, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());


	//HRESULT ���� ���� �޼��� ��� 
	//�ý������� ���� ��� ���̴� �����޼����� ����Ȯ�ϹǷ� ����.
	TCHAR herr[1024] = L"�Ʒ��� ������ Ȯ���Ͻʽÿ�.";
	/*FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		herr, 1024, NULL);
	*/


	//���� �޼��� ���̱�.
	TCHAR errmsg[1024];
	_stprintf(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n�����ڵ�(0x%08X) : %s \n\n%s",
		msg, filename, func, sm,
		hr, herr, errw);


	//(����� ��) VS ���â���� ���..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);
	//OutputDebugString(errw);


	//�α����Ϸ� ���.
	//...


	//�޼��� â ���..
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


	//(����� ��) VS ���â���� ���..
	OutputDebugString(L"\n");
	OutputDebugString(msgva);

	//�α����Ϸ� ���.
	//...

	//�޼��� â ���..
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

