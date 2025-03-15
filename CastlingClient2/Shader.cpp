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

	// 정점 셰이더 생성.
	ShaderLoad(fxname, "VS_Main", "vs_5_0", &m_pVS, &m_pVSCode);

	// 픽셀 셰이더 생성.
	ShaderLoad(fxname, "PS_Main", "ps_5_0", &m_pPS);

	return hr;
}

HRESULT Shader::ShaderLoad(const TCHAR* fxname, ID3D11VertexShader** ppVS, ID3D11PixelShader** ppPS, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;

	// 정점 셰이더 생성.
	ShaderLoad(fxname, "VS_Main", "vs_5_0", ppVS, ppCode);

	// 픽셀 셰이더 생성.
	ShaderLoad(fxname, "PS_Main", "ps_5_0", ppPS);

	return hr;
}

HRESULT Shader::ShaderLoad(const TCHAR* fxname, const char* entry, const char* target, ID3D11VertexShader** ppVS, ID3DBlob** ppCode/*=NULL*/)
{
	HRESULT hr = S_OK;

	// 정점 셰이더 컴파일 Compile a VertexShader
	ID3D11VertexShader* pVS = nullptr;
	ID3DBlob* pVSCode = nullptr;
	hr = ShaderCompile(fxname, entry, target, &pVSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"[실패] ShaderLoad :: Vertex Shader 컴파일 실패", L"Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	// 정점 셰이더 객체 생성 Create a VS Object 
	hr = D3DEngine::GetIns()->GetDevice()->CreateVertexShader(pVSCode->GetBufferPointer(),
		pVSCode->GetBufferSize(),
		nullptr,
		&pVS
	);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSCode);			//임시 개체 제거.
		return hr;
	}

	//--------------------------
	// 생성된 객체 외부 리턴
	//--------------------------
	*ppVS = pVS;
	if (ppCode) *ppCode = pVSCode;
	return hr;
}

HRESULT Shader::ShaderLoad(const TCHAR* fxname, const char* entry, const char* target, ID3D11PixelShader** ppPS)
{
	HRESULT hr = S_OK;

	// 픽셀 셰이더 컴파일 Compile a PixelShader
	ID3D11PixelShader* pPS = nullptr;
	ID3DBlob* pPSCode = nullptr;
	hr = ShaderCompile(fxname, entry, target, &pPSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"[실패] ShaderLoad :: Pixel Shader 컴파일 실패", L"Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	// 픽셀 셰이더 객체 생성 Create a PS Object 
	hr = D3DEngine::GetIns()->GetDevice()->CreatePixelShader(pPSCode->GetBufferPointer(),
		pPSCode->GetBufferSize(),
		nullptr,
		&pPS
	);

	SAFE_RELEASE(pPSCode);				//임시 개체 제거.	
	if (FAILED(hr))	return hr;


	//--------------------------
	// 생성된 객체 외부 리턴
	//--------------------------
	*ppPS = pPS;

	return hr;
}

HRESULT Shader::ShaderCompile(const TCHAR* FileName, const char* EntryPoint, const char* ShaderModel, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	//컴파일 옵션1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//열우선 행렬 처리. 구형 DX9 이전까지의 전통적인 방식. 속도가 요구된다면, "행우선" 으로 처리할 것.
	//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//행우선 행렬 처리. 속도의 향상이 있지만, 행렬을 전치 후 GPU 에 공급해야 합니다.
	//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//디버깅 모드시 옵션 추가.
#endif

	//셰이더 소스 컴파일.
	hr = D3DCompileFromFile(FileName,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, //외부 파일 Include 허용. 
		EntryPoint,
		ShaderModel,
		Flags,				//컴파일 옵션1
		0,					//컴파일 옵션2,  Effect 파일 컴파일시 적용됨. 이외에는 무시됨.
		ppCode,				//[출력] 컴파일된 셰이더 코드.
		&pError				//[출력] 컴파일 에러 코드.
	);
	if (FAILED(hr))
	{
		//컴파일 에러확인을 위해 pError 를 출력합니다.
		D3DEngine::GetIns()->PrintError(TRUE, L"셰이더 컴파일 실패", hr, pError, FileName, EntryPoint, ShaderModel);
	}

	SAFE_RELEASE(pError);
	return hr;
}

int Shader::CreateVBLayout(D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutSize)
{
	HRESULT hr = S_OK;

	// 정점 입력구조 Input layout
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

	// 정접 입력구조 객체 생성 Create the input layout
	hr = D3DEngine::GetIns()->GetDevice()->CreateInputLayout(layout,
		numElements,
		m_pVSCode->GetBufferPointer(),
		m_pVSCode->GetBufferSize(),
		&m_pVBLayout
	);
	if (FAILED(hr))	return hr;

	return hr;
}
