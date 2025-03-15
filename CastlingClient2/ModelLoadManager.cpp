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
// 외부파일에서 ASE 파일명 구하기
//
//////////////////////////////////////////////////////////////////////////
int ModelLoadManager::LoadModelFileName(TCHAR* filename)
{
	FILE* fp = _tfopen(L"data.ini", L"rt");
	if (fp == NULL) return FALSE;

	TCHAR line[256] = L"";
	TCHAR token[256] = L"";

	//유니코드 문자열로 복사시 한글깨짐 방지를 위해 로케일 변경
	//#include "locale.h" 헤더 필요
	//setlocale(LC_ALL, "korean");		// '한글 로케일'
	setlocale(LC_ALL, "");				// '운영체제 기본값'

	while (1)
	{
		/// 파일의 끝이 나타나면 루프를 빠져나온다
		if (feof(fp)) break;

		/// 문자열을 가져온다
		_fgetts(line, 256, fp);			// 라인의 공백 때문에 fscanf 는 쓸수 없다. 
		/// 형식이 지정된 데이터를 읽는다
		_stscanf(line, L"%s", token);

		/// 두 문자열에서 지정된 개수의 문자를 비교 
		/// (앞에 두개. 즉 주석 처리되어 있으면 문자열 복사하지 않고 계속 진행)
		if (_tcsnicmp(token, L"//", 2) == 0)
			continue;

		/// 문자열 복사
		_tcscpy(filename, token);

		// 유니코드 문자열로 복사
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
	// 로드할 ase 파일명을 외부파일에서 읽어온다.
	// 파일명은 'path' 가 포함된다.  
	TCHAR filename[256] = L"";
	if (!LoadModelFileName(filename))
	{
		return FALSE;
	}

	// 현재 폴더를 백업
	/// 폴더를 왜 백업하고 복구하는지 잘 모르겠다.
	TCHAR nowdir[256] = L"";
	::GetCurrentDirectory(256, nowdir);

	//-------------------------------------------------------------------
	// ase 로드 : 로드한 'path' + ase 파일명을 기준으로 
	// 경로를 변경한 후 작업을 수행한다.
	//
	// ASE 파일에 명시된  텍스쳐의 'path' 와는 상관없이 
	// ASE 파일의 폴더에서 로드하기 위한 것이다.
	//-------------------------------------------------------------------


	if (CHECKFAILED(ModelCreateFromASE(D3DEngine::GetIns()->GetDevice(), filename, NULL, &(*Obj))))
	{
		//B3D_ShowLogFile();
		return FALSE;
	}

	// 이전 폴더 복구
	::SetCurrentDirectory(nowdir);

	return TRUE;
}

BOOL ModelLoadManager::ModelLoadFile(const TCHAR* filename, ModelAse** Obj)
{
	// 현재 폴더를 백업
	TCHAR nowdir[256] = L"";
	::GetCurrentDirectory(256, nowdir);

	//-------------------------------------------------------------------
	// ase 로드 : 로드한 'path' + ase 파일명을 기준으로 
	// 경로를 변경한 후 작업을 수행한다.
	//
	// ASE 파일에 명시된  텍스쳐의 'path' 와는 상관없이 
	// ASE 파일의 폴더에서 로드하기 위한 것이다.
	//-------------------------------------------------------------------

	if (CHECKFAILED(ModelCreateFromASE(D3DEngine::GetIns()->GetDevice(), filename, NULL, &(*Obj))))
	{
		//B3D_ShowLogFile();
		return FALSE;
	}

	// 이전 폴더 복구
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
