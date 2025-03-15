#pragma once
#include "DXEngineDefine.h"
#include "Shader.h"

class CSprite
{
	struct _VTX
	{
		Vector3 pos;
		Vector2 tex;
	};

	ID3D11Buffer* m_pSprVB;

	Shader* m_pSprShader;

	ID3D11ShaderResourceView*	m_pSprTexRV;
	D3D11_TEXTURE2D_DESC		m_TexDesc;
	TCHAR						m_TexName[256];


public:
	DWORD						m_Width;
	DWORD						m_Height;
	Color						m_Color;

public:
	CSprite();
	~CSprite();

public:
	int SpriteShaderLoad();
	int SpriteLayoutCreate();

	int SpriteLoad(const TCHAR* fileName, COLOR TransColor);
	void SpriteDraw(Vector3 vPos, Vector3 vRot);
};

