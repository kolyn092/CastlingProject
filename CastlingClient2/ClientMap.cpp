#include "ClientMap.h"
#include "CGameProcess.h"
#include "NetworkCallback.h"

//////////////////////////////////////////////////////////////////////////

ClientMap::ClientMap()
{
	m_Map = Map();
	m_Object = new ModelObject();
}

ClientMap::~ClientMap()
{
	if (m_Object != nullptr)	delete m_Object;
	//delete m_Map;
}

void ClientMap::CheckCastling()
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	if (myData.OwnPlayer == eMatchUser::P1)
	{
		if (m_Map.map[Map::P1_START_X][Map::P1_START_Y].Piece.type != Piece::Type::NONE
			&& m_Map.map[Map::P1_START_X][Map::P1_START_Y].Piece.owner != eMatchUser::P1)
		{
			gameProcess->m_UIManager->m_isEnemyCastling = true;
		}
		else if (m_Map.map[Map::P2_START_X][Map::P2_START_Y].Piece.type != Piece::Type::NONE
			&& m_Map.map[Map::P2_START_X][Map::P2_START_Y].Piece.owner == eMatchUser::P1)
		{
			gameProcess->m_UIManager->m_isMyCastling = true;
		}
		else
		{
			gameProcess->m_UIManager->m_isEnemyCastling = false;
			gameProcess->m_UIManager->m_isMyCastling = false;
		}
	}
	else if (myData.OwnPlayer == eMatchUser::P2)
	{
		if (m_Map.map[Map::P2_START_X][Map::P2_START_Y].Piece.type != Piece::Type::NONE
			&& m_Map.map[Map::P2_START_X][Map::P2_START_Y].Piece.owner != eMatchUser::P2)
		{
			gameProcess->m_UIManager->m_isEnemyCastling = true;
		}
		else if (m_Map.map[Map::P1_START_X][Map::P1_START_Y].Piece.type != Piece::Type::NONE
			&& m_Map.map[Map::P1_START_X][Map::P1_START_Y].Piece.owner == eMatchUser::P2)
		{
			gameProcess->m_UIManager->m_isMyCastling = true;
		}
		else
		{
			gameProcess->m_UIManager->m_isEnemyCastling = false;
			gameProcess->m_UIManager->m_isMyCastling = false;
		}
	}
}

void ClientMap::Update()
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	CheckCastling();

	if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_ATTACK)
	{
		int StartPosX = gameProcess->m_MapStartPosX;
		int StartPosY = gameProcess->m_MapStartPosY;

		int EndPosX = gameProcess->m_MapEndPosX;
		int EndPosY = gameProcess->m_MapEndPosY;

		if (StartPosX == EndPosX && StartPosY == EndPosY)
		{
			ResetAnimationAllData();
			gameProcess->m_InGameState = eInGameState::ACTION_UNIT_MOVE;
			return;
		}

		// 이동할 지점에 적이 없으면 애니메이션하지 않고 이동한다.
		if (MoveEndPiece.type == Piece::Type::NONE)
		{
			ResetAnimationAllData();
			gameProcess->m_InGameState = eInGameState::ACTION_UNIT_MOVE;
		}
		else
		{
			if (resetEvent)
			{
				resetEvent = false;
				ResetEvent(sendAnimationEvent);
				setEvent = true;
			}
			m_ClientData[StartPosX][StartPosY].isAnimation = true;
			m_ClientData[EndPosX][EndPosY].isAnimation = true;
		}
	}
}

void ClientMap::Draw(float dTime)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	///if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_ATTACK)
	{
		AnimationUnitDrawAll(dTime);
	}

	UnitDraw(dTime);

	// 타일 그리기
	TileDraw(dTime);

	// 리소스 그리기
	ResourceDraw(dTime);

	ResourceDraw_Alpha(dTime);
}

void ClientMap::AnimationUnitDrawAll(float dTime)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_ATTACK)
	{
		AnimationUnitDraw(dTime, gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, true, MoveStartPiece);
		UnitDraw_JustOne(dTime, gameProcess->m_MapEndPosX, gameProcess->m_MapEndPosY, false, MoveEndPiece);
	}
	else if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_DIE)
	{
		UnitDraw_JustOne(dTime, gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, true, MoveStartPiece);
		AnimationUnitDraw(dTime, gameProcess->m_MapEndPosX, gameProcess->m_MapEndPosY, false, MoveEndPiece);
	}
}

void ClientMap::AnimationUnitDraw(float dTime, int x, int y, bool isAttack, Piece piece)
{
	ModelLoadManager* modelManager = ModelLoadManager::GetIns();
	CGameProcess* gameProcess = CGameProcess::GetIns();

	// 타일에 그려질 3D 좌표 위치
	float posX = static_cast<float>(x - 4);
	float posZ = static_cast<float>(y - 4);

	piece.MoveTurning(gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, gameProcess->m_MapEndPosX, gameProcess->m_MapEndPosY);

	m_Object->m_vPos = Vector3(posX, 0, posZ);
	if (isAttack)
	{
		m_Object->m_vRot = CalculatePieceRot(piece);
	}
	else
	{
		m_Object->m_vRot = CalculatePieceRot_Inverse(piece);
	}
	m_Object->m_vScale = Vector3(0.8f, 0.8f, 0.8f);


	switch (piece.type)
	{
		case Piece::Type::SCV:
			if (isAttack)
			{
				// scv는 공격없이 일하는중
				m_Object->m_pObj = modelManager->m_pScv_Work;
				int scvCount = CountPieceSCVCount();
				m_Object->m_pObj->m_AniSpeed = m_Object->m_pObj->m_AniSpeed / (float)scvCount;
			}
			else
			{
				m_Object->m_pObj = modelManager->m_pScv_Die;
			}
			m_Object->m_ObjType = eObjectType::SCV;
			break;
		case Piece::Type::SWORD:
			if (isAttack)
			{
				m_Object->m_pObj = modelManager->m_pSword_Attack;
			}
			else
			{
				m_Object->m_pObj = modelManager->m_pSword_Die;
			}
			m_Object->m_ObjType = eObjectType::SWORD;
			break;
		case Piece::Type::BOW:
			if (isAttack)
			{
				m_Object->m_pObj = modelManager->m_pBow_Attack;
			}
			else
			{
				m_Object->m_pObj = modelManager->m_pBow_Die;
			}
			m_Object->m_ObjType = eObjectType::BOW;
			break;
		case Piece::Type::KNIGHT:
			if (isAttack)
			{
				m_Object->m_pObj = modelManager->m_pKnight_Attack;
			}
			else
			{
				m_Object->m_pObj = modelManager->m_pKnight_Die;
			}
			m_Object->m_ObjType = eObjectType::KNIGHT;
			break;
		case Piece::Type::SHIELD:
			if (isAttack)
			{
				m_Object->m_pObj = modelManager->m_pShield_Attack;
			}
			else
			{
				m_Object->m_pObj = modelManager->m_pShield_Die;
			}
			m_Object->m_ObjType = eObjectType::SHIELD;
			break;
		default:
			return;
	}

	m_Object->ModelUpdate(dTime);
	int textureIndex = CheckUnitTextureIndex(piece);
	m_Object->ModelDraw(textureIndex, false, false);
}

void ClientMap::UnitDraw(float dTime)
{
	ModelLoadManager* modelManager = ModelLoadManager::GetIns();

	for (int x = 0; x < Map::WIDTH; x++)
	{
		for (int y = 0; y < Map::HEIGHT; y++)
		{
			// 없으면 다음으로 넘어간다
			if (m_Map.map[x][y].Piece.type == Piece::Type::NONE
				|| m_ClientData[x][y].isAnimation == true)
				continue;

			// 타일에 그려질 3D 좌표 위치
			float posX = static_cast<float>(x - 4);
			float posZ = static_cast<float>(y - 4);

			Vector3 vRot = CalculatePieceRot(x, y);

			// 스케일 계산 후 Update, Draw 같이.
			// Draw 할 때 1p 2p 텍스처 구분
			m_Object->m_vPos = Vector3(posX, 0, posZ);
			m_Object->m_vRot = vRot;
			m_Object->m_vScale = Vector3(0.8f, 0.8f, 0.8f);

			switch (m_Map.map[x][y].Piece.type)
			{
				case Piece::Type::SCV:
					m_Object->m_pObj = modelManager->m_pScv_Idle;
					m_Object->m_ObjType = eObjectType::SCV;
					m_Object->m_vScale = Vector3(0.008f, 0.008f, 0.008f);
					break;
				case Piece::Type::SWORD:
					m_Object->m_pObj = modelManager->m_pSword_Idle;
					m_Object->m_ObjType = eObjectType::SWORD;
					break;
				case Piece::Type::BOW:
					m_Object->m_pObj = modelManager->m_pBow_Idle;
					m_Object->m_ObjType = eObjectType::BOW;
					break;
				case Piece::Type::KNIGHT:
					m_Object->m_pObj = modelManager->m_pKnight_Idle;
					m_Object->m_ObjType = eObjectType::KNIGHT;
					break;
				case Piece::Type::SHIELD:
					m_Object->m_pObj = modelManager->m_pShield_Idle;
					m_Object->m_ObjType = eObjectType::SHIELD;
					break;
			}

			m_Object->ModelUpdate(dTime);
			int textureIndex = CheckUnitTextureIndex(x, y);
			m_Object->ModelDraw(textureIndex, false, false);
		}
	}
}

void ClientMap::UnitDraw_JustOne(float dTime, int x, int y, bool isAttack, Piece piece)
{
	ModelLoadManager* modelManager = ModelLoadManager::GetIns();
	CGameProcess* gameProcess = CGameProcess::GetIns();

	// 타일에 그려질 3D 좌표 위치
	float posX = static_cast<float>(x - 4);
	float posZ = static_cast<float>(y - 4);

	piece.MoveTurning(gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, gameProcess->m_MapEndPosX, gameProcess->m_MapEndPosY);

	m_Object->m_vPos = Vector3(posX, 0, posZ);
	if (isAttack)
	{
		m_Object->m_vRot = CalculatePieceRot(piece);
	}
	else
	{
		m_Object->m_vRot = CalculatePieceRot_Inverse(piece);
	}
	m_Object->m_vScale = Vector3(0.8f, 0.8f, 0.8f);

	switch (piece.type)
	{
		case Piece::Type::SCV:
			m_Object->m_pObj = modelManager->m_pScv_Idle;
			m_Object->m_vScale = Vector3(0.008f, 0.008f, 0.008f);
			m_Object->m_ObjType = eObjectType::SCV;
			break;
		case Piece::Type::SWORD:
			m_Object->m_pObj = modelManager->m_pSword_Idle;
			m_Object->m_ObjType = eObjectType::SWORD;
			break;
		case Piece::Type::BOW:
			m_Object->m_pObj = modelManager->m_pBow_Idle;
			m_Object->m_ObjType = eObjectType::BOW;
			break;
		case Piece::Type::KNIGHT:
			m_Object->m_pObj = modelManager->m_pKnight_Idle;
			m_Object->m_ObjType = eObjectType::KNIGHT;
			break;
		case Piece::Type::SHIELD:
			m_Object->m_pObj = modelManager->m_pShield_Idle;
			m_Object->m_ObjType = eObjectType::SHIELD;
			break;
		default:
			return;
	}

	m_Object->ModelUpdate(dTime);
	int textureIndex = CheckUnitTextureIndex(piece);
	m_Object->ModelDraw(textureIndex, false, false);
}

void ClientMap::TileDraw(float dTime)
{
	ModelLoadManager* modelManager = ModelLoadManager::GetIns();

	for (int x = 0; x < Map::WIDTH; x++)
	{
		for (int y = 0; y < Map::HEIGHT; y++)
		{
			int index = ((x * Map::WIDTH) + y) % 2;

			if (index == 0)
			{
				m_Object->m_pObj = modelManager->m_pTile1;
			}
			else
			{
				m_Object->m_pObj = modelManager->m_pTile2;
			}

			m_Object->m_vPos = Vector3((float)x - 4, -1, (float)y - 4);
			m_Object->m_vRot = Vector3::Zero;
			m_Object->m_vScale = Vector3::One;
			m_Object->ModelUpdate(dTime);
			int textureIndex = CheckTileTextureIndex(x, y);
			bool isPicking = m_ClientData[x][y].isPicking;
			m_Object->ModelDraw(textureIndex, true, isPicking);
		}
	}
}

void ClientMap::ResourceDraw(float dTime)
{
	ModelLoadManager* modelManager = ModelLoadManager::GetIns();
	CGameProcess* gameProcess = CGameProcess::GetIns();

	for (int x = 0; x < Map::WIDTH; x++)
	{
		for (int y = 0; y < Map::HEIGHT; y++)
		{
			/// 타일의 위치를 계산
			/// 타일의 Piece 회전 계산
			/// 타일의 스케일 계산
			/// 이후 Draw

			// 없으면 다음으로 넘어간다
			if (m_Map.map[x][y].Resource.Count <= 0
				|| m_Map.map[x][y].Resource.type == Resource::Type::NONE
				|| m_Map.map[x][y].Resource.type == Resource::Type::WHEAT)
				continue;

			// 타일에 그려질 3D 좌표 위치
			float posX = static_cast<float>(x - 4);
			float posZ = static_cast<float>(y - 4);

			// 성은 rot 계산해야한다. 1P/2P
			Vector3 vRot = Vector3::Zero;

			// 스케일 계산 후 Update, Draw 같이.
			// Draw 할 때 성 1p 2p 텍스처 구분, 리소스에 따라 타일 텍스처 구분
			// 성만 0.01
			Vector3 vScale = Vector3::One;

			m_Object->m_vPos = Vector3(posX, 0, posZ);
			m_Object->m_vRot = vRot;
			m_Object->m_vScale = Vector3(0.8f, 0.8f, 0.8f);

			int textureIndex = -1;


			switch (m_Map.map[x][y].Resource.type)
			{
				case Resource::Type::IRON:
					m_Object->m_pObj = modelManager->m_pIron;
					m_Object->m_ObjType = eObjectType::IRON;
					m_Object->ModelUpdate(dTime);
					m_Object->ModelDraw(textureIndex, false, false);
					break;
				case Resource::Type::HORSE:
					m_Object->m_pObj = modelManager->m_pHorse;
					m_Object->m_ObjType = eObjectType::HORSE;
					m_Object->m_vScale = Vector3(0.01f, 0.01f, 0.01f);
					m_Object->ModelUpdate(dTime);
					m_Object->ModelDraw(textureIndex, false, false);
					break;
				case Resource::Type::CASTLE:
					m_Object->m_pObj = modelManager->m_pCastle;
					m_Object->m_ObjType = eObjectType::CASTLE;
					if (x == Map::P1_START_X && y == Map::P1_START_Y)
					{
						m_Object->m_vRot = Vector3(0, D3DXToRadian(180), 0);
						textureIndex = -1;
						m_Object->m_vScale = Vector3(0.01f, 0.01f, 0.01f);
						m_Object->ModelUpdate(dTime);
						m_Object->CastleModelDraw(textureIndex, gameProcess->m_CastleLevel_1P);
					}
					else if (x == Map::P2_START_X && y == Map::P2_START_Y)
					{
						m_Object->m_vRot = Vector3(0, 0, 0);
						textureIndex = 2;
						m_Object->m_vScale = Vector3(0.01f, 0.01f, 0.01f);
						m_Object->ModelUpdate(dTime);
						m_Object->CastleModelDraw(textureIndex, gameProcess->m_CastleLevel_2P);
					}
					break;
			}
		}
	}
}

void ClientMap::ResourceDraw_Alpha(float dTime)
{
	ModelLoadManager* modelManager = ModelLoadManager::GetIns();

	for (int x = 0; x < Map::WIDTH; x++)
	{
		for (int y = 0; y < Map::HEIGHT; y++)
		{
			// 없으면 다음으로 넘어간다
			if (m_Map.map[x][y].Resource.Count <= 0
				|| m_Map.map[x][y].Resource.type == Resource::Type::NONE)
				continue;

			// 타일에 그려질 3D 좌표 위치
			float posX = static_cast<float>(x - 4);
			float posZ = static_cast<float>(y - 4);

			// 성은 rot 계산해야한다. 1P/2P
			Vector3 vRot = Vector3::Zero;

			// 스케일 계산 후 Update, Draw 같이.
			// Draw 할 때 성 1p 2p 텍스처 구분, 리소스에 따라 타일 텍스처 구분
			// 성만 0.01
			Vector3 vScale = Vector3::One;

			m_Object->m_vPos = Vector3(posX, 0, posZ);
			m_Object->m_vRot = vRot;
			m_Object->m_vScale = Vector3(0.8f, 0.8f, 0.8f);

			if (m_Map.map[x][y].Resource.type == Resource::Type::WHEAT)
			{
				m_Object->m_pObj = modelManager->m_pWheat;
				m_Object->m_ObjType = eObjectType::WHEAT;

				m_Object->ModelUpdate(dTime);
				int textureIndex = -1;
				m_Object->ModelDraw(textureIndex, false, false);
			}
		}
	}
}

void ClientMap::Reset_Animation_Attack()
{
	///CGameProcess* gameProcess = CGameProcess::GetIns();
	///m_ClientData[gameProcess->m_MapStartPosX][gameProcess->m_MapStartPosY].isAnimation = false;
	///m_ClientData[gameProcess->m_MapEndPosX][gameProcess->m_MapEndPosY].isAnimation = false;
}

void ClientMap::Reset_Animation_Die()
{
	CGameProcess* gameProcess = CGameProcess::GetIns();
	m_ClientData[gameProcess->m_MapStartPosX][gameProcess->m_MapStartPosY].isAnimation = false;
	m_ClientData[gameProcess->m_MapEndPosX][gameProcess->m_MapEndPosY].isAnimation = false;
}

Vector3 ClientMap::CalculatePieceRot(int x, int y)
{
	Vector3 vRot = Vector3::Zero;
	// 회전 계산
	if (m_Map.map[x][y].Piece.type == Piece::Type::SHIELD)
	{
		switch (m_Map.map[x][y].Piece.ShiledDir)
		{
			case Piece::Dir::FRONT:
			{
				vRot = Vector3(0, D3DXToRadian(0), 0);
			}
			break;
			case Piece::Dir::BACK:
			{
				vRot = Vector3(0, D3DXToRadian(180), 0);
			}
			break;
			case Piece::Dir::RIGHT:
			{
				vRot = Vector3(0, D3DXToRadian(270), 0);
			}
			break;
			case Piece::Dir::LEFT:
			{
				vRot = Vector3(0, D3DXToRadian(90), 0);
			}
			break;
		}
	}
	else
	{
		if (m_Map.map[x][y].Piece.owner == eMatchUser::P1)
		{
			vRot = Vector3(0, D3DXToRadian(180), 0);
		}
		else
		{
			vRot = Vector3(0, 0, 0);
		}
	}

	return vRot;
}

Vector3 ClientMap::CalculatePieceRot(Piece piece)
{
	Vector3 vRot = Vector3::Zero;
	Vector3 unitVec = Vector3::Zero;
	Vector3 moveVec = Vector3::Zero;

	CGameProcess* gameProcess = CGameProcess::GetIns();

	// 회전 계산
	if (piece.type != Piece::Type::BOW)
	{
		switch (piece.ShiledDir)
		{
			case Piece::Dir::FRONT:
			{
				vRot = Vector3(0, D3DXToRadian(0), 0);
			}
			break;
			case Piece::Dir::BACK:
			{
				vRot = Vector3(0, D3DXToRadian(180), 0);
			}
			break;
			case Piece::Dir::RIGHT:
			{
				vRot = Vector3(0, D3DXToRadian(270), 0);
			}
			break;
			case Piece::Dir::LEFT:
			{
				vRot = Vector3(0, D3DXToRadian(90), 0);
			}
			break;
		}
	}
	else
	{
		if(gameProcess->m_MapStartPosY < gameProcess->m_MapEndPosY)
		{
			unitVec = Vector3(0, 0, 1);
		}
		else
		{
			unitVec = Vector3(0, 0, -1);
		}

		moveVec = Vector3(gameProcess->m_MapEndPosX, 0, gameProcess->m_MapEndPosY) - Vector3(gameProcess->m_MapStartPosX, 0, gameProcess->m_MapStartPosY);
		moveVec.Normalize();
		float radian = atan((unitVec.x - moveVec.x) / (unitVec.y - moveVec.y));
		if (moveVec.x > unitVec.x)
		{
			if (moveVec.z > unitVec.z)
			{
				vRot = Vector3(0, radian + D3DXToRadian(45), 0);
			}
			else
			{
				vRot = Vector3(0, radian + D3DXToRadian(-45), 0);
			}
		}
		else
		{
			if (moveVec.z > unitVec.z)
			{
				vRot = Vector3(0, radian + D3DXToRadian(-45), 0);
			}
			else
			{
				vRot = Vector3(0, radian + D3DXToRadian(45), 0);
			}
		}
	}

	return vRot;
}

Vector3 ClientMap::CalculatePieceRot_Inverse(Piece piece)
{
	Vector3 vRot = Vector3::Zero;

	// 회전 계산
	if (piece.type != Piece::Type::BOW)
	{
		switch (piece.ShiledDir)
		{
			case Piece::Dir::FRONT:
			{
				vRot = Vector3(0, D3DXToRadian(180), 0);
			}
			break;
			case Piece::Dir::BACK:
			{
				vRot = Vector3(0, D3DXToRadian(0), 0);
			}
			break;
			case Piece::Dir::RIGHT:
			{
				vRot = Vector3(0, D3DXToRadian(90), 0);
			}
			break;
			case Piece::Dir::LEFT:
			{
				vRot = Vector3(0, D3DXToRadian(270), 0);
			}
			break;
		}
	}
	else
	{
		if (piece.owner == eMatchUser::P1)
		{
			vRot = Vector3(0, D3DXToRadian(180), 0);
		}
		else
		{
			vRot = Vector3(0, 0, 0);
		}
	}

	return vRot;
}

int ClientMap::CheckTileTextureIndex(int x, int y)
{
	int index = -1;

	switch (m_Map.map[x][y].Resource.type)
	{
		case Resource::Type::HORSE:
			index = 1;
			break;
		case Resource::Type::IRON:
			index = 2;
			break;
		case Resource::Type::WHEAT:
			index = 3;
			break;
	}
	return index;
}

int ClientMap::CheckUnitTextureIndex(int x, int y)
{
	int index = -1;

	switch (m_Map.map[x][y].Piece.owner)
	{
		case eMatchUser::P1:
			index = -1;
			break;
		case eMatchUser::P2:
			index = 2;
			break;
	}
	return index;
}

int ClientMap::CheckUnitTextureIndex(Piece piece)
{
	int index = -1;

	switch (piece.owner)
	{
		case eMatchUser::P1:
			index = -1;
			break;
		case eMatchUser::P2:
			index = 2;
			break;
	}
	return index;
}

int ClientMap::CheckCastleTextureIndex(int x, int y)
{
	int index = -1;

	switch (m_Map.map[x][y].Piece.owner)
	{
		case eMatchUser::P1:
			index = -1;
			break;
		case eMatchUser::P2:
			index = 2;
			break;
	}

	return index;
}

int ClientMap::CountPieceSCVCount()
{
	int scvCount = 0;
	for (int x = 0; x < Map::WIDTH; x++)
	{
		for (int y = 0; y < Map::HEIGHT; y++)
		{
			if (m_Map.map[x][y].Piece.type != Piece::Type::SCV)
				continue;

			scvCount++;
		}
	}

	return scvCount;
}

void ClientMap::ResetAnimationAllData()
{
	for (int x = 0; x < Map::WIDTH; x++)
	{
		for (int y = 0; y < Map::HEIGHT; y++)
		{
			if (m_ClientData[x][y].isAnimation == false)
				continue;

			m_ClientData[x][y].isAnimation = false;
		}
	}
}