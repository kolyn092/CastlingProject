#pragma once
#include <vector>
#include "UI.h"
#include "GameDefine.h"
#include "SoundManager.h"

class UIManager
{

public:
	enum BSTATE { BS_STANDBY, BS_CURSORON, BS_PUSH, BS_MAX };		// UI버튼 상태: 대기, 커서올라감, 눌림 
	
public:
	std::vector<UI*> m_UIList_Main;
	std::vector<UI*> m_UIList_MainButton;
	std::vector<UI*> m_UIList_HowTo;
	std::vector<UI*> m_UIList_HowToButton;
	std::vector<UI*> m_UIList_Matching;
	std::vector<UI*> m_UIList_MatchingButton;
	std::vector<UI*> m_UIList_LeaderSelect;
	std::vector<UI*> m_UIList_LeaderSelectButton;
	std::vector<UI*> m_UIList_InGame_Option;
	std::vector<UI*> m_UIList_InGame_OptionButton;
	std::vector<UI*> m_UIList_InGame;
	std::vector<UI*> m_UIList_InGameButton;
	std::vector<UI*> m_UIList_InGameLocked;
	std::vector<UI*> m_UIList_InGameTimer;
	std::vector<UI*> m_UIList_Result;
	std::vector<UI*> m_UIList_ResultButton;

public:
	BOOL m_BtnState_Main[g_MainBtnCnt] = { BS_STANDBY, };
	BOOL m_BtnState_HowTo[g_MainBtnCnt] = { BS_STANDBY, };
	BOOL m_BtnState_LeaderSelect[g_LeaderSelectBtnCnt] = { BS_STANDBY, };
	BOOL m_BtnState_InGame[g_InGameBtnCnt] = { BS_STANDBY, };
	BOOL m_BtnState_InGame_Option[g_InGameBtnCnt] = { BS_STANDBY, };
	BOOL m_BtnState_Result[g_InResultBtnCnt] = { BS_STANDBY, };

private:
	SoundManager* m_SoundManager;

public:
	BOOL m_isEffectOn;
	BOOL m_isMyCastling;
	BOOL m_isEnemyCastling;
	int m_nowHowToPage;
	const int maxHowToPage = 5;
	BOOL m_isExitButton;

public:
	UIManager();
	~UIManager();

	void ReleaseVector(std::vector<UI*> uiList);
public:
	void UICreateAll();
	void UICreate_Main(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow);
	void UICreate_HowTo(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow);
	void UICreate_Matching(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow);
	void UICreate_LeaderSelect(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow);
	void UICreate_InGame_Option(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow);
	void UICreate_InGame(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow);
	void UICreate_Result(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow);
	void SetLeaderUI();
	void UIUpdateAll_Main(POINT pt);
	void UIUpdateAll_HowTo(POINT pt);
	void UIUpdateAll_LeaderSelect(POINT pt);
	void UIUpdateAll_InGame();
	void UIUpdate_InGameTurn();
	void UIUpdate_InGameCastling();
	void UIUpdate_InGameWarning();
	void UIUpdate_InGameLocked();
	void UIUpdateAll_InGameButton(POINT pt);
	void UIUpdateAll_InGame_Option();
	void UIUpdateAll_InGame_OptionButton(POINT pt);
	void UIUpdate_InGameTimer();
	void ToolTipAction(int i);
	void UIUpdateAll_Result(POINT pt);
	void ButtonAction_Main(int i);
	void ButtonAction_HowTo(int i);
	void ButtonAction_LeaderSelect(int i);
	void ButtonAction_Option(int i);
	void ButtonAction_InGame(int i);
	void ButtonAction_Result(int i);
	void UIDrawAll_Main();
	void UIDrawAll_HowTo();
	void UIDrawAll_Matching();
	void UIDrawAll_LeaderSelect();
	void UIDrawAll_Option();
	void UIDrawAll_InGame();
	void UIDraw_Timer();
	void UIDrawAll_Result();
	void DebugBtnState(int x, int y);
};

