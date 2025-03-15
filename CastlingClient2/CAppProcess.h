#pragma once
#include <windows.h>
#include "CGameProcess.h"

class CAppProcess
{
public:
	CAppProcess();
	~CAppProcess();


private:
	// 윈도 관련
	HWND m_hWnd;
	MSG m_Msg;

	// 게임 엔진
	CGameProcess* m_pGameProcess;

public:
	HRESULT Initialize(HINSTANCE hInstance);
	void Loop();
	void Finalize();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ResizeWindow(HWND hWnd, UINT width, UINT height);
};

