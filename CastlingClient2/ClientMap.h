#pragma once
#include "Map.h"
#include "ModelObject.h"

///  �� �������ִ� Ŭ����
///  �� �� ĭ(Grid)���� Ÿ��, �ڿ�, ���� ���� ������ �־�� �Ѵ�.
///
///  [8/11/2020 Kolyn]

class ClientMap
{
public:
	struct ClientData
	{
		bool isAnimation = false;
		bool isPicking = false;
	};

public:
	Map m_Map;					/// �������� �޴� �� ����
	ClientData m_ClientData[Map::WIDTH][Map::HEIGHT];
	ModelObject* m_Object;

public:
	ClientMap();
	~ClientMap();

public:
	void CheckCastling();
	void Update();
	void Draw(float dTime);
	void AnimationUnitDrawAll(float dTime);
	void AnimationUnitDraw(float dTime, int x, int y, bool isAttack, Piece piece);
	void UnitDraw(float dTime);
	void UnitDraw_JustOne(float dTime, int x, int y, bool isAttack, Piece piece);
	void TileDraw(float dTime);
	void ResourceDraw(float dTime);
	void ResourceDraw_Alpha(float dTime);
	void Reset_Animation_Attack();
	void Reset_Animation_Die();

	Vector3 CalculatePieceRot(int x, int y);
	Vector3 CalculatePieceRot(Piece piece);
	Vector3 CalculatePieceRot_Inverse(Piece piece);
	int CheckTileTextureIndex(int x, int y);
	int CheckUnitTextureIndex(int x, int y);
	int CheckUnitTextureIndex(Piece piece);
	int CheckCastleTextureIndex(int x, int y);
	int CountPieceSCVCount();
	void ResetAnimationAllData();
};