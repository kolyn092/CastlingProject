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

	// ���� Ŭ����
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

	// ���� Ŭ���� ���
	RegisterClass(&wndclass);

	// ���� ����
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

	// ������ ������ ȭ�鿡 ǥ��
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::UpdateWindow(m_hWnd);

	// Ŭ���̾�Ʈ ���� ũ�� ������
	ResizeWindow(m_hWnd, g_ScreenWidth, g_ScreenHeight);

	///HCURSOR _cur = LoadCursorFromFile(L"../Data/UI/Cursor/MouseCursor_Default.cur");

	// ���콺 Ŀ�� �⺻���� ����
	///SetCursor(LoadCursor(NULL, IDC_ARROW));
	///SetCursor(_cur);


	//////////////////////////////////////////////////////////////////////////

	/// ���� ���μ��� ����
	m_pGameProcess = CGameProcess::GetIns();
	// D3D, D2D �ʱ�ȭ
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
	// ���� �������� ��Ÿ�� ���ϱ�
	DWORD style = ::GetWindowLong(hWnd, GWL_STYLE);
	DWORD exstyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);

	// ���� ������ ��ü ũ�� ���
	RECT oldRC;
	::GetWindowRect(hWnd, &oldRC);

	// ���� ������ �������� Ŭ�� ���� ũ�� ���
	RECT newRC;
	newRC.left = 0;
	newRC.top = 0;
	newRC.right = width;
	newRC.bottom = height;

	// ���ο� Ŭ�� ���� ũ�� ��ŭ�� ������ ũ�⸦ ���ϱ�
	::AdjustWindowRectEx(&newRC, style, NULL, exstyle);

	// ������ �������� �ʺ�� ���� ���ϱ�
	int newWidth = newRC.right - newRC.left;
	int newHeight = newRC.bottom - newRC.top;

	// ���ο� ũ��� ����
	::SetWindowPos(hWnd, HWND_NOTOPMOST, oldRC.left, oldRC.top, newWidth, newHeight, SWP_SHOWWINDOW);
}