#include "CGameProcess.h"
#include "NetworkCallback.h"

CGameProcess* CGameProcess::m_GameProcessInstance = nullptr;

CGameProcess::CGameProcess()
{
	m_D3DEngine = nullptr;
	m_KeyInput = nullptr;
	m_ModelLoadManager = nullptr;
	m_ObjManager = nullptr;
	m_UIManager = nullptr;
	m_SoundManager = nullptr;

	m_SkyBox = nullptr;

	m_pressTurnBtn = false;

	m_PrevGameState = eGameState::NONE;
	m_GameState = eGameState::TITLE;
	m_InGameState = eInGameState::NONE;
	m_PickingState = ePickingState::STANDBY;

	m_StartInGameBGM = false;
	m_ResultInGameBGM = false;

	m_CreateUnit = Piece::Type::NONE;
	m_isCoin = false;

	// 서버에서 받는 데이터로 바꿔야함
	m_MatchUser = eMatchUser::P1;

	m_MapStartPosX = -1;
	m_MapStartPosY = -1;

	m_MapEndPosX = -1;
	m_MapEndPosY = -1;

	m_CreatePosX = -1;
	m_CreatePosY = -1;

	m_CastleLevel_1P = 0;
	m_CastleLevel_2P = 0;

	m_bDebugShow = false;

	m_CamInitalize = true;

	///m_TempTypeData = Piece::Type::NONE;

	m_SettingLeader = false;

	m_CastlingWarning = false;

	m_PPWarning = false;
	m_APWarning = false;
}

CGameProcess::~CGameProcess()
{

}

void CGameProcess::Create(HINSTANCE hInst, HWND hWnd, int screenWidth, int screenHeight)
{
	m_D3DEngine = D3DEngine::GetIns();

	m_D3DEngine->DXSetUp(hWnd);

	m_KeyInput = KeyInput::GetIns();

	// Lua 시작

	// 오브젝트 로딩 (ASE)
	m_ModelLoadManager = ModelLoadManager::GetIns();
	m_ModelLoadManager->AllModelLoad();

	m_ObjManager = new ObjectManager();
	m_ObjManager->AddEtcGameObject(eObjectType::BACKGROUNDROCK, VECTOR3(0, -1, 0), VECTOR3(0, 0, 0), VECTOR3(1, 1, 1));

	m_UIManager = new UIManager();
	m_UIManager->UICreateAll();

	m_SoundManager = SoundManager::GetIns();
	m_SoundManager->Initialize();
	m_SoundManager->PlaySound_Enum(eSoundType::TitleBGM);

	m_SkyBox = new SkyBox();

	Piece::CreatePieceMoveAble();
}

extern DWORD g_MouseState;

void CGameProcess::KeyInputUpdate(HWND hWnd)
{
	//===================================================
	// 입력
	//===================================================
	m_KeyInput->GetMousePos(hWnd);
	m_KeyInput->Input();

	// UI 버튼 체크
	switch (m_GameState)
	{
		case eGameState::TITLE:
		{
			///if (GetAsyncKeyState(VK_RETURN) & 0x0001)
			///{
			///	SendFunc(PROTOCOL::QUICK_MATCH_ATTEND);
			///}
			m_UIManager->UIUpdateAll_Main(m_KeyInput->m_MousePos);
		}
		break;
		case eGameState::HOWTO:
		{
			if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
			{
				m_GameState = m_PrevGameState;
			}
			m_UIManager->UIUpdateAll_HowTo(m_KeyInput->m_MousePos);
		}
		break;
		case eGameState::CREDIT:
		{
			if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
			{
				m_GameState = eGameState::TITLE;
			}
			m_UIManager->UIUpdateAll_Main(m_KeyInput->m_MousePos);
		}
		break;
		case eGameState::MATCHING:
		{
			if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
			{
				SendFunc(PROTOCOL::QUICK_MATCH_CANCEL);
			}
		}
		break;
		case eGameState::LEADERSELECT:
		{
			if (m_KeyInput->GetIsLButtonUpJust() == true)
			{
				g_MouseState = MS_LBTNUP;
			}

			if (m_CamInitalize == false)
			{
				if (myData.OwnPlayer == eMatchUser::P1)
				{
					m_D3DEngine->m_Camera->Eye = Vector3(0, 7, -8);
					m_D3DEngine->m_Camera->LookAt = Vector3(0, 0, -1.5f);
					m_CamInitalize = true;
				}
				else if (myData.OwnPlayer == eMatchUser::P2)
				{
					m_D3DEngine->m_Camera->Eye = Vector3(0, 7, 8);
					m_D3DEngine->m_Camera->LookAt = Vector3(0, 0, 1.5f);
					m_CamInitalize = true;
				}
			}

			if (myData.leader.type == Leader::Type::NONE)
			{
				m_UIManager->UIUpdate_InGameTimer();
				m_UIManager->UIUpdateAll_LeaderSelect(m_KeyInput->m_MousePos);
			}
		}
		break;
		case eGameState::OPTION:
		{
			m_UIManager->UIUpdateAll_InGame_Option();
			m_UIManager->UIUpdateAll_InGame_OptionButton(m_KeyInput->m_MousePos);
		}
		break;
		case eGameState::INGAME:
		{
			if (IsKeyUp(VK_ESCAPE))
			{
				m_GameState = eGameState::OPTION;
			}

			if (myData.OwnPlayer == checkTurn)
			{
				// 마우스 버튼이 떨어진 시점 (딱 한번)
				if (m_KeyInput->GetIsLButtonUpJust() == true)
				{
					g_MouseState = MS_LBTNUP;

					// 카메라상의 레이를 만들어서, 평면과의 교차점을 구한다.
					Vector4 pickingPoint = m_KeyInput->ViewPortToWorld();

					if (m_InGameState != eInGameState::ACTION_UNIT_ANIMATION_ATTACK
						&& m_InGameState != eInGameState::ACTION_UNIT_ANIMATION_DIE)
					{
						// 피킹
						m_ObjManager->CheckPicking(pickingPoint);
					}
				}
			}
			m_UIManager->UIUpdateAll_InGame();
			m_UIManager->UIUpdateAll_InGameButton(m_KeyInput->m_MousePos);
		}
		break;
		case eGameState::RESULT:
		{
			if (IsKeyUp(VK_ESCAPE))	PostQuitMessage(WM_QUIT);

			m_UIManager->UIUpdateAll_Result(m_KeyInput->m_MousePos);
		}
		break;
	}

#ifdef _DEBUG
	if (GetAsyncKeyState(VK_F2) & 0x0001)
	{
		m_bDebugShow ^= true;
		//m_bDebugShow = !m_bDebugShow;

	}
#endif

}

void CGameProcess::Update()
{
	//===================================================
	// 갱신
	//===================================================
	// 엔진, 시스템 갱신
	float dTime = m_D3DEngine->Update();

	// 오브젝트들 갱신
	m_ObjManager->Update(dTime);

	// 엔진 갱신
	m_D3DEngine->SceneUpdate(dTime);

	switch (m_GameState)
	{
		case eGameState::TITLE:
		{

		}
		break;
		case eGameState::MATCHING:
		{

		}
		break;
		case eGameState::LEADERSELECT:
		{
			if (m_StartInGameBGM == false)
			{
				m_StartInGameBGM = true;
				m_SoundManager->StopSoundChannel(BGM);
				m_SoundManager->PlaySound_Enum(eSoundType::InGameBGM);
			}
		}
		break;
		case eGameState::OPTION:
		case eGameState::INGAME:
		{
			if (m_SettingLeader == false)
			{
				if (myData.leader.type != Leader::Type::NONE
					&& myData.leader.type != Leader::Type::MAX
					&& enemyData.leader.type != Leader::Type::NONE
					&& enemyData.leader.type != Leader::Type::MAX)
				{
					m_SettingLeader = true;
					m_UIManager->SetLeaderUI();
				}
			}

			// 인게임 프로세스 갱신
			GameUpdate(dTime);

			m_SkyBox->Update();
		}
		break;
		case eGameState::RESULT:
		{
			if (m_ResultInGameBGM == false)
			{
				m_ResultInGameBGM = true;
				m_SoundManager->StopSoundChannel(BGM);
				if (gameResult == -1)
				{
					m_SoundManager->PlaySound_Enum(eSoundType::UI_Lose);
				}
				else if (gameResult == 1)
				{
					m_SoundManager->PlaySound_Enum(eSoundType::UI_Win);
				}
			}
		}
		break;
	}

	Draw(dTime);
}

void CGameProcess::GameUpdate(float dTime)
{
	switch (m_InGameState)
	{
		case eInGameState::ACTION_UNIT_CREATE_REQUEST:
		{
			m_ObjManager->CalcCreateRange();
		}
		break;

		case eInGameState::ACTION_UNIT_CREATE:
		{
			m_ObjManager->AddOneUnitGameObject();
			m_InGameState = eInGameState::NONE;
		}
		break;

		case eInGameState::ACTION_CASTLE_UPGRADE:
		{
			if (castleUpgradeUser != eMatchUser::MAX)
			{
				if (castleUpgradeUser == eMatchUser::P1)
				{
					m_CastleLevel_1P++;
				}
				else if (castleUpgradeUser == eMatchUser::P2)
				{
					m_CastleLevel_2P++;
				}
			}
			m_InGameState = eInGameState::NONE;
		}
		break;

		case eInGameState::ACTION_UNIT_ANIMATION_ATTACK:
		{

		}
		break;

		case eInGameState::ACTION_UNIT_ANIMATION_DIE:
		{

		}
		break;

		case eInGameState::ACTION_UNIT_MOVE:
		{
			// 실제 이동(공격)
			POINT ptStart = POINT{ m_MapStartPosX, m_MapStartPosY };
			POINT ptEnd = POINT{ m_MapEndPosX, m_MapEndPosY };

			if (ptStart.x != ptEnd.x || ptStart.y != ptEnd.y)
			{
				m_SoundManager->StopSoundChannel(SFX);
				m_SoundManager->PlaySound_Enum(eSoundType::Sound_Eff_Move);
				m_ObjManager->UnitMove(ptStart, ptEnd);
			}

			m_InGameState = eInGameState::NONE;
		}
		break;
		default:
			break;
	}
}

void CGameProcess::Reset_Animation_Attack()
{
	///m_SoundManager->StopSoundChannel(SFX);
	///m_SoundManager->PlaySound_Enum(eSoundType::Sound_Eff_Die);
	m_InGameState = eInGameState::ACTION_UNIT_ANIMATION_DIE;
	m_ObjManager->Reset_Animation_Attack();
}

void CGameProcess::Reset_Animation_Die()
{
	m_InGameState = eInGameState::ACTION_UNIT_MOVE;
	m_ObjManager->Reset_Animation_Die();
}

void CGameProcess::Draw(float dTime)
{
	//===================================================
	// 출력
	//===================================================
	// 백버퍼, 깊이버퍼 지우기
	m_D3DEngine->SceneRenderStart();

	switch (m_GameState)
	{
		case eGameState::TITLE:
		case eGameState::CREDIT:
		{
			m_D3DEngine->Mode_2D();
			m_UIManager->UIDrawAll_Main();
			m_D3DEngine->Mode_3D();
		}
		break;
		case eGameState::HOWTO:
		{
			m_D3DEngine->Mode_2D();
			m_UIManager->UIDrawAll_HowTo();
			m_D3DEngine->Mode_3D();
		}
		break;
		case eGameState::MATCHING:
		{
			m_D3DEngine->Mode_2D();
			m_UIManager->UIDrawAll_Matching();
			m_D3DEngine->Mode_3D();
		}
		break;
		case eGameState::LEADERSELECT:
		{
			// 오브젝트 렌더링
			m_SkyBox->Draw();
			m_ObjManager->Draw(dTime);

			m_D3DEngine->Mode_2D();
			m_UIManager->UIDrawAll_LeaderSelect();
			m_UIManager->UIDraw_Timer();
			m_D3DEngine->Mode_3D();

			//TODO : 선택한 사람은 안보이게 처리
			XMFLOAT4 col = XMFLOAT4(1, 1, 1, 1);
			///m_D3DEngine->_TT(1700, 850, col, L"남은 시간 : %d", static_cast<int>(timeData.count() * 0.001f));
		}
		break;
		case eGameState::OPTION:
		{
			// 오브젝트 렌더링
			m_SkyBox->Draw();
			m_ObjManager->Draw(dTime);

			m_D3DEngine->Mode_2D();
			m_UIManager->UIDrawAll_InGame();
			m_UIManager->UIDrawAll_Option();
			m_D3DEngine->Mode_3D();

			XMFLOAT4 colWhite = XMFLOAT4(1, 1, 1, 1);
			XMFLOAT4 colBlack = XMFLOAT4(0, 0, 0, 1);

			m_D3DEngine->_TT(103, 1050, colWhite, L"%d", myData.PP);
			m_D3DEngine->_TT(214, 1050, colWhite, L"%d", myData.AP);
			m_D3DEngine->_TT(148, 964, colWhite, L"%d", myData.GetResource(Resource::Type::WHEAT));
			m_D3DEngine->_TT(184, 964, colWhite, L"%d", myData.GetResource(Resource::Type::IRON));
			m_D3DEngine->_TT(220, 964, colWhite, L"%d", myData.GetResource(Resource::Type::HORSE));
			m_D3DEngine->_TT(255, 964, colWhite, L"%d", myData.GetResource(Resource::Type::COIN));

			m_D3DEngine->_TT(1820, 203, colWhite, L"%d", enemyData.PP);
			m_D3DEngine->_TT(1882, 203, colWhite, L"%d", enemyData.AP);
			m_D3DEngine->_TT(1783, 171, colWhite, L"%d", enemyData.GetResource(Resource::Type::WHEAT));
			m_D3DEngine->_TT(1815, 171, colWhite, L"%d", enemyData.GetResource(Resource::Type::IRON));
			m_D3DEngine->_TT(1847, 171, colWhite, L"%d", enemyData.GetResource(Resource::Type::HORSE));
			m_D3DEngine->_TT(1880, 171, colWhite, L"%d", enemyData.GetResource(Resource::Type::COIN));

		}
		break;
		case eGameState::WAITING:
		case eGameState::INGAME:
		{
			// 오브젝트 렌더링
			m_SkyBox->Draw();
			m_ObjManager->Draw(dTime);

			m_D3DEngine->Mode_2D();
			m_UIManager->UIDrawAll_InGame();
			m_D3DEngine->Mode_3D();

			XMFLOAT4 colWhite = XMFLOAT4(1, 1, 1, 1);
			XMFLOAT4 colBlack = XMFLOAT4(0, 0, 0, 1);

			m_D3DEngine->_TT(103, 1050, colWhite, L"%d", myData.PP);
			m_D3DEngine->_TT(214, 1050, colWhite, L"%d", myData.AP);
			m_D3DEngine->_TT(148, 964, colWhite, L"%d", myData.GetResource(Resource::Type::WHEAT));
			m_D3DEngine->_TT(184, 964, colWhite, L"%d", myData.GetResource(Resource::Type::IRON));
			m_D3DEngine->_TT(220, 964, colWhite, L"%d", myData.GetResource(Resource::Type::HORSE));
			m_D3DEngine->_TT(255, 964, colWhite, L"%d", myData.GetResource(Resource::Type::COIN));

			m_D3DEngine->_TT(1820, 203, colWhite, L"%d", enemyData.PP);
			m_D3DEngine->_TT(1882, 203, colWhite, L"%d", enemyData.AP);
			m_D3DEngine->_TT(1783, 171, colWhite, L"%d", enemyData.GetResource(Resource::Type::WHEAT));
			m_D3DEngine->_TT(1815, 171, colWhite, L"%d", enemyData.GetResource(Resource::Type::IRON));
			m_D3DEngine->_TT(1847, 171, colWhite, L"%d", enemyData.GetResource(Resource::Type::HORSE));
			m_D3DEngine->_TT(1880, 171, colWhite, L"%d", enemyData.GetResource(Resource::Type::COIN));

#ifdef _DEBUG
			if (m_bDebugShow)
			{
				int y = 0;
				XMFLOAT4 col1 = XMFLOAT4(1, 1, 0, 1);
				m_D3DEngine->_TT(10, y += 14, col1, L"Picking Screen Pos ( %d : %d )", m_KeyInput->m_MousePos.x, m_KeyInput->m_MousePos.y);
				m_D3DEngine->_TT(10, y += 14, col1, L"MapStartPos : ( %d , %d )", m_MapStartPosX, m_MapStartPosY);
				m_D3DEngine->_TT(10, y += 14, col1, L"MapEndPos : ( %d , %d )", m_MapEndPosX, m_MapEndPosY);

				y += 14;
				XMFLOAT4 col2 = XMFLOAT4(1, 1, 0, 1);
				Camera* _cam = m_D3DEngine->m_Camera;
				m_D3DEngine->_TT(10, y += 14, col2, L"[Camera]");
				m_D3DEngine->_TT(10, y += 14, col2, L"Eye : ( %.2f , %.2f , %.2f )", _cam->Eye.x, _cam->Eye.y, _cam->Eye.z);
				m_D3DEngine->_TT(10, y += 14, col2, L"FOV : ( %.2f )", _cam->FOV);

				DebugInGameState(10, 200);
				DebugPickingState(10, 220);
				m_ObjManager->DebugShieldState(10, 300);
				m_UIManager->DebugBtnState(1600, 10);
			}
			else
			{
				m_D3DEngine->_TT(10, 10, XMFLOAT4(1, 1, 1, 1), L"Debug : F2");
			}
#endif
		}
		break;
		case eGameState::RESULT:
		{
			m_D3DEngine->Mode_2D();
			m_UIManager->UIDrawAll_Result();
			m_D3DEngine->Mode_3D();
		}
		break;
	}



	m_D3DEngine->SceneRenderEnd();
}

void CGameProcess::DebugInGameState(int x, int y)
{
	switch (m_InGameState)
	{
		case eInGameState::ACTION_UNIT_CREATE_REQUEST:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"eInGameState::ACTION_UNIT_CREATE_REQUEST");
		}
		break;
		case eInGameState::ACTION_UNIT_CREATE:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"eInGameState::ACTION_UNIT_CREATE");
		}
		break;
		case eInGameState::ACTION_CASTLE_UPGRADE:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"eInGameState::ACTION_CASTLE_UPGRADE");
		}
		break;
		case eInGameState::ACTION_UNIT_ANIMATION_ATTACK:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"eInGameState::ACTION_UNIT_ANIMATION");
		}
		break;
		case eInGameState::ACTION_UNIT_MOVE:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"eInGameState::ACTION_UNIT_MOVE");
		}
		break;
		default:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"eInGameState::NONE");
		}
		break;
	}
}

void CGameProcess::DebugPickingState(int x, int y)
{
	switch (m_PickingState)
	{
		case ePickingState::ON:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"ePickingState::ON");
		}
		break;
		case ePickingState::OFF:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"ePickingState::OFF");
		}
		break;
		case ePickingState::STANDBY:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"ePickingState::STANDBY");
		}
		break;
		default:
		{
			D3DEngine::GetIns()->_TT(x, y += 14, XMFLOAT4(1, 1, 1, 1), L"ePickingState::NONE");
		}
		break;
	}
}

void CGameProcess::OnClose()
{
	// 매니저들 제거
	m_ModelLoadManager->Release();
	m_SoundManager->Release();
	m_KeyInput->Release();

	delete m_ObjManager;
	delete m_UIManager;
	delete m_SkyBox;

	// Lua 종료

	// 엔진 관련 제거
	m_D3DEngine->DataRelease();
	m_D3DEngine->DXRelease();

	SAFE_DELETE(m_GameProcessInstance);
}
