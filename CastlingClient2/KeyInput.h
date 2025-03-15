#pragma once
#include <windows.h>

///
///
///  Key �Է� �޴� Ŭ����
///
///  [8/11/2020 Kolyn]

class KeyInput
{

public:
	POINT m_MousePos;

	HCURSOR m_DownCur;

	// �ܼ��� ���콺 ��Ŭ����ư�� ���� �ִ°� �ƴѰ�
	bool m_IsLButtonDown;

	// ���� ���¿� �޶��� ���� (�� �ѹ�)
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

