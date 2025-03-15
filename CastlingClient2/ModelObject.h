#pragma once
#include "DXEngineDefine.h"
#include "GameDefine.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
//
// 3D �� - ĳ���� ��� �Լ���
//
//////////////////////////////////////////////////////////////////////////

class ModelAse;

class ModelObject : public AlignedAllocationPolicy<16>
{

public:
	eObjectType m_ObjType;
	ModelAse* m_pObj;

public:
	// ������ ������ �������� ��ǥ
	//int PosX;
	//int PosY;

public:
	XMMATRIX m_UserTM;

	VECTOR3 m_vPos;
	VECTOR3 m_vRot;
	VECTOR3 m_vScale;

	///BOOL m_isPicking;
	///BOOL m_isCreateZone;

	// ĳ���� ��ġ (0, 0) ���� �̵�(����) ������
	std::vector<std::vector<MoveRangePos>> m_MoveRange;

	// ����� ���� �̵� ������ �̵�(����) ������
	std::vector<MoveRangePos> m_CheckMoveRange;

	/// ���⿡ �ִϸ��̼� ���� ������ �������.
	/// �׷��� ModelAse ��ü�� �����ϵ� �ִϸ��̼��� �ٸ��� ������ �� ����

	eAnimationState m_AniState;

public:
	ModelObject();
	~ModelObject();

public:
	BOOL ModelUpdate(float dTime = 0);
	BOOL ModelDraw(int materialIndex, bool isTile, bool isPicking);
	BOOL CastleModelDraw(int materialIndex, int castleLevel);

	int ModelShowInfo(int scrWidth, int scrHeight, float dTime);
};
