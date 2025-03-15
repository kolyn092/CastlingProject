#pragma once
#include <vector>
#include "DXEngineDefine.h"
#include "GameDefine.h"
#include "ModelObject.h"
#include "Map.h"
#include "ClientMap.h"

///  ���� ������Ʈ���� �������ִ� �Ŵ��� Ŭ����
///
///  ������Ʈ ���� (����/Ÿ��/�ڿ�/��Ÿ)
///  ��ü ������ ��ü
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
	/// ��ŷ �κ�
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

	/// ������Ʈ �Ŵ������� �ʼ������� �ؾ� �ϴ� �ϵ�
public:
	void DebugShieldState(int x, int y);
	int GetUnitCount(eObjectType type);
	size_t GetTileCountAll();
	size_t GetUnitCountAll();
	size_t GetResourceCountAll();
	size_t GetEtcCountAll();

	/// ���� ����, Ư�� ������ ���� ������ �˻�, ����
	/// ����Ʈ�� ��ȯ (������ Update, Render)

};

