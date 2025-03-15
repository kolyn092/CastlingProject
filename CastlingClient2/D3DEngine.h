#pragma once
#define _WINSOCK2API_
#include <Windows.h>
#include "DXEngineDefine.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"

///
///
///  DX11 ���� Ŭ����
///
///  [8/11/2020 Kolyn]

// ����/���ٽ� �׽�Ʈ ���� ��ü
enum {
	DS_DEPTH_ON,		//���̹��� ON! (�⺻��), ���ٽǹ��� OFF.
	DS_DEPTH_OFF,		//���̹��� OFF!, ���ٽ� OFF.
	DS_DEPTH_WRITE_OFF,	//���̹��� ���� ����, ���ٽ� OFF. 

	//���ٽ� ���� �����.
	DS_DEPTH_ON_STENCIL_ON,				//���̹��� On (Write On), ���ٽǹ��� ON (������ ����) : "����/���ٽ� ���".
	DS_DEPTH_ON_STENCIL_EQUAL_KEEP,		//���̹��� On (Write On), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���� ��ġ���� �׸���".
	DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP,	//���̹��� On (Write On), ���ٽǹ��� ON (�ٸ���, ������ ����) : "���� ��ġ �̿ܿ� �׸���".

	//DS_DEPTH_OFF_STENCIL_ON,			//���̹��� Off, ���ٽǹ��� ON (������ ����) : "���ٽǸ� ���"
	//DS_DEPTH_ON_STENCIL_EQUAL_INCR,	//���̹��� On (Write On), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���߱׸��� ����".
	DS_DEPTH_WRITE_OFF_STENCIL_ON,		//���̹��� On (Write Off), ���ٽǹ��� ON (�׻���, ������ ����) :" ���ٽǸ� ���".
	DS_DEPTH_OFF_STENCIL_EQUAL_INCR,	//���̹��� Off (Write Off), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���߱׸��� ����.


	//���� ��� �ִ밪.
	DS_MAX_,

	//��� ������. MAX �� ���ʿ� �����ؾ� ��. ����.
	DS_DEPTH_ON_STENCIL_OFF = DS_DEPTH_ON,			//���̹��� ON, ���ٽǹ��� OFF
	DS_DEPTH_ONLY = DS_DEPTH_ON_STENCIL_OFF,		//���̹��� ON, ���ٽǹ��� OFF
	//DS_STENCIL_ONLY = DS_DEPTH_OFF_STENCIL_ON,	//���̹��� OFF, ���ٽǹ��� ON
};

// �����Ͷ����� ���� ��ü
enum {
	RS_SOLID,				//�ﰢ�� ä��� : Fill Mode - Soild.
	RS_WIREFRAME,				//�ﰢ�� ä��� : Fill Mode - Wireframe.
	RS_CULLBACK,			//�޸� �ø� (ON) : Back-Face Culling - "CCW" 
	RS_CULLFRONT,			//���� �ø� (ON) : Front-Face Culling - "CW" 	
	RS_WIRECULLBACK,		//���̾� ������ + �޸� �ø� (ON) 
	RS_WIRECULLFRONT,		//���̾� ������ + �ո� �ø� (ON) 	

	//���� ��� �ִ밪.
	RS_MAX_,

	//��� ������. MAX �� ���ʿ� �����ؾ� ��. ����.
	RS_CULL_CCW = RS_CULLBACK,
	RS_CULL_CW = RS_CULLFRONT,
	RS_WIRE_CULL_CCW = RS_WIRECULLBACK,
	RS_WIRE_CULL_CW = RS_WIRECULLFRONT,

};

// �ؽ�ó ���÷� ���� ��ü
enum {
	SS_CLAMP,			//�ؽ�ó ��巹�� ��� : Ŭ���� (DX �⺻��) 
	SS_WRAP,			//�ؽ�ó ��巹�� ��� : ���� 
	//SS_BOARDER,
	//SS_MIRROR,
	//SS_MIRRROONCE,

	SS_MAX,
	SS_DEFAULT = SS_WRAP,	//�⺻ ���÷�

};

// ����/���� ȥ�� ���� ��ü
enum {
	BS_DEFAULT,				//�⺻ȥ�� ��ü.
	BS_ALPHA_BLEND,			//���� ����. (Src.a + (1-Src.a))
	BS_COLOR_BLEND,			//���� ����. ( Src.Color + (1-Src.Color))
	BS_COLOR_BLEND_ONE,		//1:1 ����. ( Src.Color + Dest.Color) 

	BS_MAX_,

	BS_AB_OFF = BS_DEFAULT,
	BS_AB_ON = BS_ALPHA_BLEND,
	//BS_AB_ALPHA = BS_ALPHA_BLEND,
	//BS_AB_COLOR = BS_COLOR_BLEND,
};

enum {
	RM_SOLID = 0x0000,		// �ﰢ��ä��� : ON, Solid
	RM_WIREFRAME = 0x0001,		// �ﰢ��ä��� : OFF, Wire-frame
	RM_CULLBACK = 0x0002,		// �޸� �ø� : ON, "CCW"

	//������ �⺻��� : Solid + Cull-On.
	RM_DEFAULT = RM_SOLID | RM_CULLBACK,

};

_declspec(align(16)) struct cbDEFAULT
{
	XMMATRIX mTM;		//"World" ��ȯ ��� : DirectXMath, 16����Ʈ ���� ����. 
	XMMATRIX mView;		//"View" ��ȯ ���
	XMMATRIX mProj;		//"Projection" ��ȯ ���
	XMMATRIX mWV;		// World+view ȥ�����. 
	XMMATRIX mWVP;		// World+view+Proj ȥ�����. 
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
	COLOR    Diffuse;		//�� ����(Ȯ�걤) �� �ݻ���(%) (�Ϲ�+������)
	COLOR    Ambient;		//���� ����(�ֺ���) �� �ݻ���(%) (�Ϲ�+������)
	COLOR    Specular;		//���ݻ籤 �ݻ���(%)
	float	 Power;			//���ݻ� ������ : "��ĥ��"
};

class D3DEngine : public AlignedAllocationPolicy<16>
{

private:
	// D3D ���� ��ü �������̽�
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDXDC;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRTView;

	// ����, ���ٽ� ����
	ID3D11Texture2D* m_pDS;
	ID3D11DepthStencilView* m_pDSView;

private:
	// ��ġ ���� �⺻ ����
	DXGI_MODE_DESC m_Mode;
	// ���� ������ ���
	DWORD m_RMode;

	// AA & AF Option
	DWORD m_dwAA;
	DWORD m_dwAF;
	BOOL m_bMipMap;

	// D3D ��� ����
	D3D_FEATURE_LEVEL m_FeatureLevels;
	const TCHAR* m_strFeatureLevel;

	// ��ġ ����
	DXGI_ADAPTER_DESC1 m_Adc;

private:
	// ��üȭ�� ��뿩��
	BOOL m_bWindowMode;
	// ��������ȭ ��뿩��
	BOOL m_bVSync;
	// �޸� ����
	BOOL m_bCullBack;
	// ���̾� ������
	BOOL m_bWireFrame;
	// ���� ���� ����
	BOOL m_bZEnable;
	// ���� ���� ����
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

	// �⺻ ������� ���ſ�
	cbDEFAULT		m_cbDef;
	cbLIGHT			m_cbLit;
	cbMATERIAL		m_cbMtrl;

	//���̴� ��� ����.
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

