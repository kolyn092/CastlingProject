#include "UIManager.h"
#include "D3DEngine.h"
#include "CGameProcess.h"
#include <string>
#include "NetworkCallback.h"
#include <windows.h>				// timeGetTime용
#pragma comment(lib, "winmm.lib")	// timeGetTime용

bool g_captionClose = false;

DWORD g_MouseState = MS_STANDBY;

UIManager::UIManager()
{
	m_SoundManager = SoundManager::GetIns();

	m_isEffectOn = TRUE;
	m_isMyCastling = FALSE;
	m_isEnemyCastling = FALSE;

	m_nowHowToPage = 0;
}

UIManager::~UIManager()
{
	ReleaseVector(m_UIList_Main);
	ReleaseVector(m_UIList_MainButton);
	ReleaseVector(m_UIList_HowTo);
	ReleaseVector(m_UIList_HowToButton);
	ReleaseVector(m_UIList_Matching);
	ReleaseVector(m_UIList_MatchingButton);
	ReleaseVector(m_UIList_LeaderSelect);
	ReleaseVector(m_UIList_LeaderSelectButton);
	ReleaseVector(m_UIList_InGame_Option);
	ReleaseVector(m_UIList_InGame_OptionButton);
	ReleaseVector(m_UIList_InGame);
	ReleaseVector(m_UIList_InGameButton);
	ReleaseVector(m_UIList_InGameLocked);
	ReleaseVector(m_UIList_InGameTimer);
	ReleaseVector(m_UIList_Result);
	ReleaseVector(m_UIList_ResultButton);
}

void UIManager::ReleaseVector(std::vector<UI*> uiList)
{
	for (auto& ui : uiList)
	{
		delete ui;
		ui = nullptr;
	}
	uiList.clear();
}

void UIManager::UICreateAll()
{
	/// 타이틀
	UICreate_Main(eUICategory::NONE, L"1_bg.png", Vector3(0, 0, 1), Vector3(0, 0, 0), true);
	UICreate_Main(eUICategory::BUTTON, L"1_button_1.png", Vector3(1410, 614, 1), Vector3(0, 0, 0), true);
	UICreate_Main(eUICategory::BUTTON, L"1_button_2.png", Vector3(1410, 712, 1), Vector3(0, 0, 0), true);
	UICreate_Main(eUICategory::BUTTON, L"1_button_3.png", Vector3(1410, 810, 1), Vector3(0, 0, 0), true);
	UICreate_Main(eUICategory::BUTTON, L"1_button_4.png", Vector3(1410, 908, 1), Vector3(0, 0, 0), true);
	UICreate_Main(eUICategory::NONE, L"11_bg.png", Vector3(0, 0, 1), Vector3(0, 0, 0), false);

	/// HOWTO
	UICreate_HowTo(eUICategory::NONE, L"10_bg_1.png", Vector3(0, 0, 1), Vector3(0, 0, 0), true);
	UICreate_HowTo(eUICategory::NONE, L"10_bg_2.png", Vector3(0, 0, 1), Vector3(0, 0, 0), false);
	UICreate_HowTo(eUICategory::NONE, L"10_bg_3.png", Vector3(0, 0, 1), Vector3(0, 0, 0), false);
	UICreate_HowTo(eUICategory::NONE, L"10_bg_4.png", Vector3(0, 0, 1), Vector3(0, 0, 0), false);
	UICreate_HowTo(eUICategory::NONE, L"10_bg_5.png", Vector3(0, 0, 1), Vector3(0, 0, 0), false);
	UICreate_HowTo(eUICategory::BUTTON, L"10_button.png", Vector3(1657, 957, 1), Vector3(0, 0, 0), true);

	/// 매칭
	UICreate_Matching(eUICategory::NONE, L"3_bg.png", Vector3(0, 0, 1), Vector3(0, 0, 0), true);
	UICreate_Matching(eUICategory::NONE, L"3_effect.png", Vector3(794, 952, 1), Vector3(0, 0, 0), true);

	/// 지도자 선택
	UICreate_LeaderSelect(eUICategory::NONE, L"4_bg.png", Vector3(0, 50, 1), Vector3(0, 0, 0), true);
	UICreate_LeaderSelect(eUICategory::NONE, L"4_logo.png", Vector3(35, 892, 1), Vector3(0, 0, 0), true);
	UICreate_LeaderSelect(eUICategory::BUTTON, L"4_demeter.png", Vector3(292, 797, 1), Vector3(0, 0, 0), true);
	UICreate_LeaderSelect(eUICategory::NONE, L"4_selectbox.png", Vector3(289, 794, 1), Vector3(0, 0, 0), false);
	UICreate_LeaderSelect(eUICategory::BUTTON, L"4_ares.png", Vector3(743, 797, 1), Vector3(0, 0, 0), true);
	UICreate_LeaderSelect(eUICategory::NONE, L"4_selectbox.png", Vector3(740, 794, 1), Vector3(0, 0, 0), false);
	UICreate_LeaderSelect(eUICategory::BUTTON, L"4_hades.png", Vector3(1189, 797, 1), Vector3(0, 0, 0), true);
	UICreate_LeaderSelect(eUICategory::NONE, L"4_selectbox.png", Vector3(1186, 794, 1), Vector3(0, 0, 0), false);
	UICreate_LeaderSelect(eUICategory::BUTTON, L"4_start.png", Vector3(1644, 868, 1), Vector3(0, 0, 0), true);

	/// 옵션
	UICreate_InGame_Option(eUICategory::NONE, L"2_bg.png", Vector3(0, 0, 1), Vector3(0, 0, 0), true);
	UICreate_InGame_Option(eUICategory::BUTTON, L"2_button_1.png", Vector3(884, 388, 1), Vector3(0, 0, 0), true);
	UICreate_InGame_Option(eUICategory::BUTTON, L"2_button_2.png", Vector3(884, 498, 1), Vector3(0, 0, 0), true);
	UICreate_InGame_Option(eUICategory::BUTTON, L"2_button_3.png", Vector3(884, 608, 1), Vector3(0, 0, 0), true);
	UICreate_InGame_Option(eUICategory::NONE, L"9_text.png", Vector3(838, 421, 1), Vector3(0, 0, 0), false);
	UICreate_InGame_Option(eUICategory::BUTTON, L"9_icon_yes.png", Vector3(833, 524, 1), Vector3(0, 0, 0), false);
	UICreate_InGame_Option(eUICategory::BUTTON, L"9_icon_no.png", Vector3(1044, 524, 1), Vector3(0, 0, 0), false);

	/// 인게임 N0
	UICreate_InGame(eUICategory::NONE, L"5_bg.png", Vector3(0, 0, 1), Vector3(0, 0, 0), true);

	// 지도자 MY N1
	UICreate_InGame(eUICategory::NONE, L"5_1p_demeter.png", Vector3(17, 885, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_1p_ares.png", Vector3(29, 859, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_1p_hades.png", Vector3(11, 887, 1), Vector3(0, 0, 0), false);

	// 지도자 ENEMY N4
	UICreate_InGame(eUICategory::NONE, L"5_2p_demeter.png", Vector3(1760, 26, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_2p_ares.png", Vector3(1759, 11, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_2p_hades.png", Vector3(1758, 34, 1), Vector3(0, 0, 0), false);

	// 경고창 N7
	UICreate_InGame(eUICategory::NONE, L"5_warning_1.png", Vector3(810, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_2.png", Vector3(890, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3.png", Vector3(807, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_4.png", Vector3(882, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_5.png", Vector3(898, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_wheat.png", Vector3(860, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_iron.png", Vector3(860, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_horse.png", Vector3(860, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_no1.png", Vector3(1043, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_no2.png", Vector3(1042, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_no3.png", Vector3(1042, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_no4.png", Vector3(1041, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_no5.png", Vector3(1042, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_no6.png", Vector3(1042, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_no7.png", Vector3(1042, 842, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_warning_3_no8.png", Vector3(1042, 842, 1), Vector3(0, 0, 0), false);

	// 알림창 N23
	UICreate_InGame(eUICategory::NONE, L"5_notice_3.png", Vector3(309, 49, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_notice_1.png", Vector3(309, 49, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_notice_2.png", Vector3(309, 49, 1), Vector3(0, 0, 0), false);

	// 캐릭터 버튼 창 N26
	UICreate_InGame(eUICategory::NONE, L"5_cha.png", Vector3(335, 914, 1), Vector3(0, 0, 0), true);

	// 성 증축 B0
	UICreate_InGame(eUICategory::BUTTON, L"5_button_1.png", Vector3(469, 931, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::BUTTON, L"5_button_2.png", Vector3(469, 997, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(469, 931, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(469, 997, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::BUTTON, L"5_cha_tooltip.png", Vector3(436, 936, 1), Vector3(0, 0, 0), true);

	// 일꾼 B3
	UICreate_InGame(eUICategory::BUTTON, L"5_button_3.png", Vector3(680, 931, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::BUTTON, L"5_button_4.png", Vector3(680, 997, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(680, 931, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(680, 997, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::BUTTON, L"5_cha_tooltip.png", Vector3(647, 936, 1), Vector3(0, 0, 0), true);

	// 검사 B6
	UICreate_InGame(eUICategory::BUTTON, L"5_button_5.png", Vector3(892, 931, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::BUTTON, L"5_button_6.png", Vector3(892, 997, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(892, 931, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(892, 997, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::BUTTON, L"5_cha_tooltip.png", Vector3(859, 936, 1), Vector3(0, 0, 0), true);

	// 궁수 B9
	UICreate_InGame(eUICategory::BUTTON, L"5_button_7.png", Vector3(1103, 931, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::BUTTON, L"5_button_8.png", Vector3(1103, 997, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(1103, 931, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(1103, 997, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::BUTTON, L"5_cha_tooltip.png", Vector3(1070, 936, 1), Vector3(0, 0, 0), true);

	// 방패 B12
	UICreate_InGame(eUICategory::BUTTON, L"5_button_9.png", Vector3(1314, 931, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::BUTTON, L"5_button_10.png", Vector3(1314, 997, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(1314, 931, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(1314, 997, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::BUTTON, L"5_cha_tooltip.png", Vector3(1281, 936, 1), Vector3(0, 0, 0), true);

	// 기사 B15
	UICreate_InGame(eUICategory::BUTTON, L"5_button_11.png", Vector3(1525, 931, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::BUTTON, L"5_button_12.png", Vector3(1525, 997, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(1525, 931, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::LOCKED, L"5_button_lock.png", Vector3(1525, 997, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::BUTTON, L"5_cha_tooltip.png", Vector3(1492, 936, 1), Vector3(0, 0, 0), true);

	// 툴팁 N27
	UICreate_InGame(eUICategory::NONE, L"5_tooltip_1.png", Vector3(380, 771, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_tooltip_2.png", Vector3(590, 771, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_tooltip_3.png", Vector3(801, 771, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_tooltip_4.png", Vector3(1011, 771, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_tooltip_5.png", Vector3(1222, 771, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_tooltip_6.png", Vector3(1432, 771, 1), Vector3(0, 0, 0), false);

	// 턴 변경 N33 B18
	UICreate_InGame(eUICategory::NONE, L"5_myturn_icon.png", Vector3(1686, 908, 1), Vector3(0, 0, 0), true);
	UICreate_InGame(eUICategory::BUTTON, L"5_turnend.png", Vector3(1644, 868, 1), Vector3(0, 0, 0), true);

	// 턴 변경 - 적턴 표시 N34
	UICreate_InGame(eUICategory::NONE, L"5_enemyturn_icon.png", Vector3(1686, 908, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::NONE, L"5_turnWait.png", Vector3(1783, 1018, 1), Vector3(0, 0, 0), false);
	
	// 알림창 - 적턴 N36
	UICreate_InGame(eUICategory::NONE, L"5_notice_4.png", Vector3(309, 49, 1), Vector3(0, 0, 0), false);

	// 타이머 숫자 N34
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no0.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no1.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no2.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no3.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no4.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no5.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no6.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no7.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no8.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no9.png", Vector3(1810, 963, 1), Vector3(0, 0, 0), false);

	UICreate_InGame(eUICategory::TIMER, L"5_timer_no0.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no1.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no2.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no3.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no4.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no5.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no6.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no7.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no8.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);
	UICreate_InGame(eUICategory::TIMER, L"5_timer_no9.png", Vector3(1780, 963, 1), Vector3(0, 0, 0), false);

	// 결과창
	UICreate_Result(eUICategory::NONE, L"8_bg_1.png", Vector3(0, 0, 1), Vector3(0, 0, 0), false);
	UICreate_Result(eUICategory::NONE, L"8_bg_2.png", Vector3(0, 0, 1), Vector3(0, 0, 0), false);
	UICreate_Result(eUICategory::BUTTON, L"8_button_1.png", Vector3(880, 801, 1), Vector3(0, 0, 0), false);
	UICreate_Result(eUICategory::BUTTON, L"8_button_2.png", Vector3(880, 801, 1), Vector3(0, 0, 0), false);
}

void UIManager::UICreate_Main(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow)
{
	UI* _ui = new UI(vPos, vRot, isShow, filename);

	_ui->UICreate((L"../Data/UI/Main/" + std::wstring(filename)).c_str());

	switch (uiState)
	{
		case eUICategory::NONE:
			m_UIList_Main.push_back(_ui);
			break;
		case eUICategory::BUTTON:
			m_UIList_MainButton.push_back(_ui);
			break;
	}
}

void UIManager::UICreate_HowTo(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow)
{
	UI* _ui = new UI(vPos, vRot, isShow, filename);

	_ui->UICreate((L"../Data/UI/HowTo/" + std::wstring(filename)).c_str());

	switch (uiState)
	{
		case eUICategory::NONE:
			m_UIList_HowTo.push_back(_ui);
			break;
		case eUICategory::BUTTON:
			m_UIList_HowToButton.push_back(_ui);
			break;
	}
}

void UIManager::UICreate_Matching(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow)
{
	UI* _ui = new UI(vPos, vRot, isShow, filename);

	_ui->UICreate((L"../Data/UI/Matching/" + std::wstring(filename)).c_str());

	switch (uiState)
	{
		case eUICategory::NONE:
			m_UIList_Matching.push_back(_ui);
			break;
		case eUICategory::BUTTON:
			m_UIList_MatchingButton.push_back(_ui);
			break;
	}
}

void UIManager::UICreate_LeaderSelect(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow)
{
	UI* _ui = new UI(vPos, vRot, isShow, filename);

	_ui->UICreate((L"../Data/UI/InGame/LeaderSelect/" + std::wstring(filename)).c_str());

	switch (uiState)
	{
		case eUICategory::NONE:
			m_UIList_LeaderSelect.push_back(_ui);
			break;
		case eUICategory::BUTTON:
			m_UIList_LeaderSelectButton.push_back(_ui);
			break;
	}
}

void UIManager::UICreate_InGame_Option(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow)
{
	UI* _ui = new UI(vPos, vRot, isShow, filename);

	_ui->UICreate((L"../Data/UI/Option/" + std::wstring(filename)).c_str());

	switch (uiState)
	{
		case eUICategory::NONE:
			m_UIList_InGame_Option.push_back(_ui);
			break;
		case eUICategory::BUTTON:
			m_UIList_InGame_OptionButton.push_back(_ui);
			break;
	}
}

void UIManager::UICreate_InGame(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow)
{
	UI* _ui = new UI(vPos, vRot, isShow, filename);

	_ui->UICreate((L"../Data/UI/InGame/" + std::wstring(filename)).c_str());

	switch (uiState)
	{
		case eUICategory::NONE:
			m_UIList_InGame.push_back(_ui);
			break;
		case eUICategory::BUTTON:
			m_UIList_InGameButton.push_back(_ui);
			break;
		case eUICategory::LOCKED:
			m_UIList_InGameLocked.push_back(_ui);
			break;
		case eUICategory::TIMER:
			m_UIList_InGameTimer.push_back(_ui);
			break;
	}
}

void UIManager::UICreate_Result(eUICategory uiState, const TCHAR* filename, Vector3 vPos, Vector3 vRot, bool isShow)
{
	UI* _ui = new UI(vPos, vRot, isShow, filename);

	_ui->UICreate((L"../Data/UI/Result/" + std::wstring(filename)).c_str());

	switch (uiState)
	{
		case eUICategory::NONE:
			m_UIList_Result.push_back(_ui);
			break;
		case eUICategory::BUTTON:
			m_UIList_ResultButton.push_back(_ui);
			break;
	}
}

void UIManager::SetLeaderUI()
{
	switch (myData.leader.type)
	{
		case Leader::Type::DEMETER:
		{
			m_UIList_InGame[1]->m_isShow = true;
			m_UIList_InGame[2]->m_isShow = false;
			m_UIList_InGame[3]->m_isShow = false;
		}
		break;
		case Leader::Type::ARES:
		{
			m_UIList_InGame[1]->m_isShow = false;
			m_UIList_InGame[2]->m_isShow = true;
			m_UIList_InGame[3]->m_isShow = false;
		}
		break;
		case Leader::Type::HADES:
		{
			m_UIList_InGame[1]->m_isShow = false;
			m_UIList_InGame[2]->m_isShow = false;
			m_UIList_InGame[3]->m_isShow = true;
		}
		break;
		default:
			break;
	}

	switch (enemyData.leader.type)
	{
		case Leader::Type::DEMETER:
		{
			m_UIList_InGame[4]->m_isShow = true;
			m_UIList_InGame[5]->m_isShow = false;
			m_UIList_InGame[6]->m_isShow = false;
		}
		break;
		case Leader::Type::ARES:
		{
			m_UIList_InGame[4]->m_isShow = false;
			m_UIList_InGame[5]->m_isShow = true;
			m_UIList_InGame[6]->m_isShow = false;
		}
		break;
		case Leader::Type::HADES:
		{
			m_UIList_InGame[4]->m_isShow = false;
			m_UIList_InGame[5]->m_isShow = false;
			m_UIList_InGame[6]->m_isShow = true;
		}
		break;
		default:
			break;
	}
}

void UIManager::UIUpdateAll_Main(POINT pt)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();
	if (gameProcess->m_GameState == eGameState::TITLE)
	{
		m_UIList_Main[0]->m_isShow = true;
		m_UIList_Main[1]->m_isShow = false;
		m_UIList_MainButton[0]->m_isShow = true;
		m_UIList_MainButton[1]->m_isShow = true;
		m_UIList_MainButton[2]->m_isShow = true;
		m_UIList_MainButton[3]->m_isShow = true;
	}
	else if (gameProcess->m_GameState == eGameState::CREDIT)
	{
		m_UIList_Main[0]->m_isShow = false;
		m_UIList_Main[1]->m_isShow = true;
		m_UIList_MainButton[0]->m_isShow = false;
		m_UIList_MainButton[1]->m_isShow = false;
		m_UIList_MainButton[2]->m_isShow = false;
		m_UIList_MainButton[3]->m_isShow = false;
	}

	for (unsigned int i = 0; i < m_UIList_MainButton.size(); i++)
	{
		if (m_UIList_MainButton[i]->m_isShow == false)
			continue;

		UI* nowUI = m_UIList_MainButton[i];

		// UI버튼 영역 계산
		RECT rc = { (LONG)nowUI->m_vPos.x, (LONG)nowUI->m_vPos.y,
					(LONG)(nowUI->m_vPos.x + nowUI->m_Sprite->m_Width),
					(LONG)(nowUI->m_vPos.y + nowUI->m_Sprite->m_Height) };

		if (PtInRect(&rc, pt))
		{
			///*
			switch (g_MouseState)
			{
				default:
				case MS_STANDBY:
				{
					m_BtnState_Main[i] = BS_CURSORON;
				}
				break;
			}
			//*/
			if (KeyInput::GetIns()->GetIsLButtonUpJust() == true)
			{
				m_SoundManager->StopSoundChannel(SFX);
				m_SoundManager->PlaySound_Enum(eSoundType::UI_Btn);
				ButtonAction_Main(i);
			}
		}
		else
		{
			m_BtnState_Main[i] = BS_STANDBY;
		}
	}
}

void UIManager::UIUpdateAll_HowTo(POINT pt)
{
	for (unsigned int i = 0; i < m_UIList_HowToButton.size(); i++)
	{
		UI* nowUI = m_UIList_HowToButton[i];

		// UI버튼 영역 계산
		RECT rc = { (LONG)nowUI->m_vPos.x, (LONG)nowUI->m_vPos.y,
					(LONG)(nowUI->m_vPos.x + nowUI->m_Sprite->m_Width),
					(LONG)(nowUI->m_vPos.y + nowUI->m_Sprite->m_Height) };

		if (PtInRect(&rc, pt))
		{
			///*
			switch (g_MouseState)
			{
				default:
				case MS_STANDBY:
				{
					m_BtnState_HowTo[i] = BS_CURSORON;
				}
				break;
			}
			//*/
			if (KeyInput::GetIns()->GetIsLButtonUpJust() == true)
			{
				m_SoundManager->StopSoundChannel(SFX);
				m_SoundManager->PlaySound_Enum(eSoundType::UI_Btn);
				ButtonAction_HowTo(i);
			}
		}
		else
		{
			m_BtnState_HowTo[i] = BS_STANDBY;
		}
	}
}

void UIManager::UIUpdateAll_LeaderSelect(POINT pt)
{
	for (unsigned int i = 0; i < m_UIList_LeaderSelectButton.size(); i++)
	{
		UI* nowUI = m_UIList_LeaderSelectButton[i];

		// UI버튼 영역 계산
		RECT rc = { (LONG)nowUI->m_vPos.x, (LONG)nowUI->m_vPos.y,
					(LONG)(nowUI->m_vPos.x + nowUI->m_Sprite->m_Width),
					(LONG)(nowUI->m_vPos.y + nowUI->m_Sprite->m_Height) };

		if (PtInRect(&rc, pt))
		{
			///*
			switch (g_MouseState)
			{
				default:
				case MS_STANDBY:
				{
					m_BtnState_LeaderSelect[i] = BS_CURSORON;
				}
				break;
			}
			//*/
			if (KeyInput::GetIns()->GetIsLButtonUpJust() == true)
			{
				m_SoundManager->StopSoundChannel(SFX);
				m_SoundManager->PlaySound_Enum(eSoundType::UI_Btn);
				ButtonAction_LeaderSelect(i);
			}
		}
		else
		{
			m_BtnState_LeaderSelect[i] = BS_STANDBY;
		}
	}
}

void UIManager::UIUpdateAll_InGame()
{
	// 툴팁 초기화
	for (int i = 27; i < 33; i++)
	{
		m_UIList_InGame[i]->m_isShow = false;
	}

	UIUpdate_InGameTurn();
	UIUpdate_InGameCastling();
	UIUpdate_InGameLocked();

	UIUpdate_InGameWarning();

	UIUpdate_InGameTimer();
}

void UIManager::UIUpdate_InGameTurn()
{
	if (myData.OwnPlayer == checkTurn)
	{
		// 턴 관련 UI 활성화
		m_UIList_InGameButton[18]->m_isShow = true;
		m_UIList_InGame[33]->m_isShow = true;
		m_UIList_InGame[34]->m_isShow = false;
		m_UIList_InGame[35]->m_isShow = false;

		m_UIList_InGame[36]->m_isShow = false;
		m_UIList_InGame[23]->m_isShow = true;
	}
	else
	{
		// 턴 관련 UI 비활성화
		m_UIList_InGameButton[18]->m_isShow = false;
		m_UIList_InGame[33]->m_isShow = false;
		m_UIList_InGame[34]->m_isShow = true;
		m_UIList_InGame[35]->m_isShow = true;

		m_UIList_InGame[36]->m_isShow = true;
		m_UIList_InGame[23]->m_isShow = false;
	}
}

void UIManager::UIUpdate_InGameCastling()
{
	// 캐슬링 경고
	if (m_isMyCastling)
	{
		m_UIList_InGame[24]->m_isShow = true;
	}
	else
	{
		m_UIList_InGame[24]->m_isShow = false;
	}
	// 캐슬링 경고
	if (m_isEnemyCastling)
	{
		m_UIList_InGame[25]->m_isShow = true;
	}
	else
	{
		m_UIList_InGame[25]->m_isShow = false;
	}
}

void UIManager::UIUpdate_InGameWarning()
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	if (gameProcess->m_PPWarning == true)
	{
		gameProcess->m_PPWarning = false;
		m_SoundManager->StopSoundChannel(SFX);
		m_SoundManager->PlaySound_Enum(eSoundType::UI_CancelBtn);
	}

	if (gameProcess->m_APWarning == true)
	{
		gameProcess->m_APWarning = false;
		m_SoundManager->StopSoundChannel(SFX);
		m_SoundManager->PlaySound_Enum(eSoundType::UI_CancelBtn);
	}
}

void UIManager::UIUpdate_InGameLocked()
{
	// 자원 수에 따른 잠금 UI 활성화
	int wheat = myData.GetResource(Resource::Type::WHEAT);
	int iron = myData.GetResource(Resource::Type::IRON);
	int horse = myData.GetResource(Resource::Type::HORSE);
	int coin = myData.GetResource(Resource::Type::COIN);

	// 성 증축
	if (wheat < 2 || horse < 2)
	{
		m_UIList_InGameLocked[0]->m_isShow = true;
		m_UIList_InGameButton[0]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[0]->m_isShow = false;
		m_UIList_InGameButton[0]->m_isLocked = false;
	}

	// 성 증축
	if (coin < 6)
	{
		m_UIList_InGameLocked[1]->m_isShow = true;
		m_UIList_InGameButton[1]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[1]->m_isShow = false;
		m_UIList_InGameButton[1]->m_isLocked = false;
	}

	// 농부 생산
	if (wheat < 3)
	{
		m_UIList_InGameLocked[2]->m_isShow = true;
		m_UIList_InGameButton[3]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[2]->m_isShow = false;
		m_UIList_InGameButton[3]->m_isLocked = false;
	}

	// 농부, 검사 생산
	if (coin < 3)
	{
		m_UIList_InGameLocked[3]->m_isShow = true;
		m_UIList_InGameButton[4]->m_isLocked = true;

		m_UIList_InGameLocked[5]->m_isShow = true;
		m_UIList_InGameButton[7]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[3]->m_isShow = false;
		m_UIList_InGameButton[4]->m_isLocked = false;

		m_UIList_InGameLocked[5]->m_isShow = false;
		m_UIList_InGameButton[7]->m_isLocked = false;
	}

	// 검사 생산
	if (wheat < 2 || iron < 1)
	{
		m_UIList_InGameLocked[4]->m_isShow = true;
		m_UIList_InGameButton[6]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[4]->m_isShow = false;
		m_UIList_InGameButton[6]->m_isLocked = false;
	}

	// 궁수 생산
	if (wheat < 2 || iron < 2)
	{
		m_UIList_InGameLocked[6]->m_isShow = true;
		m_UIList_InGameButton[9]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[6]->m_isShow = false;
		m_UIList_InGameButton[9]->m_isLocked = false;
	}

	// 궁수, 방패 생산
	if (coin < 4)
	{
		m_UIList_InGameLocked[7]->m_isShow = true;
		m_UIList_InGameButton[10]->m_isLocked = true;

		m_UIList_InGameLocked[9]->m_isShow = true;
		m_UIList_InGameButton[13]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[7]->m_isShow = false;
		m_UIList_InGameButton[10]->m_isLocked = false;

		m_UIList_InGameLocked[9]->m_isShow = false;
		m_UIList_InGameButton[13]->m_isLocked = false;
	}

	// 방패 생산
	if (wheat < 1 || iron < 3)
	{
		m_UIList_InGameLocked[8]->m_isShow = true;
		m_UIList_InGameButton[12]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[8]->m_isShow = false;
		m_UIList_InGameButton[12]->m_isLocked = false;
	}

	// 기사 생산
	if (wheat < 3 || horse < 1)
	{
		m_UIList_InGameLocked[10]->m_isShow = true;
		m_UIList_InGameButton[15]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[10]->m_isShow = false;
		m_UIList_InGameButton[15]->m_isLocked = false;
	}

	if (coin < 5)
	{
		m_UIList_InGameLocked[11]->m_isShow = true;
		m_UIList_InGameButton[16]->m_isLocked = true;
	}
	else
	{
		m_UIList_InGameLocked[11]->m_isShow = false;
		m_UIList_InGameButton[16]->m_isLocked = false;
	}
}

void UIManager::UIUpdateAll_InGame_Option()
{
	if (m_isExitButton)
	{
		m_UIList_InGame_Option[1]->m_isShow = true;
		m_UIList_InGame_OptionButton[0]->m_isShow = false;
		m_UIList_InGame_OptionButton[1]->m_isShow = false;
		m_UIList_InGame_OptionButton[2]->m_isShow = false;
		m_UIList_InGame_OptionButton[3]->m_isShow = true;
		m_UIList_InGame_OptionButton[4]->m_isShow = true;
	}
	else
	{
		m_UIList_InGame_Option[1]->m_isShow = false;
		m_UIList_InGame_OptionButton[0]->m_isShow = true;
		m_UIList_InGame_OptionButton[1]->m_isShow = true;
		m_UIList_InGame_OptionButton[2]->m_isShow = true;
		m_UIList_InGame_OptionButton[3]->m_isShow = false;
		m_UIList_InGame_OptionButton[4]->m_isShow = false;
	}
}

void UIManager::UIUpdateAll_InGame_OptionButton(POINT pt)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	for (unsigned int i = 0; i < m_UIList_InGame_OptionButton.size(); i++)
	{
		UI* nowUI = m_UIList_InGame_OptionButton[i];

		// UI버튼 영역 계산
		RECT rc = { (LONG)nowUI->m_vPos.x, (LONG)nowUI->m_vPos.y,
					(LONG)(nowUI->m_vPos.x + nowUI->m_Sprite->m_Width),
					(LONG)(nowUI->m_vPos.y + nowUI->m_Sprite->m_Height) };

		if (m_UIList_InGame_OptionButton[i]->m_isShow == true)
		{
			if (gameProcess->m_pressTurnBtn == true)
			{
				continue;
			}

			if (PtInRect(&rc, pt))
			{
				///*
				switch (g_MouseState)
				{
					default:
					case MS_STANDBY:
					{
						m_BtnState_InGame_Option[i] = BS_CURSORON;
					}
					break;
				}

				if (KeyInput::GetIns()->GetIsLButtonUpJust() == true)
				{
					m_SoundManager->StopSoundChannel(SFX);
					m_SoundManager->PlaySound_Enum(eSoundType::UI_Btn);
					ButtonAction_Option(i);
				}
			}
			else
			{
				m_BtnState_InGame_Option[i] = BS_STANDBY;
			}
		}
	}
}

void UIManager::UIUpdate_InGameTimer()
{
	for (unsigned int i = 0; i < m_UIList_InGameTimer.size(); i++)
	{
		m_UIList_InGameTimer[i]->m_isShow = false;
	}

	int nowTime = static_cast<int>(timeData.count() * 0.001f);

	int timeTens = nowTime / 10;
	int timeUnits = nowTime % 10;

	if (timeTens >= 0 && timeUnits >= 0)
	{
		m_UIList_InGameTimer[timeTens + 10]->m_isShow = true;
		m_UIList_InGameTimer[timeUnits]->m_isShow = true;
	}
}

void UIManager::UIUpdateAll_InGameButton(POINT pt)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	for (unsigned int i = 0; i < m_UIList_InGameButton.size(); i++)
	{
		UI* nowUI = m_UIList_InGameButton[i];

		// UI버튼 영역 계산
		RECT rc = { (LONG)nowUI->m_vPos.x, (LONG)nowUI->m_vPos.y,
					(LONG)(nowUI->m_vPos.x + nowUI->m_Sprite->m_Width),
					(LONG)(nowUI->m_vPos.y + nowUI->m_Sprite->m_Height) };

		if (m_UIList_InGameButton[i]->m_isShow == true)
		{
			if (gameProcess->m_pressTurnBtn == true)
			{
				continue;
			}

			if (PtInRect(&rc, pt))
			{
				///*
				switch (g_MouseState)
				{
					default:
					case MS_STANDBY:
					{
						m_BtnState_InGame[i] = BS_CURSORON;
						ToolTipAction(i);
					}
					break;
				}
				//*/
				if (myData.OwnPlayer == checkTurn)
				{
					if (KeyInput::GetIns()->GetIsLButtonUpJust() == true)
					{
						if (m_UIList_InGameButton[i]->m_isLocked == false)
						{
							m_SoundManager->StopSoundChannel(SFX);
							m_SoundManager->PlaySound_Enum(eSoundType::UI_Btn);
							ButtonAction_InGame(i);
						}
					}
				}
				else
				{
					if (KeyInput::GetIns()->GetIsLButtonUpJust() == true)
					{
						m_SoundManager->StopSoundChannel(SFX);
						m_SoundManager->PlaySound_Enum(eSoundType::UI_CancelBtn);
					}
				}
			}
			else
			{
				m_BtnState_InGame[i] = BS_STANDBY;
			}
		}
	}
}

void UIManager::ToolTipAction(int i)
{
	switch (i)
	{
		case 2:
		{
			m_UIList_InGame[27]->m_isShow = true;
		}
		break;
		case 5:
		{
			m_UIList_InGame[28]->m_isShow = true;
		}
		break;
		case 8:
		{
			m_UIList_InGame[29]->m_isShow = true;
		}
		break;
		case 11:
		{
			m_UIList_InGame[30]->m_isShow = true;
		}
		break;
		case 14:
		{
			m_UIList_InGame[31]->m_isShow = true;
		}
		break;
		case 17:
		{
			m_UIList_InGame[32]->m_isShow = true;
		}
		break;
		default:
			break;
	}
}

void UIManager::UIUpdateAll_Result(POINT pt)
{
	if (gameResult == -1)	// 패배
	{
		m_UIList_Result[0]->m_isShow = false;
		m_UIList_ResultButton[0]->m_isShow = false;
		m_UIList_Result[1]->m_isShow = true;
		m_UIList_ResultButton[1]->m_isShow = true;
	}
	else if (gameResult == 1)	//승리
	{
		m_UIList_Result[0]->m_isShow = true;
		m_UIList_ResultButton[0]->m_isShow = true;
		m_UIList_Result[1]->m_isShow = false;
		m_UIList_ResultButton[1]->m_isShow = false;
	}

	for (unsigned int i = 0; i < m_UIList_ResultButton.size(); i++)
	{
		UI* nowUI = m_UIList_ResultButton[i];

		// UI버튼 영역 계산
		RECT rc = { (LONG)nowUI->m_vPos.x, (LONG)nowUI->m_vPos.y,
					(LONG)(nowUI->m_vPos.x + nowUI->m_Sprite->m_Width),
					(LONG)(nowUI->m_vPos.y + nowUI->m_Sprite->m_Height) };

		if (m_UIList_ResultButton[i]->m_isShow == true)
		{
			if (PtInRect(&rc, pt))
			{
				///*
				switch (g_MouseState)
				{
					default:
					case MS_STANDBY:
					{
						m_BtnState_Result[i] = BS_CURSORON;
					}
					break;
				}
				//*/
				if (KeyInput::GetIns()->GetIsLButtonUpJust() == true)
				{
					//if (m_SoundManager)
					//{
					//	m_SoundManager->StopSoundChannel(SFX);
					//	m_SoundManager->PlaySound_Enum(eSoundType::UI_Btn);
					//}
					ButtonAction_Result(i);
				}
			}
			else
			{
				m_BtnState_Result[i] = BS_STANDBY;
			}
		}
	}
}

void UIManager::ButtonAction_Main(int i)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	switch (i)
	{
		case 0:
		{
			SendFunc(PROTOCOL::QUICK_MATCH_ATTEND);
		}
		break;
		case 1:
		{
			gameProcess->m_PrevGameState = gameProcess->m_GameState;
			gameProcess->m_GameState = eGameState::HOWTO;
		}
		break;
		case 2:
		{
			gameProcess->m_GameState = eGameState::CREDIT;
		}
		break;
		case 3:
		{
			PostQuitMessage(WM_QUIT);
		}
		break;
		default:
			break;
	}
}

void UIManager::ButtonAction_HowTo(int i)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	switch (i)
	{
		case 0:
		{
			m_nowHowToPage++;
			if (m_nowHowToPage >= maxHowToPage)
			{
				m_nowHowToPage = 0;
			}
			for (int i = 0; i < maxHowToPage; i++)
			{
				m_UIList_HowTo[i]->m_isShow = false;
			}
			m_UIList_HowTo[m_nowHowToPage]->m_isShow = true;
		}
		break;
		default:
			break;
	}
}

void UIManager::ButtonAction_LeaderSelect(int i)
{
	switch (i)
	{
		case 0:
		{
			if (myData.leader.type == Leader::Type::NONE)
			{
				m_UIList_LeaderSelect[2]->m_isShow = true;
				m_UIList_LeaderSelect[3]->m_isShow = false;
				m_UIList_LeaderSelect[4]->m_isShow = false;
			}
		}
		break;
		case 1:
		{
			if (myData.leader.type == Leader::Type::NONE)
			{
				m_UIList_LeaderSelect[2]->m_isShow = false;
				m_UIList_LeaderSelect[3]->m_isShow = true;
				m_UIList_LeaderSelect[4]->m_isShow = false;
			}
		}
		break;
		case 2:
		{
			if (myData.leader.type == Leader::Type::NONE)
			{
				m_UIList_LeaderSelect[2]->m_isShow = false;
				m_UIList_LeaderSelect[3]->m_isShow = false;
				m_UIList_LeaderSelect[4]->m_isShow = true;
			}
		}
		break;
		case 3:
		{
			if (myData.leader.type == Leader::Type::NONE)
			{
				if (m_UIList_LeaderSelect[2]->m_isShow == true)
				{
					myData.leader.type = Leader::Type::DEMETER;
					SendFunc(PROTOCOL::MATCH_SELECT_LEADER);
				}
				else if (m_UIList_LeaderSelect[3]->m_isShow == true)
				{
					myData.leader.type = Leader::Type::ARES;
					SendFunc(PROTOCOL::MATCH_SELECT_LEADER);
				}
				else if (m_UIList_LeaderSelect[4]->m_isShow == true)
				{
					myData.leader.type = Leader::Type::HADES;
					SendFunc(PROTOCOL::MATCH_SELECT_LEADER);
				}
			}
		}
		break;
		default:
			break;
	}
}

void UIManager::ButtonAction_Option(int i)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();
	switch (i)
	{
		case 0:
		{
			// 계속하기
			gameProcess->m_GameState = eGameState::INGAME;
		}
		break;
		case 1:
		{
			// 게임설명
			gameProcess->m_PrevGameState = gameProcess->m_GameState;
			gameProcess->m_GameState = eGameState::HOWTO;
		}
		break;
		case 2:
		{
			// 게임종료
			m_isExitButton = true;
		}
		break;
		case 3:
		{
			// 네
			gameProcess->m_InGameState = eInGameState::NONE;
			m_SoundManager->StopSoundChannel(BGM);
			m_SoundManager->StopSoundChannel(SFX);
			SendFunc(PROTOCOL::ACTION_GIVE_UP);
		}
		break;
		case 4:
		{
			// 아니오
			m_isExitButton = false;
		}
		break;
		default:
			break;
	}
}

void UIManager::ButtonAction_InGame(int i)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();
	ObjectManager* objManager = gameProcess->m_ObjManager;

	if (gameProcess->m_InGameState != eInGameState::ACTION_UNIT_ANIMATION_ATTACK
		&& gameProcess->m_InGameState != eInGameState::ACTION_UNIT_ANIMATION_DIE)
	{
		switch (i)
		{
			case 0:
			{
				// 성 증축 버튼 (자원)
				isCoin = FALSE;
				SendFunc(PROTOCOL::ACTION_CASTLE_UPGRADE);
			}
			break;
			case 1:
			{
				// 성 증축 버튼 (금)
				isCoin = TRUE;
				SendFunc(PROTOCOL::ACTION_CASTLE_UPGRADE);
			}
			break;
			case 2:
			{
				// 성 증축 툴팁 버튼 - 작동X
			}
			break;
			case 3:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 일꾼 생산 버튼 (자원)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::SCV;
				gameProcess->m_isCoin = false;
			}
			break;
			case 4:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 일꾼 생산 버튼 (금)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::SCV;
				gameProcess->m_isCoin = true;
			}
			break;
			case 5:
			{
				// 일꾼 툴팁 버튼 - 작동X
			}
			break;
			case 6:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 검사 생산 버튼 (자원)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::SWORD;
				gameProcess->m_isCoin = false;
			}
			break;
			case 7:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 검사 생산 버튼 (금)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::SWORD;
				gameProcess->m_isCoin = true;
			}
			break;
			case 8:
			{
				// 검사 툴팁 버튼 - 작동X
			}
			break;
			case 9:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 궁수 생산 버튼 (자원)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::BOW;
				gameProcess->m_isCoin = false;
			}
			break;
			case 10:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 궁수 생산 버튼 (금)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::BOW;
				gameProcess->m_isCoin = true;
			}
			break;
			case 11:
			{
				// 궁수 툴팁 버튼 - 작동X
			}
			break;
			case 12:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 방패 생산 버튼 (자원)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::SHIELD;
				gameProcess->m_isCoin = false;
			}
			break;
			case 13:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 방패 생산 버튼 (금)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::SHIELD;
				gameProcess->m_isCoin = true;
			}
			break;
			case 14:
			{
				// 방패 툴팁 버튼 - 작동X
			}
			break;
			case 15:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 기사 생산 버튼 (자원)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::KNIGHT;
				gameProcess->m_isCoin = false;
			}
			break;
			case 16:
			{
				if (myData.PP <= 0)
					gameProcess->m_PPWarning = true;

				// 기사 생산 버튼 (금)
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE_REQUEST;
				gameProcess->m_CreateUnit = Piece::Type::KNIGHT;
				gameProcess->m_isCoin = true;
			}
			break;
			case 17:
			{
				// 기사 툴팁 버튼 - 작동X
			}
			break;
			case 18:
			{
				// 턴 변경 버튼
				if (gameProcess->m_pressTurnBtn == false)
				{
					gameProcess->m_pressTurnBtn = true;
					objManager->ResetPicking();
					objManager->ResetCreatePicking();
					objManager->Unselect();
					objManager->SetTilePickingState(gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, FALSE);
					SendFunc(PROTOCOL::ACTION_TURN_REQ);
				}
			}
			break;
			default:
				break;
		}
	}
}

void UIManager::ButtonAction_Result(int i)
{
	switch (i)
	{
		case 0:
		case 1:
		{
			///m_SoundManager->StopSoundChannel(SFX);
			g_captionClose = true;
			PostQuitMessage(WM_DESTROY);
		}
		break;
	}
}

void UIManager::UIDrawAll_Main()
{
	for (unsigned int i = 0; i < m_UIList_Main.size(); i++)
	{
		if (m_UIList_Main[i]->m_isShow == true)
		{
			m_UIList_Main[i]->UIDrawAB();
		}
	}
	for (unsigned int i = 0; i < m_UIList_MainButton.size(); i++)
	{
		if (m_UIList_MainButton[i]->m_isShow == true)
		{
			switch (m_BtnState_Main[i])
			{
				case BS_STANDBY:
				{
					m_UIList_MainButton[i]->UIDrawAB();
				}
				break;
				case BS_CURSORON:
				{
					m_UIList_MainButton[i]->UIDrawAB();
					m_UIList_MainButton[i]->UIDrawFX();
				}
				break;
				case BS_PUSH:
				{
					m_UIList_MainButton[i]->UIDrawAB();
					m_BtnState_Main[i] = BS_STANDBY;
				}
				break;
				default:
					break;
			}
		}
	}
}

void UIManager::UIDrawAll_HowTo()
{
	for (unsigned int i = 0; i < m_UIList_HowTo.size(); i++)
	{
		if (m_UIList_HowTo[i]->m_isShow == true)
		{
			m_UIList_HowTo[i]->UIDrawAB();
		}
	}
	for (unsigned int i = 0; i < m_UIList_HowToButton.size(); i++)
	{
		if (m_UIList_HowToButton[i]->m_isShow == true)
		{
			switch (m_BtnState_HowTo[i])
			{
				case BS_STANDBY:
				{
					m_UIList_HowToButton[i]->UIDrawAB();
				}
				break;
				case BS_CURSORON:
				{
					m_UIList_HowToButton[i]->UIDrawAB();
					m_UIList_HowToButton[i]->UIDrawFX();
				}
				break;
				case BS_PUSH:
				{
					m_UIList_HowToButton[i]->UIDrawAB();
					m_BtnState_HowTo[i] = BS_STANDBY;
				}
				break;
				default:
					break;
			}
		}
	}
}

void UIManager::UIDrawAll_Matching()
{
	for (unsigned int i = 0; i < m_UIList_Matching.size(); i++)
	{
		static DWORD LastTime = timeGetTime();

		DWORD NowTime = timeGetTime();

		if (LastTime + 900 <= NowTime)
		{
			m_isEffectOn ^= TRUE;
			LastTime = timeGetTime();
		}
		if (i == 1)
		{
			if (m_isEffectOn == TRUE)
			{
				m_UIList_Matching[i]->UIDrawAB();
			}
		}
		else
		{
			m_UIList_Matching[i]->UIDrawAB();
		}

	}
	for (unsigned int i = 0; i < m_UIList_MatchingButton.size(); i++)
	{
		m_UIList_MatchingButton[i]->UIDrawAB();
	}
}

void UIManager::UIDrawAll_LeaderSelect()
{
	if (m_UIList_LeaderSelect[0]->m_isShow == true)
	{
		m_UIList_LeaderSelect[0]->UIDrawAB();
	}

	for (unsigned int i = 0; i < m_UIList_LeaderSelectButton.size(); i++)
	{
		switch (m_BtnState_LeaderSelect[i])
		{
			case BS_STANDBY:
			{
				m_UIList_LeaderSelectButton[i]->UIDrawAB();
			}
			break;
			case BS_CURSORON:
			{
				/// 이건 스타트 버튼
				if (i == 3)
				{
					m_UIList_LeaderSelectButton[i]->UIDrawAB();
					m_UIList_LeaderSelectButton[i]->UIDrawFX();
				}
				else
				{
					m_UIList_LeaderSelectButton[i]->UIDrawAB();
				}
			}
			break;
			case BS_PUSH:
			{
				m_UIList_LeaderSelectButton[i]->UIDrawAB();
				m_BtnState_LeaderSelect[i] = BS_STANDBY;
			}
			break;
			default:
				break;
		}
	}
	for (unsigned int i = 1; i < m_UIList_LeaderSelect.size(); i++)
	{
		if (m_UIList_LeaderSelect[i]->m_isShow == true)
		{
			m_UIList_LeaderSelect[i]->UIDrawAB();
		}
	}

	if (m_UIList_LeaderSelectButton[3]->m_isShow == true)
	{
		m_UIList_LeaderSelectButton[3]->UIDrawAB();
	}
}

void UIManager::UIDrawAll_Option()
{
	for (unsigned int i = 0; i < m_UIList_InGame_Option.size(); i++)
	{
		if (m_UIList_InGame_Option[i]->m_isShow == true)
		{
			m_UIList_InGame_Option[i]->UIDrawAB();
		}
	}
	for (unsigned int i = 0; i < m_UIList_InGame_OptionButton.size(); i++)
	{
		if (m_UIList_InGame_OptionButton[i]->m_isShow == true)
		{
			switch (m_BtnState_InGame_Option[i])
			{
				case BS_STANDBY:
				{
					m_UIList_InGame_OptionButton[i]->UIDrawAB();
				}
				break;
				case BS_CURSORON:
				{
					m_UIList_InGame_OptionButton[i]->UIDrawAB();
					m_UIList_InGame_OptionButton[i]->UIDrawFX();

				}
				break;
				case BS_PUSH:
				{
					m_UIList_InGame_OptionButton[i]->UIDrawAB();
					m_BtnState_InGame_Option[i] = BS_STANDBY;
				}
				break;
				default:
					break;
			}
		}
	}
}

void UIManager::UIDrawAll_InGame()
{
	for (unsigned int i = 0; i < m_UIList_InGame.size(); i++)
	{
		if (m_UIList_InGame[i]->m_isShow == true)
		{
			m_UIList_InGame[i]->UIDrawAB();
		}
	}

	for (unsigned int i = 0; i < m_UIList_InGameButton.size(); i++)
	{
		if (m_UIList_InGameButton[i]->m_isShow == true)
		{
			switch (m_BtnState_InGame[i])
			{
				case BS_STANDBY:
				{
					m_UIList_InGameButton[i]->UIDrawAB();
				}
				break;
				case BS_CURSORON:
				{
					m_UIList_InGameButton[i]->UIDrawAB();
					if (m_UIList_InGameButton[i]->m_isLocked == false)
					{
						m_UIList_InGameButton[i]->UIDrawFX();
					}
				}
				break;
				case BS_PUSH:
				{
					m_UIList_InGameButton[i]->UIDrawAB();
					m_BtnState_InGame[i] = BS_STANDBY;
				}
				break;
				default:
					break;
			}
		}
	}

	for (unsigned int i = 0; i < m_UIList_InGameLocked.size(); i++)
	{
		if (m_UIList_InGameLocked[i]->m_isShow == true)
		{
			m_UIList_InGameLocked[i]->UIDrawAB();
		}
	}

	if (m_UIList_InGameButton[18]->m_isShow == true)
	{
		m_UIList_InGameButton[18]->UIDrawAB();
	}

	UIDraw_Timer();
}

void UIManager::UIDraw_Timer()
{
	for (unsigned int i = 0; i < m_UIList_InGameTimer.size(); i++)
	{
		if (m_UIList_InGameTimer[i]->m_isShow == true)
		{
			m_UIList_InGameTimer[i]->UIDrawAB();
		}
	}
}

void UIManager::UIDrawAll_Result()
{
	for (unsigned int i = 0; i < m_UIList_Result.size(); i++)
	{
		if (m_UIList_Result[i]->m_isShow == true)
		{
			m_UIList_Result[i]->UIDrawAB();
		}
	}
	for (unsigned int i = 0; i < m_UIList_ResultButton.size(); i++)
	{
		if (m_UIList_ResultButton[i]->m_isShow == true)
		{
			switch (m_BtnState_Result[i])
			{
				case BS_STANDBY:
				{
					m_UIList_ResultButton[i]->UIDrawAB();
				}
				break;
				case BS_CURSORON:
				{
					m_UIList_ResultButton[i]->UIDrawAB();
					m_UIList_ResultButton[i]->UIDrawFX();
				}
				break;
				case BS_PUSH:
				{
					m_UIList_ResultButton[i]->UIDrawAB();
					m_BtnState_Result[i] = BS_STANDBY;
				}
				break;
				default:
					break;
			}
		}
	}
}

void UIManager::DebugBtnState(int x, int y)
{
	for (unsigned int i = 0; i < m_UIList_LeaderSelectButton.size(); i++)
	{
		switch (m_BtnState_LeaderSelect[i])
		{
			case BS_CURSORON:
				D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"BtnState : %s / BS_CURSORON", m_UIList_LeaderSelectButton[i]->m_UIName);
				break;
			case BS_PUSH:
				D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"BtnState : %s / BS_PUSH", m_UIList_LeaderSelectButton[i]->m_UIName);
				break;
			case BS_STANDBY:
				D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"BtnState : %s / BS_STANDBY", m_UIList_LeaderSelectButton[i]->m_UIName);
				break;
		}
	}

	for (unsigned int i = 0; i < m_UIList_InGameButton.size(); i++)
	{
		switch (m_BtnState_InGame[i])
		{
			case BS_CURSORON:
				D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"BtnState : %s / BS_CURSORON", m_UIList_InGameButton[i]->m_UIName);
				break;
			case BS_PUSH:
				D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"BtnState : %s / BS_PUSH", m_UIList_InGameButton[i]->m_UIName);
				break;
			case BS_STANDBY:
				D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"BtnState : %s / BS_STANDBY", m_UIList_InGameButton[i]->m_UIName);
				break;
		}
	}
}