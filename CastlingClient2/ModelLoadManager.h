#pragma once
#include "ModelAse.h"

///
///
///  모델을 로드하고 가지고 있는 매니저 클래스
///
///  [8/11/2020 Kolyn]

class ModelLoadManager
{

public:
	ModelAse* m_pTile1;
	ModelAse* m_pTile2;
	ModelAse* m_pScv_Idle;
	ModelAse* m_pScv_Work;
	ModelAse* m_pScv_Die;
	ModelAse* m_pBow_Idle;
	ModelAse* m_pBow_Attack;
	ModelAse* m_pBow_Die;
	ModelAse* m_pKnight_Idle;
	ModelAse* m_pKnight_Attack;
	ModelAse* m_pKnight_Die;
	ModelAse* m_pSword_Idle;
	ModelAse* m_pSword_Attack;
	ModelAse* m_pSword_Die;
	ModelAse* m_pShield_Idle;
	ModelAse* m_pShield_Attack;
	ModelAse* m_pShield_Die;
	ModelAse* m_pCastle;
	ModelAse* m_pWheat;
	ModelAse* m_pIron;
	ModelAse* m_pHorse;
	ModelAse* m_pGroundObj;

private:
	static ModelLoadManager* m_Instance;

public:
	static ModelLoadManager* GetIns()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new ModelLoadManager();
		}
		return m_Instance;
	}

public:
	ModelLoadManager();
	~ModelLoadManager();

public:
	void AllModelLoad();
	int LoadModelFileName(TCHAR* filename);
	BOOL ModelLoad(ModelAse** Obj);
	BOOL ModelLoadFile(const TCHAR* filename, ModelAse** Obj);
	void Release();
};

