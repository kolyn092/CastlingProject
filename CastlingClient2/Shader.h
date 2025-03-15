#pragma once
#include "DXEngineDefine.h"


class Shader
{

public:
	ID3D11VertexShader* m_pVS;			//���� ���̴� Vertex Shader �������̽�.
	ID3D11PixelShader* m_pPS;			//�ȼ� ���̴� Pixel Shader �������̽�.

	ID3DBlob* m_pVSCode;				//���� ���̴� ������ �ڵ� ��ü.(�ӽ�)

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

