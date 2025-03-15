#include "Shader.h"
#include "D3DEngine.h"

Shader::Shader() :m_pVS(nullptr), m_pPS(nullptr), m_pVSCode(nullptr), m_pVBLayout(nullptr)
{

}

Shader::~Shader()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pVSCode);
	SAFE_RELEASE(m_pVBLayout);
}


HRESULT Shader::ShaderLoad(const TCHAR* fxname)
{
	HRESULT hr = S_OK;

	// ���� ���̴� ����.
	ShaderLoad(fxname, "VS_Main", "vs_5_0", &m_pVS, &m_pVSCode);

	// �ȼ� ���̴� ����.
	ShaderLoad(fxname, "PS_Main", "ps_5_0", &m_pPS);

	return hr;
}

HRESULT Shader::ShaderLoad(const TCHAR* fxname, ID3D11VertexShader** ppVS, ID3D11PixelShader** ppPS, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;

	// ���� ���̴� ����.
	ShaderLoad(fxname, "VS_Main", "vs_5_0", ppVS, ppCode);

	// �ȼ� ���̴� ����.
	ShaderLoad(fxname, "PS_Main", "ps_5_0", ppPS);

	return hr;
}

HRESULT Shader::ShaderLoad(const TCHAR* fxname, const char* entry, const char* target, ID3D11VertexShader** ppVS, ID3DBlob** ppCode/*=NULL*/)
{
	HRESULT hr = S_OK;

	// ���� ���̴� ������ Compile a VertexShader
	ID3D11VertexShader* pVS = nullptr;
	ID3DBlob* pVSCode = nullptr;
	hr = ShaderCompile(fxname, entry, target, &pVSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"[����] ShaderLoad :: Vertex Shader ������ ����", L"Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	// ���� ���̴� ��ü ���� Create a VS Object 
	hr = D3DEngine::GetIns()->GetDevice()->CreateVertexShader(pVSCode->GetBufferPointer(),
		pVSCode->GetBufferSize(),
		nullptr,
		&pVS
	);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSCode);			//�ӽ� ��ü ����.
		return hr;
	}

	//--------------------------
	// ������ ��ü �ܺ� ����
	//--------------------------
	*ppVS = pVS;
	if (ppCode) *ppCode = pVSCode;
	return hr;
}

HRESULT Shader::ShaderLoad(const TCHAR* fxname, const char* entry, const char* target, ID3D11PixelShader** ppPS)
{
	HRESULT hr = S_OK;

	// �ȼ� ���̴� ������ Compile a PixelShader
	ID3D11PixelShader* pPS = nullptr;
	ID3DBlob* pPSCode = nullptr;
	hr = ShaderCompile(fxname, entry, target, &pPSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"[����] ShaderLoad :: Pixel Shader ������ ����", L"Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	// �ȼ� ���̴� ��ü ���� Create a PS Object 
	hr = D3DEngine::GetIns()->GetDevice()->CreatePixelShader(pPSCode->GetBufferPointer(),
		pPSCode->GetBufferSize(),
		nullptr,
		&pPS
	);

	SAFE_RELEASE(pPSCode);				//�ӽ� ��ü ����.	
	if (FAILED(hr))	return hr;


	//--------------------------
	// ������ ��ü �ܺ� ����
	//--------------------------
	*ppPS = pPS;

	return hr;
}

HRESULT Shader::ShaderCompile(const TCHAR* FileName, const char* EntryPoint, const char* ShaderModel, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	//������ �ɼ�1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//���켱 ��� ó��. ���� DX9 ���������� �������� ���. �ӵ��� �䱸�ȴٸ�, "��켱" ���� ó���� ��.
	//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//��켱 ��� ó��. �ӵ��� ����� ������, ����� ��ġ �� GPU �� �����ؾ� �մϴ�.
	//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//����� ���� �ɼ� �߰�.
#endif

	//���̴� �ҽ� ������.
	hr = D3DCompileFromFile(FileName,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, //�ܺ� ���� Include ���. 
		EntryPoint,
		ShaderModel,
		Flags,				//������ �ɼ�1
		0,					//������ �ɼ�2,  Effect ���� �����Ͻ� �����. �̿ܿ��� ���õ�.
		ppCode,				//[���] �����ϵ� ���̴� �ڵ�.
		&pError				//[���] ������ ���� �ڵ�.
	);
	if (FAILED(hr))
	{
		//������ ����Ȯ���� ���� pError �� ����մϴ�.
		D3DEngine::GetIns()->PrintError(TRUE, L"���̴� ������ ����", hr, pError, FileName, EntryPoint, ShaderModel);
	}

	SAFE_RELEASE(pError);
	return hr;
}

int Shader::CreateVBLayout(D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutSize)
{
	HRESULT hr = S_OK;

	// ���� �Է±��� Input layout
	/*
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		  0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	*/

	//UINT numElements = ARRAYSIZE(layout);
	UINT numElements = layoutSize;

	// ���� �Է±��� ��ü ���� Create the input layout
	hr = D3DEngine::GetIns()->GetDevice()->CreateInputLayout(layout,
		numElements,
		m_pVSCode->GetBufferPointer(),
		m_pVSCode->GetBufferSize(),
		&m_pVBLayout
	);
	if (FAILED(hr))	return hr;

	return hr;
}
