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

	// 스프라이트 (공용) 정점 버퍼 생성 : 초기 1번만 생성
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

	//스프라이트 (공용) 셰이더 생성
	SpriteShaderLoad();

	//스프라이트 (공용) 정점입력구조 생성
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

	// 2D Screen 좌표 보정
	vPos.x -= 0.5f;	vPos.y -= 0.5f;

	// 스프라이트 변환
	float sizex = (float)m_Width;								// 원본 스프라이트 크기(픽셀)
	float sizey = (float)m_Height;
	XMMATRIX mScale = XMMatrixScaling(sizex, sizey, 1.0f);
	XMMATRIX mTrans = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);	// 위치 설정 : 2D Screen 좌표

	XMMATRIX mRot = XMMatrixRotationY(vRot.y);
	XMMATRIX mTM = mScale * mRot * mTrans;

	//--------------------------------------
	// 외부 지정 렌더링 옵션 및 상수 버퍼 갱신.
	//--------------------------------------
	// 장면내의 유닛/모델별 렌더링 옵션이 상이할 수 있기에 개별 처리가 필요합니다.
	// 모델 각 객체들은 외부의 상수 버퍼를 공유중이므로 그리기 전에 갱신해야 합니다. 
	// 만일 유닛별 상수버퍼가 준비되어 있다면 이런 동작은 불필요 합니다. 
	cbDEFAULT cbDef = _d3d->m_cbDef;
	cbDef.mTM = mTM;
	_d3d->UpdateDynamicConstantBuffer(_d3d->m_pCBDef, &cbDef, sizeof(cbDEFAULT));

	//재질 정보 상수버퍼 갱신 : 외부 추가색 및 Alpha를 지정
	cbMATERIAL cbMtrl = _d3d->m_cbMtrl;
	cbMtrl.Diffuse = COLOR(m_Color.x, m_Color.y, m_Color.z, m_Color.w);
	cbMtrl.Ambient = COLOR(1, 1, 1, 1);
	cbMtrl.Specular = COLOR(1, 1, 1, 1);
	cbMtrl.Power = 30.0f;
	_d3d->UpdateDynamicConstantBuffer(_d3d->m_pCBMtrl, &cbMtrl, sizeof(cbMATERIAL));

	// 스프라이트 메쉬 설정. (공용)
	UINT stride = sizeof(_VTX);
	UINT offset = 0;
	ID3D11DeviceContext* _dc = _d3d->GetDXDC();

	_dc->IASetVertexBuffers(0, 1, &m_pSprVB, &stride, &offset);		
	_dc->IASetInputLayout(m_pSprShader->m_pVBLayout);
	_dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* pSampler = D3DEngine::GetIns()->GetSampler(SS_CLAMP);
	_dc->PSSetSamplers(0, 1, &pSampler);

	//셰이더 설정. : 표준 조명 셰이더 사용. 
	_dc->VSSetShader(m_pSprShader->m_pVS, nullptr, 0);
	_dc->PSSetShader(m_pSprShader->m_pPS, nullptr, 0);

	//상수버퍼 설정 
	_dc->VSSetConstantBuffers(0, 1, &_d3d->m_pCBDef);			//상수 버퍼 설정.(VS)
	_dc->VSSetConstantBuffers(2, 1, &_d3d->m_pCBMtrl);			//재질 정보 설정.(VS)
	//셰이더 리소스 설정.
	_dc->PSSetShaderResources(0, 1, &m_pSprTexRV);				//PS 에 텍스처 설정

	// 그리기
	_dc->Draw(4, 0);

}