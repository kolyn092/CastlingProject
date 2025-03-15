#pragma once
#include "CSprite.h"

class UI
{
public:
	CSprite* m_Sprite;
	Vector3 m_vPos;
	Vector3 m_vRot;
	bool m_isShow;
	bool m_isLocked;

	const TCHAR* m_UIName;

public:
	UI();
	UI(Vector3 vPos, Vector3 vRot, bool isShow, const TCHAR* uiName);
	~UI();

public:
	void UICreate(const TCHAR* filename);
	void UIUpdate();
	void UIDrawAB();
	void UIDrawFX();
};

