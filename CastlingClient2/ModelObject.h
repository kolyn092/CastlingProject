#pragma once
#include "DXEngineDefine.h"
#include "GameDefine.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
//
// 3D 모델 - 캐릭터 운용 함수들
//
//////////////////////////////////////////////////////////////////////////

class ModelAse;

class ModelObject : public AlignedAllocationPolicy<16>
{

public:
	eObjectType m_ObjType;
	ModelAse* m_pObj;

public:
	// 논리적인 게임판 위에서의 좌표
	//int PosX;
	//int PosY;

public:
	XMMATRIX m_UserTM;

	VECTOR3 m_vPos;
	VECTOR3 m_vRot;
	VECTOR3 m_vScale;

	///BOOL m_isPicking;
	///BOOL m_isCreateZone;

	// 캐릭터 위치 (0, 0) 기준 이동(공격) 범위들
	std::vector<std::vector<MoveRangePos>> m_MoveRange;

	// 연산된 실제 이동 가능한 이동(공격) 범위들
	std::vector<MoveRangePos> m_CheckMoveRange;

	/// 여기에 애니메이션 관련 변수들 빼줘야함.
	/// 그래야 ModelAse 객체는 공유하되 애니메이션은 다르게 적용할 수 있음

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
