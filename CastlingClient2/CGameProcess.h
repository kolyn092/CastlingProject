#pragma once
#include "D3DEngine.h"
#include "ModelObject.h"
#include "KeyInput.h"
#include "ModelLoadManager.h"
#include "ObjectManager.h"
#include "UIManager.h"
#include "SkyBox.h"
#include "SoundManager.h"


///
///
///  ���� ���μ���
///
///  [8/11/2020 Kolyn]

class CGameProcess
{

private:
	// D3D ���� Ŭ����
	D3DEngine* m_D3DEngine;

	// Ű �Է� Ŭ����
	KeyInput* m_KeyInput;

	// �� �ε����ִ� �Ŵ��� Ŭ����
	ModelLoadManager* m_ModelLoadManager;

public:
	// ������Ʈ �������ִ� �Ŵ��� Ŭ����
	ObjectManager* m_ObjManager;

	// UI �������ִ� �Ŵ��� Ŭ����
	UIManager* m_UIManager;

	// ���� �������ִ� �Ŵ��� Ŭ����
	SoundManager* m_SoundManager;

private:
	SkyBox* m_SkyBox;

public:
	bool m_pressTurnBtn;
	
	eGameState m_PrevGameState;
	eGameState m_GameState;
	eInGameState m_InGameState;
	ePickingState m_PickingState;

	bool m_StartInGameBGM;
	bool m_ResultInGameBGM;

	Piece::Type m_CreateUnit;
	bool m_isCoin;

	eMatchUser m_MatchUser;

	int m_MapStartPosX;
	int m_MapStartPosY;

	int m_MapEndPosX;
	int m_MapEndPosY;

	int m_CreatePosX;
	int m_CreatePosY;

	int m_CastleLevel_1P;
	int m_CastleLevel_2P;

	bool m_CamInitalize;

	bool m_SettingLeader;

	bool m_CastlingWarning;

	bool m_PPWarning;	//����� ���
	bool m_APWarning;	//�ൿ�� ���

private:
	bool m_bDebugShow;


private:
	CGameProcess();
	~CGameProcess();

private:
	static CGameProcess* m_GameProcessInstance;

public:
	static CGameProcess* GetIns()
	{
		if (m_GameProcessInstance == nullptr)
		{
			m_GameProcessInstance = new CGameProcess();
		}
		return m_GameProcessInstance;
	}

public:
	//����
	void Create(HINSTANCE hInst, HWND hWnd, int screenWidth, int screenHeight);

	//����
	void KeyInputUpdate(HWND hWnd);
	void Update();
	void GameUpdate(float dTime);
	void Reset_Animation_Attack();
	void Reset_Animation_Die();
	void Draw(float dTime);

	void DebugInGameState(int x, int y);
	void DebugPickingState(int x, int y);

	//����
	void OnClose();
};

