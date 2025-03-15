#include "ModelLoadManager.h"
#include "D3DEngine.h"
#include <clocale>

ModelLoadManager* ModelLoadManager::m_Instance = nullptr;

ModelLoadManager::ModelLoadManager()
{
	m_pTile1 = nullptr;
	m_pTile2 = nullptr;
	m_pScv_Idle = nullptr;
	m_pScv_Work = nullptr;
	m_pScv_Die = nullptr;
	m_pBow_Idle = nullptr;
	m_pBow_Attack = nullptr;
	m_pBow_Die = nullptr;
	m_pKnight_Idle = nullptr;
	m_pKnight_Attack = nullptr;
	m_pKnight_Die = nullptr;
	m_pSword_Idle = nullptr;
	m_pSword_Attack = nullptr;
	m_pSword_Die = nullptr;
	m_pShield_Idle = nullptr;
	m_pShield_Attack = nullptr;
	m_pShield_Die = nullptr;
	m_pCastle = nullptr;
	m_pWheat = nullptr;
	m_pIron = nullptr;
	m_pHorse = nullptr;
	m_pGroundObj = nullptr;
}

ModelLoadManager::~ModelLoadManager()
{

}

void ModelLoadManager::AllModelLoad()
{
	ModelLoadFile(L"../Data/MapTile/Tile1.ase", &m_pTile1);
	ModelLoadFile(L"../Data/MapTile/Tile2.ase", &m_pTile2);

	ModelLoadFile(L"../Data/Character/Character_Bow_Idle.ase", &m_pBow_Idle);
	//ModelLoadFile(L"../Data/Character/Character_Bow_Attack.ase", &m_pBow_Idle);
	ModelLoadFile(L"../Data/Character/Character_Bow_Attack.ase", &m_pBow_Attack);
	ModelLoadFile(L"../Data/Character/Character_Bow_Die.ase", &m_pBow_Die);

	ModelLoadFile(L"../Data/Character/Character_Scv_Idle.ase", &m_pScv_Idle);
	//ModelLoadFile(L"../Data/Character/Character_Scv_Working.ase", &m_pScv_Idle);
	ModelLoadFile(L"../Data/Character/Character_Scv_Working.ase", &m_pScv_Work);
	ModelLoadFile(L"../Data/Character/Character_Scv_Die.ase", &m_pScv_Die);

	ModelLoadFile(L"../Data/Character/Character_Knight_Idle.ase", &m_pKnight_Idle);
	//ModelLoadFile(L"../Data/Character/Character_Knight_Attack.ase", &m_pKnight_Idle);
	ModelLoadFile(L"../Data/Character/Character_Knight_Attack.ase", &m_pKnight_Attack);
	ModelLoadFile(L"../Data/Character/Character_Knight_Die.ase", &m_pKnight_Die);
	m_pKnight_Die->m_AniSpeed = 0.3f;

	ModelLoadFile(L"../Data/Character/Character_Sword_Idle.ase", &m_pSword_Idle);
	//ModelLoadFile(L"../Data/Character/Character_Sword_Attack.ase", &m_pSword_Idle);
	ModelLoadFile(L"../Data/Character/Character_Sword_Attack.ase", &m_pSword_Attack);
	ModelLoadFile(L"../Data/Character/Character_Sword_Die.ase", &m_pSword_Die);
	m_pSword_Die->m_AniSpeed = 0.3f;

	ModelLoadFile(L"../Data/Character/Character_Shield_Idle.ase", &m_pShield_Idle);
	//ModelLoadFile(L"../Data/Character/Character_Shield_Attack.ase", &m_pShield_Idle);
	ModelLoadFile(L"../Data/Character/Character_Shield_Attack.ase", &m_pShield_Attack);
	ModelLoadFile(L"../Data/Character/Character_Shield_Die.ase", &m_pShield_Die);
	m_pShield_Die->m_AniSpeed = 0.3f;

	ModelLoadFile(L"../Data/Castle/Castle.ase", &m_pCastle);
	ModelLoadFile(L"../Data/Resource/Resource_Wheat.ase", &m_pWheat);
	ModelLoadFile(L"../Data/Resource/Resource_Iron.ase", &m_pIron);
	ModelLoadFile(L"../Data/Resource/Resource_Horse.ase", &m_pHorse);
	ModelLoadFile(L"../Data/Obj_005.ase", &m_pGroundObj);
}

//////////////////////////////////////////////////////////////////////////
//
// �ܺ����Ͽ��� ASE ���ϸ� ���ϱ�
//
//////////////////////////////////////////////////////////////////////////
int ModelLoadManager::LoadModelFileName(TCHAR* filename)
{
	FILE* fp = _tfopen(L"data.ini", L"rt");
	if (fp == NULL) return FALSE;

	TCHAR line[256] = L"";
	TCHAR token[256] = L"";

	//�����ڵ� ���ڿ��� ����� �ѱ۱��� ������ ���� ������ ����
	//#include "locale.h" ��� �ʿ�
	//setlocale(LC_ALL, "korean");		// '�ѱ� ������'
	setlocale(LC_ALL, "");				// '�ü�� �⺻��'

	while (1)
	{
		/// ������ ���� ��Ÿ���� ������ �������´�
		if (feof(fp)) break;

		/// ���ڿ��� �����´�
		_fgetts(line, 256, fp);			// ������ ���� ������ fscanf �� ���� ����. 
		/// ������ ������ �����͸� �д´�
		_stscanf(line, L"%s", token);

		/// �� ���ڿ����� ������ ������ ���ڸ� �� 
		/// (�տ� �ΰ�. �� �ּ� ó���Ǿ� ������ ���ڿ� �������� �ʰ� ��� ����)
		if (_tcsnicmp(token, L"//", 2) == 0)
			continue;

		/// ���ڿ� ����
		_tcscpy(filename, token);

		// �����ڵ� ���ڿ��� ����
		//size_t size = ::mbstowcs(NULL, token, 0);
		//size = ::mbstowcs(filename, token, size);
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
BOOL ModelLoadManager::ModelLoad(ModelAse** Obj)
{
	// �ε��� ase ���ϸ��� �ܺ����Ͽ��� �о�´�.
	// ���ϸ��� 'path' �� ���Եȴ�.  
	TCHAR filename[256] = L"";
	if (!LoadModelFileName(filename))
	{
		return FALSE;
	}

	// ���� ������ ���
	/// ������ �� ����ϰ� �����ϴ��� �� �𸣰ڴ�.
	TCHAR nowdir[256] = L"";
	::GetCurrentDirectory(256, nowdir);

	//-------------------------------------------------------------------
	// ase �ε� : �ε��� 'path' + ase ���ϸ��� �������� 
	// ��θ� ������ �� �۾��� �����Ѵ�.
	//
	// ASE ���Ͽ� ��õ�  �ؽ����� 'path' �ʹ� ������� 
	// ASE ������ �������� �ε��ϱ� ���� ���̴�.
	//-------------------------------------------------------------------


	if (CHECKFAILED(ModelCreateFromASE(D3DEngine::GetIns()->GetDevice(), filename, NULL, &(*Obj))))
	{
		//B3D_ShowLogFile();
		return FALSE;
	}

	// ���� ���� ����
	::SetCurrentDirectory(nowdir);

	return TRUE;
}

BOOL ModelLoadManager::ModelLoadFile(const TCHAR* filename, ModelAse** Obj)
{
	// ���� ������ ���
	TCHAR nowdir[256] = L"";
	::GetCurrentDirectory(256, nowdir);

	//-------------------------------------------------------------------
	// ase �ε� : �ε��� 'path' + ase ���ϸ��� �������� 
	// ��θ� ������ �� �۾��� �����Ѵ�.
	//
	// ASE ���Ͽ� ��õ�  �ؽ����� 'path' �ʹ� ������� 
	// ASE ������ �������� �ε��ϱ� ���� ���̴�.
	//-------------------------------------------------------------------

	if (CHECKFAILED(ModelCreateFromASE(D3DEngine::GetIns()->GetDevice(), filename, NULL, &(*Obj))))
	{
		//B3D_ShowLogFile();
		return FALSE;
	}

	// ���� ���� ����
	::SetCurrentDirectory(nowdir);

	return TRUE;
}

void ModelLoadManager::Release()
{
	SAFE_DELETE(m_pTile1);
	SAFE_DELETE(m_pTile2);
	SAFE_DELETE(m_pScv_Idle);
	SAFE_DELETE(m_pScv_Work);
	SAFE_DELETE(m_pScv_Die);
	SAFE_DELETE(m_pBow_Idle);
	SAFE_DELETE(m_pBow_Attack);
	SAFE_DELETE(m_pBow_Die);
	SAFE_DELETE(m_pKnight_Idle);
	SAFE_DELETE(m_pKnight_Attack);
	SAFE_DELETE(m_pKnight_Die);
	SAFE_DELETE(m_pSword_Idle);
	SAFE_DELETE(m_pSword_Attack);
	SAFE_DELETE(m_pSword_Die);
	SAFE_DELETE(m_pShield_Idle);
	SAFE_DELETE(m_pShield_Attack);
	SAFE_DELETE(m_pShield_Die);
	SAFE_DELETE(m_pCastle);
	SAFE_DELETE(m_pWheat);
	SAFE_DELETE(m_pIron);
	SAFE_DELETE(m_pHorse);
	SAFE_DELETE(m_pGroundObj);

	SAFE_DELETE(m_Instance);
}
