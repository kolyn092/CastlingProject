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
///  게임 프로세스
///
///  [8/11/2020 Kolyn]

class CGameProcess
{

private:
	// D3D 엔진 클래스
	D3DEngine* m_D3DEngine;

	// 키 입력 클래스
	KeyInput* m_KeyInput;

	// 모델 로드해주는 매니저 클래스
	ModelLoadManager* m_ModelLoadManager;

public:
	// 오브젝트 관리해주는 매니저 클래스
	ObjectManager* m_ObjManager;

	// UI 관리해주는 매니저 클래스
	UIManager* m_UIManager;

	// 사운드 관리해주는 매니저 클래스
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

	bool m_PPWarning;	//생산력 경고
	bool m_APWarning;	//행동력 경고

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
	//시작
	void Create(HINSTANCE hInst, HWND hWnd, int screenWidth, int screenHeight);

	//로직
	void KeyInputUpdate(HWND hWnd);
	void Update();
	void GameUpdate(float dTime);
	void Reset_Animation_Attack();
	void Reset_Animation_Die();
	void Draw(float dTime);

	void DebugInGameState(int x, int y);
	void DebugPickingState(int x, int y);

	//종료
	void OnClose();
};

