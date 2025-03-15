#include "ObjectManager.h"
#include "ModelLoadManager.h"
#include "CGameProcess.h"
#include "NetworkCallback.h"

ObjectManager::ObjectManager()
{
	m_ClientMap = new ClientMap();


	//m_TileObjectList.reserve();
}

ObjectManager::~ObjectManager()
{
	delete m_ClientMap;

	for (auto& i : m_TileObjectList)
	{
		delete i;
	}

	for (auto& i : m_UnitObjectList)
	{
		delete i;
	}

	for (auto& i : m_ResourceObjectList)
	{
		delete i;
	}

	for (auto& i : m_EtcObjectList)
	{
		delete i;
	}
}

void ObjectManager::AddOneUnitGameObject()
{
	if (checkSuccess)
	{
		SoundManager* soundManager = SoundManager::GetIns();
		soundManager->StopSoundChannel(SFX);
		soundManager->PlaySound_Enum(eSoundType::Sound_Eff_Creation);
		m_ClientMap->m_Map.map[createPosX][createPosY].Piece = pieceData;
	}
}

void ObjectManager::AddEtcGameObject(eObjectType objType, Vector3 Pos, Vector3 Rot, Vector3 Scale)
{
	ModelLoadManager* modelManager = ModelLoadManager::GetIns();
	ModelObject* newModelObj = new ModelObject();

	newModelObj->m_pObj = modelManager->m_pGroundObj;
	newModelObj->m_ObjType = eObjectType::BACKGROUNDROCK;

	newModelObj->m_vPos = Pos;
	newModelObj->m_vRot = Rot;
	newModelObj->m_vScale = Scale;

	m_EtcObjectList.push_back(newModelObj);
}

void ObjectManager::Update(float dTime)
{
	m_ClientMap->Update();

	m_EtcObjectList[0]->ModelUpdate(dTime);
}

void ObjectManager::Draw(float dTime)
{
	int materialIndex = -1;
	m_EtcObjectList[0]->ModelDraw(materialIndex, false, false);

	m_ClientMap->Draw(dTime);
}

void ObjectManager::CalcUnitRange()
{
	CGameProcess* gameProcess = CGameProcess::GetIns();
	int x = gameProcess->m_MapStartPosX;
	int y = gameProcess->m_MapStartPosY;

	m_ClientMap->m_Map.CalcMoveAbleGrid(x, y);

	for (unsigned int i = 0; i < m_ClientMap->m_Map.MoveAbleGrid.size(); i++)
	{
		m_ClientMap->m_ClientData[m_ClientMap->m_Map.MoveAbleGrid[i].x][m_ClientMap->m_Map.MoveAbleGrid[i].y].isPicking = TRUE;
	}
}

void ObjectManager::CheckPicking(Vector4 pickingPoint)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	// ��ŷ�� ��ǥ�� ������ ���� �ε����� �ٲ�.
	int x = (int)(pickingPoint.x + 4.5f);
	int y = (int)(pickingPoint.z + 4.5f);

	// ������ ����üũ
	if (0 <= x && x < 9 && 0 <= y && y < 9)
	{
		// ���� ���� ������ �� ��ŷ ����
		if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_CREATE_REQUEST)
		{
			if (m_ClientMap->m_Map.map[x][y].Piece.type == Piece::Type::NONE)
			{
				CreatePicking(x, y);
			}
			else
			{
				gameProcess->m_InGameState = eInGameState::NONE;
			}
		}
		else
		{
			// ������ �ִٸ� �̵� ��ŷ ����
			if (m_ClientMap->m_Map.map[x][y].Piece.type != Piece::Type::NONE)
			{
				if (gameProcess->m_PickingState == ePickingState::STANDBY)
				{
					// ������Ʈ�� ���´�?
					Pick_Up(x, y);
				}
				else if (gameProcess->m_PickingState == ePickingState::ON)
				{
					// ���� �� ������Ʈ�� �̵��Ѵ�.
					Pick_MoveTo(x, y);
				}
				else
				{
					// ���� ������Ʈ�� �� �� ������ ���� �ʱ�ȭ�� �Ѵ�.
					Unselect();
					SetTilePickingState(gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, FALSE);
					ResetPicking();
				}
			}
			// �� �����̴�.
			else
			{
				if (gameProcess->m_PickingState == ePickingState::ON)
				{
					// ���� �� ������Ʈ�� �̵��Ѵ�.
					Pick_MoveTo(x, y);
				}
			}
		}
	}
}

void ObjectManager::CalcCreateRange()
{
	if (myData.OwnPlayer == eMatchUser::P1)
	{
		for (int i = 0; i < 9; i++)
		{
			if (m_ClientMap->m_Map.map[i][0].Piece.type == Piece::Type::NONE)
			{
				m_ClientMap->m_ClientData[i][0].isPicking = TRUE;
			}
		}
	}
	else if (myData.OwnPlayer == eMatchUser::P2)
	{
		for (int i = 0; i < 9; i++)
		{
			if (m_ClientMap->m_Map.map[i][Map::HEIGHT - 1].Piece.type == Piece::Type::NONE)
			{
				m_ClientMap->m_ClientData[i][Map::HEIGHT - 1].isPicking = TRUE;
			}
		}
	}
}

void ObjectManager::UnitMove(POINT ptStart, POINT ptEnd)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	if (m_ClientMap->m_Map.map[ptStart.x][ptStart.y].Piece.type != Piece::Type::NONE)
	{
		m_ClientMap->m_Map.map[ptStart.x][ptStart.y].Piece.MoveTurning(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
		// ������ �ִ� �� ����
		m_ClientMap->m_Map.map[ptEnd.x][ptEnd.y].Piece = m_ClientMap->m_Map.map[ptStart.x][ptStart.y].Piece;
		m_ClientMap->m_Map.map[ptStart.x][ptStart.y].Piece = Piece();
	}
}

void ObjectManager::Reset_Animation_Attack()
{
	m_ClientMap->Reset_Animation_Attack();
}

void ObjectManager::Reset_Animation_Die()
{
	m_ClientMap->Reset_Animation_Die();
}

void ObjectManager::CreatePicking(int x, int y)
{
	ResetCreatePicking();
	CGameProcess* gameProcess = CGameProcess::GetIns();

	//1P �϶�
	if (myData.OwnPlayer == eMatchUser::P1)
	{
		if (y == 0)
		{
			// ���� ����
			pieceData.type = gameProcess->m_CreateUnit;
			createPosX = x;
			createPosY = y;
			isCoin = gameProcess->m_isCoin;

			if (gameProcess->m_InGameState != eInGameState::ACTION_UNIT_ANIMATION_ATTACK
				&& gameProcess->m_InGameState != eInGameState::ACTION_UNIT_ANIMATION_DIE)
			{
				SendFunc(PROTOCOL::ACTION_UNIT_CREATE);
			}


			// ���� ���� ���� ��ġ �ʱ�ȭ
			//gameProcess->m_isUICheck = FALSE;
			gameProcess->m_InGameState = eInGameState::NONE;
		}
		else
		{
			gameProcess->m_InGameState = eInGameState::NONE;
		}
	}
	//2P �ϋ�
	else if(myData.OwnPlayer == eMatchUser::P2)
	{
		if (y == 8)
		{
			// ���� ����
			pieceData.type = gameProcess->m_CreateUnit;
			createPosX = x;
			createPosY = y;
			isCoin = gameProcess->m_isCoin;
			if (gameProcess->m_InGameState != eInGameState::ACTION_UNIT_ANIMATION_ATTACK
				&& gameProcess->m_InGameState != eInGameState::ACTION_UNIT_ANIMATION_DIE)
			{
				SendFunc(PROTOCOL::ACTION_UNIT_CREATE);
			}

			gameProcess->m_InGameState = eInGameState::NONE;
		}
		else
		{
			gameProcess->m_InGameState = eInGameState::NONE;
		}
	}
}

void ObjectManager::Pick_Up(int x, int y)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	// ���� �ش� ��ġ�� ������ ���� ���
	if (m_ClientMap->m_Map.map[x][y].Piece.type != Piece::Type::NONE)
	{
		// �� ������ �ƴϸ� ���Ѵ�
		if (m_ClientMap->m_Map.map[x][y].Piece.owner != myData.OwnPlayer)
			return;

		m_ClientMap->m_ClientData[x][y].isPicking = TRUE;

		gameProcess->m_InGameState = eInGameState::ACTION_UNIT_PICKING;
		gameProcess->m_PickingState = ePickingState::ON;
		gameProcess->m_MapStartPosX = x;
		gameProcess->m_MapStartPosY = y;

		// ���� ������ ���?
		CalcUnitRange();
	}

	// ���������� �ƹ� �ϵ� �Ͼ�� �ʴ´�.
}

// �̵��Ѵ�.
void ObjectManager::Pick_MoveTo(int x, int y)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	// �̵��� ����
	gameProcess->m_MapEndPosX = x;
	gameProcess->m_MapEndPosY = y;

	bool isMoveable = false;

	for (unsigned int i = 0; i < m_ClientMap->m_Map.MoveAbleGrid.size(); i++)
	{
		if (x == m_ClientMap->m_Map.MoveAbleGrid[i].x && y == m_ClientMap->m_Map.MoveAbleGrid[i].y)
		{
			isMoveable = true;
			break;
		}
	}

	if (isMoveable)
	{
		// �̵��Ѵ�.
		// �̵� ��ŷ ���¸� �� �ش�.
		// �̵���Ŷ ������
		SendFunc(PROTOCOL::ACTION_UNIT_MOVE);
		///gameProcess->m_InGameState = eInGameState::ACTION_UNIT_MOVE;
		/// ������ζ�� �̵��ϴ� �߰��� �����̴�.
		///gameProcess->m_PickingState = ePickingState::OFF;
		gameProcess->m_PickingState = ePickingState::STANDBY;

	}
	else
	{
		Unselect();
	}

	///// �̵� ��ŷ ���� �� �ش� Ÿ�ϸʵ� ��ŷ ���°� ����
	SetTilePickingState(gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, FALSE);
	SetTilePickingState(gameProcess->m_MapEndPosX, gameProcess->m_MapEndPosY, FALSE);
	ResetPicking();
}

void ObjectManager::ResetCreatePicking()
{
	if (myData.OwnPlayer == eMatchUser::P1)
	{
		for (int i = 0; i < 9; i++)
		{
			if (m_ClientMap->m_Map.map[i][0].Piece.type == Piece::Type::NONE)
			{
				m_ClientMap->m_ClientData[i][0].isPicking = FALSE;
			}
		}
	}
	else if (myData.OwnPlayer == eMatchUser::P2)
	{
		for (int i = 0; i < 9; i++)
		{
			if (m_ClientMap->m_Map.map[i][Map::HEIGHT - 1].Piece.type == Piece::Type::NONE)
			{
				m_ClientMap->m_ClientData[i][Map::HEIGHT - 1].isPicking = FALSE;
			}
		}
	}
}

// ��ŷ ���¸� �����Ѵ�.
void ObjectManager::ResetPicking()
{
	CGameProcess* gameProcess = CGameProcess::GetIns();
	int x = gameProcess->m_MapStartPosX;
	int y = gameProcess->m_MapStartPosY;

	/// �� ���� ������Ʈ�� ������, Ŭ��/�������� ������ �����ش�.
	if (m_ClientMap->m_Map.map[x][y].Piece.type != Piece::Type::NONE)
	{
		for (unsigned int i = 0; i < m_ClientMap->m_Map.MoveAbleGrid.size(); i++)
		{
			m_ClientMap->m_ClientData[m_ClientMap->m_Map.MoveAbleGrid[i].x][m_ClientMap->m_Map.MoveAbleGrid[i].y].isPicking = FALSE;
		}
		m_ClientMap->m_Map.MoveAbleGrid.clear();
	}
}

// ��ŷ ���¸� ĵ���ؾ� �Ѵ�.
void ObjectManager::Unselect()
{
	CGameProcess::GetIns()->m_InGameState = eInGameState::NONE;
	CGameProcess::GetIns()->m_PickingState = ePickingState::STANDBY;
}

void ObjectManager::SetTilePickingState(int x, int y, BOOL flag)
{
	m_ClientMap->m_ClientData[x][y].isPicking = flag;
}

void ObjectManager::ResetPickingAll()
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	ResetPicking();
	ResetCreatePicking();
	Unselect();
	SetTilePickingState(gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, FALSE);
}

void ObjectManager::DebugShieldState(int x, int y)
{
	for (int x = 0; x < Map::WIDTH; x++)
	{
		for (int z = 0; z < Map::HEIGHT; z++)
		{
			if (m_ClientMap->m_Map.map[x][z].Piece.type == Piece::Type::SHIELD)
			{
				XMFLOAT4 col = XMFLOAT4(1, 1, 1, 1);
				D3DEngine::GetIns()->_TT(x, y += 14, col, L"[Shield Info]");
				D3DEngine::GetIns()->_TT(x, y += 14, col, L"Locate : %d %d", x, z);
				D3DEngine::GetIns()->_TT(x, y += 14, col, L"Dir : %d", m_ClientMap->m_Map.map[x][z].Piece.ShiledDir);
				D3DEngine::GetIns()->_TT(x, y += 14, col, L"Owner : %d", m_ClientMap->m_Map.map[x][z].Piece.owner);
				y += 14;
			}
		}
	}
}

int ObjectManager::GetUnitCount(eObjectType type)
{
	switch (type)
	{
		case eObjectType::TILE1:
		case eObjectType::TILE2:
		{

		}
		break;

		case eObjectType::SCV:
		case eObjectType::SWORD:
		case eObjectType::BOW:
		case eObjectType::KNIGHT:
		case eObjectType::SHIELD:
		case eObjectType::CASTLE:
		case eObjectType::WHEAT:
		case eObjectType::IRON:
		case eObjectType::HORSE:
		{

		}
		break;

		case eObjectType::NONE:
		{

		}
		break;

		default:
		{

		}
		break;
	}

	return 0;
}

size_t ObjectManager::GetTileCountAll()
{
	return m_TileObjectList.size();
}

size_t ObjectManager::GetUnitCountAll()
{
	return m_UnitObjectList.size();
}

size_t ObjectManager::GetResourceCountAll()
{
	return m_ResourceObjectList.size();
}

size_t ObjectManager::GetEtcCountAll()
{
	return m_EtcObjectList.size();
}
