#include "DXEngineDefine.h"
#include "D3DEngine.h"
#include "KeyInput.h"

KeyInput* KeyInput::m_KeyInstance = nullptr;

KeyInput::KeyInput()
{
	m_MousePos = POINT{ 0, 0 };

	m_DownCur = LoadCursorFromFile(L"../Data/UI/Cursor/MouseCursor_Click.cur");

	m_IsLButtonDown = false;
	m_IsLButtonUpJust = false;
	m_IsLButtonDownJust = false;
}

KeyInput::~KeyInput()
{

}

extern DWORD g_MouseState;

void KeyInput::Input()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		SetCursor(m_DownCur);

		// 마우스가 떨어져 있다가 눌린 시점
		if (m_IsLButtonDown == false)
		{
			m_IsLButtonDownJust = true;
		}
		else
		{
			m_IsLButtonDownJust = false;
		}

		m_IsLButtonDown = true;
	}
	else
	{
		// 마우스가 눌려 있다가 떨어진 시점
		if (m_IsLButtonDown == true)
		{
			m_IsLButtonUpJust = true;
		}
		else
		{
			m_IsLButtonUpJust = false;
		}

		m_IsLButtonDown = false;
	}
}

bool KeyInput::GetIsLButtonUpJust()
{
	return m_IsLButtonUpJust;
}

void KeyInput::GetMousePos(HWND hwnd)
{
	POINT temp;

	// 윈도우 마우스 좌표 획득
	GetCursorPos(&temp);
	ScreenToClient(hwnd, &temp);

	// 현재 장치의 화면모드 및 모니터 정보획득 (단일 모니터 기준)
	BOOL bFullScreen = FALSE;
	IDXGIOutput* pOutput = NULL;
	D3DEngine::GetIns()->GetSwapChain()->GetFullscreenState(&bFullScreen, &pOutput);

	DXGI_MODE_DESC mode = D3DEngine::GetIns()->GetDisplayMode();

	if (bFullScreen == FALSE)
	{
		// 창모드 일 경우
		m_MousePos = temp;
	}
	else
	{
		// 전체화면 모드일 경우	
		// 모니터해상도 기준, 좌표계를 스케일 (단일 모니터 기준)
		DXGI_OUTPUT_DESC dc;
		ZeroMemory(&dc, sizeof(dc));
		pOutput->GetDesc(&dc);

		// 전체화면시 모니터해상도 기준 좌표 보정
		m_MousePos.x = (LONG)(temp.x * ((float)mode.Width) / dc.DesktopCoordinates.right);
		m_MousePos.y = (LONG)(temp.y * ((float)mode.Height) / dc.DesktopCoordinates.bottom);
	}

	// 화면 영역에 위치하도록 보정
	if (m_MousePos.x > (LONG)mode.Width - 1) m_MousePos.x = mode.Width - 1;
	if (m_MousePos.x < 0) m_MousePos.x = 0;
	if (m_MousePos.y > (LONG)mode.Height - 1) m_MousePos.y = mode.Height - 1;
	if (m_MousePos.y < 0) m_MousePos.y = 0;

	// 인터페이스 해제
	SAFE_RELEASE(pOutput);
}

XMVECTOR KeyInput::ViewPortToWorld()
{
	DXGI_MODE_DESC mode = D3DEngine::GetIns()->GetDisplayMode();
	Matrix ViewMatrix = D3DEngine::GetIns()->m_Camera->mView;
	Matrix ProjMatrix = D3DEngine::GetIns()->m_Camera->mProj;

	XMVECTOR vPos1 = XMVector3Unproject(XMVectorSet((float)m_MousePos.x, (float)m_MousePos.y, 1, 0), 0, 0, (float)mode.Width, (float)mode.Height, 0, 1, ProjMatrix, ViewMatrix, XMMatrixIdentity());
	XMVECTOR vPos2 = XMVectorSet(0, 0, 0, 1);
	XMMATRIX invView = XMMatrixInverse(NULL, ViewMatrix);
	vPos2 = XMVector3TransformCoord(vPos2, invView);

	XMVECTOR vPlane = XMPlaneFromPointNormal(XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0));
	XMVECTOR Result = XMPlaneIntersectLine(vPlane, vPos1, vPos2);

	Result = XMVectorSetW(Result, 1);

	return Result;
}

void KeyInput::Release()
{
	SAFE_DELETE(m_KeyInstance);
}
