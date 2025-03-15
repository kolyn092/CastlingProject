#pragma once
#include "DXEngineDefine.h"


class Shader
{

public:
	ID3D11VertexShader* m_pVS;			//정점 셰이더 Vertex Shader 인터페이스.
	ID3D11PixelShader* m_pPS;			//픽셀 셰이더 Pixel Shader 인터페이스.

	ID3DBlob* m_pVSCode;				//정점 셰이더 컴파일 코드 개체.(임시)

	ID3D11InputLayout* m_pVBLayout;

public:
	Shader();
	~Shader();

public:
	HRESULT ShaderLoad(const TCHAR* fxname);
	HRESULT ShaderLoad(const TCHAR* fxname, ID3D11VertexShader** ppVS, ID3D11PixelShader** ppPS, ID3DBlob** ppCode);
	HRESULT ShaderLoad(const TCHAR* fxname, const char* entry, const char* target, ID3D11VertexShader** ppVS, ID3DBlob** ppCode/*=NULL*/);
	HRESULT ShaderLoad(const TCHAR* fxname, const char* entry, const char* target, ID3D11PixelShader** ppPS);

private:
	HRESULT ShaderCompile(const TCHAR* FileName, const char* EntryPoint, const char* ShaderModel, ID3DBlob** ppCode);

public:
	int CreateVBLayout(D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutSize);
};

