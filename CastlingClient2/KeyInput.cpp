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

		// ���콺�� ������ �ִٰ� ���� ����
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
		// ���콺�� ���� �ִٰ� ������ ����
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

	// ������ ���콺 ��ǥ ȹ��
	GetCursorPos(&temp);
	ScreenToClient(hwnd, &temp);

	// ���� ��ġ�� ȭ���� �� ����� ����ȹ�� (���� ����� ����)
	BOOL bFullScreen = FALSE;
	IDXGIOutput* pOutput = NULL;
	D3DEngine::GetIns()->GetSwapChain()->GetFullscreenState(&bFullScreen, &pOutput);

	DXGI_MODE_DESC mode = D3DEngine::GetIns()->GetDisplayMode();

	if (bFullScreen == FALSE)
	{
		// â��� �� ���
		m_MousePos = temp;
	}
	else
	{
		// ��üȭ�� ����� ���	
		// ������ػ� ����, ��ǥ�踦 ������ (���� ����� ����)
		DXGI_OUTPUT_DESC dc;
		ZeroMemory(&dc, sizeof(dc));
		pOutput->GetDesc(&dc);

		// ��üȭ��� ������ػ� ���� ��ǥ ����
		m_MousePos.x = (LONG)(temp.x * ((float)mode.Width) / dc.DesktopCoordinates.right);
		m_MousePos.y = (LONG)(temp.y * ((float)mode.Height) / dc.DesktopCoordinates.bottom);
	}

	// ȭ�� ������ ��ġ�ϵ��� ����
	if (m_MousePos.x > (LONG)mode.Width - 1) m_MousePos.x = mode.Width - 1;
	if (m_MousePos.x < 0) m_MousePos.x = 0;
	if (m_MousePos.y > (LONG)mode.Height - 1) m_MousePos.y = mode.Height - 1;
	if (m_MousePos.y < 0) m_MousePos.y = 0;

	// �������̽� ����
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
