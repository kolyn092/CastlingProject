#include "CSprite.h"
#include "D3DEngine.h"

CSprite::CSprite()
{
	_VTX	vtxs[] =
	{
		{ VECTOR3(0, 0, 0),  VECTOR2(0, 0) },
		{ VECTOR3(1, 0, 0),  VECTOR2(1, 0) },
		{ VECTOR3(0, 1, 0),  VECTOR2(0, 1) },
		{ VECTOR3(1, 1, 0),  VECTOR2(1, 1) },
	};

	// ��������Ʈ (����) ���� ���� ���� : �ʱ� 1���� ����
	if (m_pSprVB == nullptr)
	{
		D3DEngine::GetIns()->CreateVB(vtxs, sizeof(vtxs), &m_pSprVB);
	}

	m_pSprShader = new Shader();

	m_pSprTexRV = nullptr;
	m_Width = m_Height = 0;
	_tcscpy(m_TexName, L"N/A");
	ZeroMemory(&m_TexDesc, sizeof(D3D11_TEXTURE2D_DESC));

	m_Color = Color(1, 1, 1, 1);

	//��������Ʈ (����) ���̴� ����
	SpriteShaderLoad();

	//��������Ʈ (����) �����Է±��� ����
	SpriteLayoutCreate();
}

CSprite::~CSprite()
{
	SAFE_RELEASE(m_pSprTexRV);
	delete m_pSprShader;
}

int CSprite::SpriteShaderLoad()
{
	m_pSprShader->ShaderLoad(L"./fx/Sprite.fx");

	return TRUE;
}

int CSprite::SpriteLayoutCreate()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_pSprShader->CreateVBLayout(layout, ARRAYSIZE(layout));

	return TRUE;
}

int CSprite::SpriteLoad(const TCHAR* fileName, COLOR TransColor)
{
	D3DEngine::GetIns()->LoadTexture(fileName, &m_pSprTexRV);

	D3D11_TEXTURE2D_DESC td;
	D3DEngine::GetIns()->GetTextureDesc(m_pSprTexRV, &td);

	m_TexDesc = td;
	m_Width = td.Width;
	m_Height = td.Height;
	_tcscpy(m_TexName, fileName);

	return TRUE;
}

void CSprite::SpriteDraw(Vector3 vPos, Vector3 vRot)
{
	D3DEngine* _d3d = D3DEngine::GetIns();

	// 2D Screen ��ǥ ����
	vPos.x -= 0.5f;	vPos.y -= 0.5f;

	// ��������Ʈ ��ȯ
	float sizex = (float)m_Width;								// ���� ��������Ʈ ũ��(�ȼ�)
	float sizey = (float)m_Height;
	XMMATRIX mScale = XMMatrixScaling(sizex, sizey, 1.0f);
	XMMATRIX mTrans = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);	// ��ġ ���� : 2D Screen ��ǥ

	XMMATRIX mRot = XMMatrixRotationY(vRot.y);
	XMMATRIX mTM = mScale * mRot * mTrans;

	//--------------------------------------
	// �ܺ� ���� ������ �ɼ� �� ��� ���� ����.
	//--------------------------------------
	// ��鳻�� ����/�𵨺� ������ �ɼ��� ������ �� �ֱ⿡ ���� ó���� �ʿ��մϴ�.
	// �� �� ��ü���� �ܺ��� ��� ���۸� �������̹Ƿ� �׸��� ���� �����ؾ� �մϴ�. 
	// ���� ���ֺ� ������۰� �غ�Ǿ� �ִٸ� �̷� ������ ���ʿ� �մϴ�. 
	cbDEFAULT cbDef = _d3d->m_cbDef;
	cbDef.mTM = mTM;
	_d3d->UpdateDynamicConstantBuffer(_d3d->m_pCBDef, &cbDef, sizeof(cbDEFAULT));

	//���� ���� ������� ���� : �ܺ� �߰��� �� Alpha�� ����
	cbMATERIAL cbMtrl = _d3d->m_cbMtrl;
	cbMtrl.Diffuse = COLOR(m_Color.x, m_Color.y, m_Color.z, m_Color.w);
	cbMtrl.Ambient = COLOR(1, 1, 1, 1);
	cbMtrl.Specular = COLOR(1, 1, 1, 1);
	cbMtrl.Power = 30.0f;
	_d3d->UpdateDynamicConstantBuffer(_d3d->m_pCBMtrl, &cbMtrl, sizeof(cbMATERIAL));

	// ��������Ʈ �޽� ����. (����)
	UINT stride = sizeof(_VTX);
	UINT offset = 0;
	ID3D11DeviceContext* _dc = _d3d->GetDXDC();

	_dc->IASetVertexBuffers(0, 1, &m_pSprVB, &stride, &offset);		
	_dc->IASetInputLayout(m_pSprShader->m_pVBLayout);
	_dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* pSampler = D3DEngine::GetIns()->GetSampler(SS_CLAMP);
	_dc->PSSetSamplers(0, 1, &pSampler);

	//���̴� ����. : ǥ�� ���� ���̴� ���. 
	_dc->VSSetShader(m_pSprShader->m_pVS, nullptr, 0);
	_dc->PSSetShader(m_pSprShader->m_pPS, nullptr, 0);

	//������� ���� 
	_dc->VSSetConstantBuffers(0, 1, &_d3d->m_pCBDef);			//��� ���� ����.(VS)
	_dc->VSSetConstantBuffers(2, 1, &_d3d->m_pCBMtrl);			//���� ���� ����.(VS)
	//���̴� ���ҽ� ����.
	_dc->PSSetShaderResources(0, 1, &m_pSprTexRV);				//PS �� �ؽ�ó ����

	// �׸���
	_dc->Draw(4, 0);

}