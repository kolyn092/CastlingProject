#include "UI.h"
#include "D3DEngine.h"


UI::UI()
{
	m_Sprite = new CSprite();
	m_vPos = Vector3(0, 0, 0);
	m_vRot = Vector3(0, 0, 0);
	m_isShow = true;
	m_isLocked = false;
	m_UIName = nullptr;
}

UI::UI(Vector3 vPos, Vector3 vRot, bool isShow, const TCHAR* uiName)
{
	m_Sprite = new CSprite();
	m_vPos = vPos;
	m_vRot = vRot;
	m_isShow = isShow;
	m_isLocked = false;
	m_UIName = uiName;
}

UI::~UI()
{
	delete m_Sprite;
}

void UI::UICreate(const TCHAR* filename)
{
	m_Sprite->SpriteLoad(filename, COLOR(1, 1, 1, 1));
}

void UI::UIUpdate()
{

}

void UI::UIDrawAB()
{
	D3DEngine* _d3d = D3DEngine::GetIns();
	ID3D11DeviceContext* _dc = _d3d->GetDXDC();

	ID3D11SamplerState* _pSampler = _d3d->GetSampler(SS_CLAMP);
	_dc->PSSetSamplers(0, 1, &_pSampler);

	ID3D11BlendState* _pBlendABON = _d3d->GetBlend(BS_AB_ON);
	ID3D11BlendState* _pBlendABOFF = _d3d->GetBlend(BS_AB_OFF);

	// AB ON
	_dc->OMSetBlendState(_pBlendABON, 0, 0xFFFFFFFF);

	m_Sprite->SpriteDraw(m_vPos, m_vRot);

	// 렌더링 옵션 복구  
	_dc->OMSetBlendState(_pBlendABOFF, 0, 0xFFFFFFFF);

}

void UI::UIDrawFX()
{
	D3DEngine* _d3d = D3DEngine::GetIns();
	ID3D11DeviceContext* _dc = _d3d->GetDXDC();

	ID3D11SamplerState* _pSampler = _d3d->GetSampler(SS_CLAMP);
	_dc->PSSetSamplers(0, 1, &_pSampler);

	ID3D11BlendState* _pColorBlendON = _d3d->GetBlend(BS_COLOR_BLEND_ONE);
	ID3D11BlendState* _pBlendABOFF = _d3d->GetBlend(BS_AB_OFF);

	// COLOR BLEND ON
	_dc->OMSetBlendState(_pColorBlendON, 0, 0xFFFFFFFF);

	m_Sprite->m_Color = XMFLOAT4(1, 1, 1, 0.5f);
	m_Sprite->SpriteDraw(m_vPos, m_vRot);

	// 렌더링 옵션 복구
	_dc->OMSetBlendState(_pBlendABOFF, 0, 0xFFFFFFFF);
}
