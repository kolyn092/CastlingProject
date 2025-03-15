#pragma once
#include <windows.h>

///
///
///  Key 입력 받는 클래스
///
///  [8/11/2020 Kolyn]

class KeyInput
{

public:
	POINT m_MousePos;

	HCURSOR m_DownCur;

	// 단순히 마우스 좌클릭버튼이 눌려 있는가 아닌가
	bool m_IsLButtonDown;

	// 이전 상태와 달라진 시점 (딱 한번)
	bool m_IsLButtonUpJust;
	bool m_IsLButtonDownJust;


public:
	KeyInput();
	~KeyInput();

private:
	static KeyInput* m_KeyInstance;

public:
	static KeyInput* GetIns()
	{
		if (m_KeyInstance == nullptr)
		{
			m_KeyInstance = new KeyInput();
		}
		return m_KeyInstance;
	}

public:
	void Input();
	void GetMousePos(HWND hwnd);
	XMVECTOR ViewPortToWorld();
	void Release();

public:
	bool GetIsLButtonUpJust();
};

