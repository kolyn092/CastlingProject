#pragma once
#include "Shader.h"

class SkyBox : public AlignedAllocationPolicy<16>
{
public:
	struct cbDEFAULT_SkyBox
	{
		XMMATRIX mViewInv;	
		XMMATRIX mProjInv;	
	};

	struct cbExtraData_SkyBox
	{
		XMVECTOR diffuse;
		float z_far;
		float padding[3];
	};

	struct VTX_SkyBox
	{
		XMFLOAT2 pos;
	};

private:
	ID3D11Buffer* m_pSkyBoxVB;
	Shader* m_Shader;
	ID3D11ShaderResourceView* m_TextureRV;
	cbDEFAULT_SkyBox cbDef_SkyBox = {};
	cbExtraData_SkyBox cbExt_SkyBox = {};
	ID3D11Buffer* m_pCB_SkyBox;
	ID3D11Buffer* m_pExtCB_SkyBox;

public:
	SkyBox();
	~SkyBox();

public:
	void Update();
	void Draw();
};

