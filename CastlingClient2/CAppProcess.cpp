#include <tchar.h>
#include "CAppProcess.h"
#include "DXEngineDefine.h"
#include "NetworkCallback.h"

CAppProcess::CAppProcess()
	: m_hWnd(NULL), m_Msg(), m_pGameProcess(nullptr)
{

}

CAppProcess::~CAppProcess()
{

}

HRESULT CAppProcess::Initialize(HINSTANCE hInstance)
{
	HCURSOR _cur = LoadCursorFromFile(L"../Data/UI/Cursor/MouseCursor_Default.cur");
	HICON _icon = LoadIcon(hInstance, MAKEINTRESOURCE(102));

	// 윈도 클래스
	TCHAR szAppName[] = _TEXT("Castling");
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = CAppProcess::WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = _icon;
	wndclass.hCursor = _cur;//LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	// 윈도 클래스 등록
	RegisterClass(&wndclass);

	// 윈도 생성
	m_hWnd = CreateWindow(
		szAppName,
		szAppName,
		//WS_POPUP | WS_MAXIMIZE,
		//WS_OVERLAPPEDWINDOW,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, g_ScreenWidth, g_ScreenHeight,
		GetDesktopWindow(), NULL, hInstance, NULL);

	if (!m_hWnd) return FALSE;

	if (FAILED(CoInitialize(NULL)))
	{
		return S_FALSE;
	}

	// 생성된 윈도를 화면에 표시
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::UpdateWindow(m_hWnd);

	// 클라이언트 영역 크기 재조정
	ResizeWindow(m_hWnd, g_ScreenWidth, g_ScreenHeight);

	///HCURSOR _cur = LoadCursorFromFile(L"../Data/UI/Cursor/MouseCursor_Default.cur");

	// 마우스 커서 기본으로 설정
	///SetCursor(LoadCursor(NULL, IDC_ARROW));
	///SetCursor(_cur);


	//////////////////////////////////////////////////////////////////////////

	/// 게임 프로세스 관련
	m_pGameProcess = CGameProcess::GetIns();
	// D3D, D2D 초기화
	m_pGameProcess->Create(hInstance, m_hWnd, g_ScreenWidth, g_ScreenHeight);

	return S_OK;
}

void CAppProcess::Loop()
{
	while (true)
	{
		if (::PeekMessage(&m_Msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_Msg.message == WM_QUIT) break;

			::TranslateMessage(&m_Msg);
			::DispatchMessage(&m_Msg);
		}
		else
		{
			m_pGameProcess->KeyInputUpdate(m_hWnd);
			m_pGameProcess->Update();
			///m_pGameProcess->Draw();
		}
	}
}

void CAppProcess::Finalize()
{
	m_pGameProcess->OnClose();
}

extern DWORD g_MouseState;
extern bool g_captionClose;

LRESULT CALLBACK CAppProcess::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CAppProcess::ResizeWindow(HWND hWnd, UINT width, UINT height)
{
	// 현재 윈도우의 스타일 구하기
	DWORD style = ::GetWindowLong(hWnd, GWL_STYLE);
	DWORD exstyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);

	// 현재 윈도우 전체 크기 얻기
	RECT oldRC;
	::GetWindowRect(hWnd, &oldRC);

	// 새로 생성할 윈도우의 클라 영역 크기 계산
	RECT newRC;
	newRC.left = 0;
	newRC.top = 0;
	newRC.right = width;
	newRC.bottom = height;

	// 새로운 클라 영역 크기 만큼의 윈도우 크기를 구하기
	::AdjustWindowRectEx(&newRC, style, NULL, exstyle);

	// 보정된 윈도우의 너비와 폭을 구하기
	int newWidth = newRC.right - newRC.left;
	int newHeight = newRC.bottom - newRC.top;

	// 새로운 크기로 설정
	::SetWindowPos(hWnd, HWND_NOTOPMOST, oldRC.left, oldRC.top, newWidth, newHeight, SWP_SHOWWINDOW);
}