#pragma once
#include <vector>
#include "DXEngineDefine.h"
#include "GameDefine.h"
#include "ModelObject.h"
#include "Map.h"
#include "ClientMap.h"

///  게임 오브젝트들을 관리해주는 매니저 클래스
///
///  오브젝트 생성 (유닛/타일/자원/기타)
///  객체 관리의 주체
///
///  [8/11/2020 Kolyn]

class ObjectManager
{
public:
	ClientMap* m_ClientMap;

public:
	std::vector<ModelObject*> m_TileObjectList;

	std::vector<ModelObject*> m_UnitObjectList;
	std::vector<ModelObject*> m_ResourceObjectList;
	std::vector<ModelObject*> m_EtcObjectList;

public:
	ObjectManager();
	~ObjectManager();

public:
	void AddOneUnitGameObject();
	void AddEtcGameObject(eObjectType objType, Vector3 Pos, Vector3 Rot, Vector3 Scale);

public:
	void Update(float dTime);
	void Draw(float dTime);

	void CheckPicking(Vector4 vec);
	void CalcCreateRange();
	void UnitMove(POINT ptStart, POINT ptEnd);
	void Reset_Animation_Attack();
	void Reset_Animation_Die();

private:
	void CalcUnitRange();

	/// ///////////////////////////////////////////////////////////////////////
	/// 피킹 부분
	void CreatePicking(int x, int y);
	void Pick_Up(int x, int y);
	void Pick_MoveTo(int x, int y);

public:
	void ResetCreatePicking();
	void ResetPicking();

	void Unselect();
	void SetTilePickingState(int x, int y, BOOL flag);
	/// ///////////////////////////////////////////////////////////////////////
	void ResetPickingAll();

	/// 오브젝트 매니저에서 필수적으로 해야 하는 일들
public:
	void DebugShieldState(int x, int y);
	int GetUnitCount(eObjectType type);
	size_t GetTileCountAll();
	size_t GetUnitCountAll();
	size_t GetResourceCountAll();
	size_t GetEtcCountAll();

	/// 유닛 삭제, 특정 조건을 가진 유닛을 검색, 정렬
	/// 리스트를 반환 (이유는 Update, Render)

};

