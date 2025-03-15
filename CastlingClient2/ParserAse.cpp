#include "D3DEngine.h"
#include "ModelAse.h"
#include "ParserAse.h"
#include "string"

using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// ParserAse : Static Functions
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// void GetFileName(...)   
//
// 특정 파일명 'FullPath' 에서 '실제 파일명' 만 얻어내는 유틸 메소드
// <STL 버전>
// EX) 'xx\\xxxx\\xxxx\\demo.jpg' 또는 'xx/xxxx/xxxx/demo.jpg' 에서 
//		'demo.jpg' 얻기
//
//////////////////////////////////////////////////////////////////////////
void ParserAse::GetFileName(
	const TCHAR* FullPath,	//'Full-Path' 의 소스 포인터
	TCHAR* FileName		//'demo.jpg' 가 리턴될 포인터
)
{
	// 파일명 저장
	std::wstring name;
	name = FullPath;
	size_t pos = name.find_last_of('\\');
	if (pos <= 0)
	{
		pos = name.find_last_of('/');		// 실패 시, 슬래시로도 검사
	}
	//if(pos <0) return;	

	_tcscpy(FileName, &name[pos + 1]);
}


//////////////////////////////////////////////////////////////////////////
//
// void ParserAse::GetPath(...)  
// 
// PathName 에서 Path 를 분리 <STL 버전>
// ex) "xx/xxxx/demo.jpg" -> "xx/xxxx" 를 얻음
//
//////////////////////////////////////////////////////////////////////////
void ParserAse::GetPath(const TCHAR* FullPathName, TCHAR* Path)
{
	std::wstring path;
	path = FullPathName;

	size_t pos = path.find_last_of('\\');
	if (pos <= 0)
	{
		pos = path.find_last_of('/');
	}

	_tcsncpy(Path, FullPathName, pos);		// pos 만큼 복사
}

void ParserAse::GetPath1(const TCHAR* FullPathName, TCHAR* Path)
{
	std::wstring path;
	path = FullPathName;

	size_t pos = path.find_last_of('/');
	if (pos <= 0)
	{
		// 실패시 '\\'로 다시 시도
		pos = path.find_last_of('\\');
	}

	// 찾아낸 가장 끝 '\\', '/' 까지 복사
	_tcsncpy(Path, FullPathName, pos + 1);
}


int ParserAse::m_Count = 0;

int ParserAse::m_Depth = 0;

//////////////////////////////////////////////////////////////////////////
// 
// class ParserAse : Member Functions
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// 생성자 & 소멸자
//
//////////////////////////////////////////////////////////////////////////
ParserAse::ParserAse()
{
	m_pModel = NULL;
	m_LineCount = 0;
	m_fp = NULL;
	_tcscpy(m_CurrLine, L"----");
}


ParserAse::~ParserAse()
{

}


//////////////////////////////////////////////////////////////////////////
//
// _ReadLine : 파일의 한 줄씩 읽는다.
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_ReadLine(TCHAR* buff)
{
	_tcscpy(buff, L"******");		//버퍼 클리어. 비교를위한 임시코드..
	_fgetts(buff, 256, m_fp);		//읽어들인 데이터 복사.
	m_LineCount++;					//읽어들인 라인수 증가..(디버깅용)

	return CHK_OK;

}


//////////////////////////////////////////////////////////////////////////
//
// ASE 파일 읽기 최상위 레벨 : 진입점 함수
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::Load(ID3D11Device* pDev, const TCHAR* filename, ModelAse* pModel)
{
	TCHAR token[256] = L"";

	assert(pDev != NULL);

	// 1-1. 파일 열기
	m_fp = _tfopen(filename, L"rt");
	if (m_fp == NULL)
	{
		return CHK_ERROR;
	}

	// 1-2. 파일 헤더 검사
	_ReadLine(m_CurrLine);				// 1줄 읽기
	BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);	// 현재 줄에서 첫번째 '단어' 읽기

	if (NOT_EQUAL(token, L"*3DSMAX_ASCIIEXPORT")) return CHK_ERROR;

	// 검사 완료

	// 2. 외부 파일 등록
	assert(pModel != NULL);
	m_pModel = pModel;

	// 3. 파일명 저장
	ParserAse::GetFileName(filename, m_pModel->m_FileName);

	// 4. 정상 데이터, 파일 끝까지 읽기
	while (1)
	{
		if (feof(m_fp)) break;

		_ReadLine(m_CurrLine);					// 1줄 읽기
		bCheck = _stscanf(m_CurrLine, L"%s", token);		// 현재 줄에서 첫번째 '단어' 읽기
		// 주석
		if (EQUAL(token, L"*COMMENT")) continue;	//*COMMENT

		// Scene 정보
		else if (EQUAL(token, L"*SCENE"))
		{
			_LoadSceneInfo();
		}

		// 재질 데이터 - Node 정보 읽기
		else if (EQUAL(token, L"*MATERIAL_LIST"))	//*MATERIAL_LIST
		{
			///OutputDebugStringW(L"MATERIAL_LIST");

			_LoadMaterialList();
			continue;
		}


		// 기하데이터 - Node 정보 읽기
		else if (EQUAL(token, L"*GEOMOBJECT"))		//*GEOMOBJECT
		{
			// 함수 내부에서 Geometry - Node 개체 생성. 외부에 등록.
			_LoadGeomObject();
			continue;
		}

		//각 정보별 읽기 코드를 추가할 예정

	}

	// 5. 전체 ASE 작업 읽기 완료 
	fclose(m_fp);

	// 계층 구조 만들기
	_CreateHierarchy();

	// 노말 개수에 따른 데이터 새로 집어넣기
	_SetNewVertexList();

	// 텍스처 로드
	_LoadTexture(filename);

	// TM 만들기

	for (unsigned int i = 0; i < m_pModel->m_RootList.size(); i++)
	{
		m_pModel->CreateTM(m_pModel->m_RootList[i]);

		SetQuaternionRotation(m_pModel->m_RootList[i]);
	}

	///m_pModel->CreateTM(m_pModel->m_Root);

	///SetQuaternionRotation(m_pModel->m_Root);

	//return m_LineCount;		// 총 읽어들인 라인수 리턴 (디버깅용)
	return CHK_OK;
}

int ParserAse::_LoadSceneInfo()
{
	TCHAR token[256] = L"";
	UINT materialNum = 0;

	// 모델 정보 확인
	if (m_pModel == NULL) return CHK_ERROR;

	while (1)
	{
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		// 노드 이름
		if (EQUAL(token, L"*SCENE_FIRSTFRAME"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *SCENE_FIRSTFRAME %d", &m_pModel->m_SceneFirstFrame);
		}

		else if (EQUAL(token, L"*SCENE_LASTFRAME"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *SCENE_LASTFRAME %d", &m_pModel->m_SceneLastFrame);
		}

		else if (EQUAL(token, L"*SCENE_FRAMESPEED"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *SCENE_FRAMESPEED %d", &m_pModel->m_SceneFrameSpeed);
		}

		else if (EQUAL(token, L"*SCENE_TICKSPERFRAME"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *SCENE_TICKSPERFRAME %d", &m_pModel->m_SceneTickPerFrame);
		}

		// 닫기
		if (EQUAL(token, L"}")) break;

	}
	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 재질 정보
// MATERIAL_COUNT...
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadMaterialList()
{
	TCHAR token[256] = L"";
	UINT materialNum = 0;

	// 모델 정보 확인
	if (m_pModel == NULL) return CHK_ERROR;

	while (1)
	{
		_ReadLine(m_CurrLine);					// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);		// 현재 줄에서 첫번째 '단어' 읽기

		// 노드 이름
		if (EQUAL(token, L"*MATERIAL_COUNT"))			//*MATERIAL_COUNT
		{
			int num = 0;
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_COUNT %d", &num);

			m_pModel->m_pMaterialList = new MATERIAL_ASE[num];
			assert(m_pModel->m_pMaterialList != NULL);
			ZeroMemory(m_pModel->m_pMaterialList, sizeof(MATERIAL_ASE) * num);

			m_pModel->m_MaterialCnt = num;

			continue;
		}

		// 노드 행렬
		else if (EQUAL(token, L"*MATERIAL"))			//*MATERIAL
		{
			_LoadMaterial(materialNum);
			materialNum++;
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			// '}' - *MATERIAL_LIST 데이터의 끝
	}


	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 지오메트리 정보 읽기
//
// [읽기 내역]
// *GEOMOBJECT {	....	}
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadGeomObject()
{
	///OutputDebugStringW(L"_LoadGeomObject");

	TCHAR token[256] = L"";

	// 모델 정보 확인
	if (m_pModel == NULL) return CHK_ERROR;

	NodeAse* nowNode = new NodeAse();

	while (1)
	{
		_ReadLine(m_CurrLine);					// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);		// 현재 줄에서 첫번째 '단어' 읽기

		// 노드 이름
		if (EQUAL(token, L"*NODE_NAME"))			//*NODE_NAME
		{
			bCheck = _stscanf(m_CurrLine, L"\t *NODE_NAME \"%[^\"]\"", nowNode->m_NodeName);
			continue;
		}

		// 부모 이름
		else if (EQUAL(token, L"*NODE_PARENT"))			//*NODE_PARENT
		{
			bCheck = _stscanf(m_CurrLine, L"\t *NODE_PARENT \"%[^\"]\"", nowNode->m_NodeParentName);
			continue;
		}

		// 노드 행렬
		else if (EQUAL(token, L"*NODE_TM"))			//*NODE_TM
		{
			_LoadTM(nowNode);
			continue;
		}

		// 메쉬 정보
		else if (EQUAL(token, L"*MESH"))				//*MESH
		{
			///OutputDebugStringW(L"_LoadMesh");
			_LoadMesh(nowNode);
			continue;
		}

		// 기타 키워드 무시..
		//*PROP_MOTIONBLUR 0
		//*PROP_CASTSHADOW 1
		//*PROP_RECVSHADOW 1

		else if (EQUAL(token, L"*TM_ANIMATION"))
		{
			_LoadAnimation(nowNode);
			continue;
		}

		// 내가 몇번 재질을 사용하는지...?
		else if (EQUAL(token, L"*MATERIAL_REF"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_REF %d", &nowNode->m_MaterialRef);
			continue;
		}

		// 스킨
		else if (EQUAL(token, L"*SKIN"))
		{
			_LoadSkin(nowNode);
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			// '}' - *GEOMOBJECT 데이터의 끝
	}

	///m_pModel->m_pNodeList.push_back(nowNode);
	if (NULL != _tcsstr(nowNode->m_NodeName, L"Bone")
		|| NULL != _tcsstr(nowNode->m_NodeName, L"Bip"))
	{
		m_pModel->m_pBoneList.push_back(nowNode);
	}
	else
	{
		m_pModel->m_pNodeList.push_back(nowNode);
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// TM 행렬 정보 읽기
// 
// [ 읽기 내역]
// *NODE_TM {	....	}
// 
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadTM(NodeAse* node)
{
	TCHAR token[256] = L"";
	MATRIX mTM = Matrix::Identity;
	VECTOR3 vPos = VECTOR3(0, 0, 0);
	VECTOR3 vRotAxis = VECTOR3(0, 0, 0);
	VECTOR3 vScale = VECTOR3(0, 0, 0);
	VECTOR3 vScaleAxis = VECTOR3(0, 0, 0);

	while (1)
	{
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		//
		if (EQUAL(token, L"*INHERIT_POS"))
		{
			continue;
		}

		else if (EQUAL(token, L"*INHERIT_ROT"))
		{
			continue;
		}

		else if (EQUAL(token, L"*INHERIT_SCL"))
		{
			continue;
		}

		else if (EQUAL(token, L"*TM_ROW0"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_ROW0 %f %f %f", &mTM._11, &mTM._13, &mTM._12);
			continue;
		}

		else if (EQUAL(token, L"*TM_ROW1"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_ROW1 %f %f %f", &mTM._31, &mTM._33, &mTM._32);
			continue;
		}

		else if (EQUAL(token, L"*TM_ROW2"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_ROW2 %f %f %f", &mTM._21, &mTM._23, &mTM._22);
			continue;
		}

		else if (EQUAL(token, L"*TM_ROW3"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_ROW3 %f %f %f", &mTM._41, &mTM._43, &mTM._42);
			continue;
		}

		else if (EQUAL(token, L"*TM_POS"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_POS %f %f %f", &vPos.x, &vPos.z, &vPos.y);
			continue;
		}

		else if (EQUAL(token, L"*TM_ROTAXIS"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_ROTAXIS %f %f %f", &vRotAxis.x, &vRotAxis.z, &vRotAxis.y);
			continue;
		}

		else if (EQUAL(token, L"*TM_ROTANGLE"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_ROTANGLE %f", &node->m_vRotAngle);
			continue;
		}

		else if (EQUAL(token, L"*TM_SCALE"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_SCALE %f %f %f", &vScale.x, &vScale.z, &vScale.y);
			continue;
		}

		else if (EQUAL(token, L"*TM_SCALEAXIS"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_SCALEAXIS %f %f %f", &vScaleAxis.x, &vScaleAxis.z, &vScaleAxis.y);
			continue;
		}

		else if (EQUAL(token, L"*TM_SCALEAXISANG"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *TM_SCALEAXISANG %f", &node->m_vScaleAxisAng);
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;				// '}'
	}
	node->m_NodeTM = mTM;
	node->m_vPos = vPos;
	node->m_vRotAxis = vRotAxis;
	node->m_vScale = vScale;
	node->m_vScaleAxis = vScaleAxis;

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 메쉬 정보 읽기
//
// GeomObject {   
//       Mesh  { ...   <--여기서 부터 파싱
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadMesh(NodeAse* node)
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		//TIMEVALUE 미사용, 무시.  			//*TIMEVALUE :  


		// 정점 개수
		if (EQUAL(token, L"*MESH_NUMVERTEX"))				//'*MESH_NUMVERTEX 3'
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMVERTEX %d", &node->m_VertexCnt);
			continue;
		}

		// 페이스 개수
		else if (EQUAL(token, L"*MESH_NUMFACES"))				//'*MESH_NUMFACES 1'
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMFACES %d", &node->m_FaceCnt);
			continue;
		}

		// 정점 리스트 로드	
		else if (EQUAL(token, L"*MESH_VERTEX_LIST"))				//'*MESH_VERTEX_LIST {'
		{
			node->m_NormalCnt = node->m_FaceCnt * 3;
			_LoadVertexList(node);
			continue;
		}

		// 페이스별 정점 리스트 (Index Buffer) 정보 로드
		else if (EQUAL(token, L"*MESH_FACE_LIST"))				//'*MESH_FACE_LIST {'
		{
			_LoadFaceList(node);
			continue;
		}

		// UV 관련 로드
		else if (EQUAL(token, L"*MESH_NUMTVERTEX"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMTVERTEX %d", &node->m_TVertexCnt);
			continue;
		}

		// UV 관련 로드
		else if (EQUAL(token, L"*MESH_TVERTLIST"))
		{
			_LoadTVertList(node);
			continue;
		}

		// UV 관련 로드
		else if (EQUAL(token, L"*MESH_NUMTVFACES"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMTVFACES %d", &node->m_TFaceCnt);
			continue;
		}

		// UV 관련 로드
		else if (EQUAL(token, L"*MESH_TFACELIST"))
		{
			_LoadTFaceList(node);
			continue;
		}

		// 노말 정보 로드
		else if (EQUAL(token, L"*MESH_NORMALS"))				//'*MESH_NORMALS {'
		{
			///OutputDebugStringW(L"MESH_NORMALS");
			_LoadNormalList(node);
			continue;
		}

		// 정점 색 리스트
		else if (EQUAL(token, L"*MESH_NUMCVERTEX"))				//'*MESH_NUMCVERTEX 6'
		{
			_LoadCVertexList(node);
			continue;
		}

		// 페이스별 정점색 인덱스 리스트 로드
		else if (EQUAL(token, L"*MESH_NUMCVFACES"))				//'*MESH_NUMCVFACES 1'
		{
			_LoadCFaceList(node);
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}'  
	}

	return CHK_OK;

}


//////////////////////////////////////////////////////////////////////////
//
// 재질 정보 읽기
//
// [읽기 내역]
// 	*MATERIAL 0 {
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadMaterial(UINT num)
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);


		if (EQUAL(token, L"*MATERIAL_NAME"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_NAME \"%[^\"]\"", m_pModel->m_pMaterialList[num].MaterialName);
			continue;
		}

		else if (EQUAL(token, L"*MATERIAL_CLASS"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_CLASS \"%[^\"]\"", m_pModel->m_pMaterialList[num].MaterialClass);
			continue;
		}

		else if (EQUAL(token, L"*MATERIAL_AMBIENT"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_AMBIENT %f %f %f",
				&m_pModel->m_pMaterialList[num].Ambient.x,
				&m_pModel->m_pMaterialList[num].Ambient.y,
				&m_pModel->m_pMaterialList[num].Ambient.z
			);
			continue;
		}

		else if (EQUAL(token, L"*MATERIAL_DIFFUSE"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_DIFFUSE %f %f %f",
				&m_pModel->m_pMaterialList[num].Diffuse.x,
				&m_pModel->m_pMaterialList[num].Diffuse.y,
				&m_pModel->m_pMaterialList[num].Diffuse.z
			);
			continue;
		}

		else if (EQUAL(token, L"*MATERIAL_SPECULAR"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_SPECULAR %f %f %f",
				&m_pModel->m_pMaterialList[num].SpecularCol.x,
				&m_pModel->m_pMaterialList[num].SpecularCol.y,
				&m_pModel->m_pMaterialList[num].SpecularCol.z
			);
			continue;
		}

		/// specular power
		/// specular = shine * 100
		else if (EQUAL(token, L"*MATERIAL_SHINE"))
		{
			float shine = 0.0f;
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_SHINE %f", &shine);
			m_pModel->m_pMaterialList[num].Power = shine * 100;
			continue;
		}

		/// specular
		/// specular = (r, g, b) * ShineStrength
		else if (EQUAL(token, L"*MATERIAL_SHINESTRENGTH"))
		{
			float specular = 0.0f;
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_SHINESTRENGTH %f", &specular);
			m_pModel->m_pMaterialList[num].Specular.x = m_pModel->m_pMaterialList[num].SpecularCol.x * specular;
			m_pModel->m_pMaterialList[num].Specular.y = m_pModel->m_pMaterialList[num].SpecularCol.y * specular;
			m_pModel->m_pMaterialList[num].Specular.z = m_pModel->m_pMaterialList[num].SpecularCol.z * specular;
			m_pModel->m_pMaterialList[num].Specular.w = m_pModel->m_pMaterialList[num].SpecularCol.w * specular;
			continue;
		}

		/// transparency (alpha)
		/// alpha = 1 - transparency
		else if (EQUAL(token, L"*MATERIAL_TRANSPARENCY"))
		{
			float transparency = 0.0f;
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_TRANSPARENCY %f", &transparency);
			m_pModel->m_pMaterialList[num].SpecularCol.w = 1 - transparency;
			continue;
		}

		else if (EQUAL(token, L"*MATERIAL_WIRESIZE"))
		{
			continue;
		}

		else if (EQUAL(token, L"*MAP_GENERIC"))
		{
			continue;
		}

		// 비트맵
		else if (EQUAL(token, L"*MAP_DIFFUSE"))
		{
			_LoadMapDiffuse(num);
			continue;
		}

		else if (EQUAL(token, L"*MAP_OPACITY"))
		{
			continue;
		}

		else if (EQUAL(token, L"*MAP_REFLECT"))
		{
			_LoadMapReflect();
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// MAP_DIFFUSE
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadMapDiffuse(UINT num)
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MAP_NAME"))
		{
			continue;
		}

		else if (EQUAL(token, L"*MAP_CLASS"))
		{
			continue;
		}

		else if (EQUAL(token, L"*MAP_AMOUNT"))
		{
			continue;
		}

		else if (EQUAL(token, L"*BITMAP"))
		{
			TCHAR path[256] = L"";
			bCheck = _stscanf(m_CurrLine, L"\t *BITMAP \"%[^\"]\"", path);
			GetFileName(path, m_pModel->m_pMaterialList[num].m_TextureName);
			continue;
		}

		else if (EQUAL(token, L"*UVW_U_OFFSET"))
		{
			continue;
		}

		else if (EQUAL(token, L"*UVW_V_OFFSET"))
		{
			continue;
		}

		else if (EQUAL(token, L"*UVW_U_TILING"))
		{
			continue;
		}

		else if (EQUAL(token, L"*UVW_V_TILING"))
		{
			continue;
		}

		else if (EQUAL(token, L"*UVW_ANGLE"))
		{
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// MAP_REFLECT
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadMapReflect()
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MAP_NAME"))
		{
			continue;
		}

		else if (EQUAL(token, L"*MAP_CLASS"))
		{
			continue;
		}

		else if (EQUAL(token, L"*MAP_SUBNO"))
		{
			continue;
		}

		else if (EQUAL(token, L"*MAP_AMOUNT"))
		{
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 텍스쳐 로드
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadTexture(const TCHAR* filename)
{
	TCHAR path[256] = L"";
	TCHAR texFilename[256] = L"";

	GetPath1(filename, path);

	for (unsigned int i = 0; i < m_pModel->m_MaterialCnt; i++)
	{
		_tcscpy(texFilename, path);

		/// 텍스처 없을 때 처리를 이렇게 해줘야하나..? 
		if (EQUAL(m_pModel->m_pMaterialList[i].m_TextureName, L""))
			continue;

		_tcscat(texFilename, m_pModel->m_pMaterialList[i].m_TextureName);
		D3DEngine::GetIns()->LoadTexture(texFilename, &m_pModel->m_pMaterialList[i].m_Texture);
	}

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// 정점 리스트 얻기
//
//  [읽기 내역]
// *MESH_VERTEX_LIST {
//			*MESH_VERTEX    0	10.0000	0.0000	0.0000  <-- 여기서 부터 읽는다.
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadVertexList(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT cnt = 0;
	UINT num = 0;

	VECTOR3 pos;

	//정점 리스트 생성 및 초기화. 
	node->m_pVertList = new VERTEX_ASE[node->m_VertexCnt];
	assert(node->m_pVertList != NULL);
	ZeroMemory(node->m_pVertList, sizeof(VERTEX_ASE) * node->m_VertexCnt);

	// 정점 개수만큼 읽어야 한다.
	while (1)
	{
		// 1줄 읽기 ->  '*MESH_VERTEX  0	10.0000	0.0000	0.0000'		 
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MESH_VERTEX"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_VERTEX %d %f %f %f",
				&num,
				&pos.x,
				&pos.z,	// <- 방향주의! Z와 Y 반대로  
				&pos.y	// <- 방향주의! 
			);

			XMMATRIX mTM = XMMatrixIdentity();
			XMVECTOR vec = XMLoadFloat3(&pos);
			vec = XMVector3Transform(vec, XMMatrixInverse(nullptr, XMLoadFloat4x4(&node->m_NodeTM)));

			XMStoreFloat3(&pos, vec);

			node->m_pVertList[cnt].vPos.x = pos.x;
			node->m_pVertList[cnt].vPos.y = pos.y;
			node->m_pVertList[cnt].vPos.z = pos.z;

			cnt++;
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}'  
	}

	// 오류 검증 : 정점 개수만큼 루프를 돌아야 한다.
	if (cnt != node->m_VertexCnt)
	{
		// error
		return CHK_ERROR;
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 페이스 리스트 읽기 : '인덱스' 버퍼를 위해 필요하다.
//
// [읽기 내역]
//	*MESH_FACE  379:	A:  410		B:  408		C:  409		AB:    1  BC:    1 CA:    0	  *MESH_SMOOTHING 1 	*MESH_MTLID 3
// 
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadFaceList(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT num = 0;
	UINT cnt = 0;

	// 페이스 리스트 생성 및 초기화
	node->m_pFaceList = new FACE_ASE[node->m_FaceCnt];
	assert(node->m_pFaceList != NULL);
	ZeroMemory(node->m_pFaceList, sizeof(FACE_ASE) * node->m_FaceCnt);

	// 삼각형 개수만큼 읽어야 한다.
	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		// *MESH_FACE  379:  A: 410  B: 408  C: 409  AB: 1   BC: 1  CA: 0	*MESH_SMOOTHING 1 	*MESH_MTLID 3
		if (EQUAL(token, L"*MESH_FACE"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_FACE %d:  A: %d  B: %d  C: %d",
				//" AB: %d  BC: %d  CA: %d "			 --> 무시..
				//" *MESH_SMOOTHING %d  *MESH_MTLID %d", --> 무시.. 
				&num,			// '*MESH_FACE 379: 페이스 번호. IB 의 그것과 동일. 무시'
				&node->m_pFaceList[cnt].A,	// 'A: XX' 
				&node->m_pFaceList[cnt].C,	// 'B: XX' <- 순서 주의 
				&node->m_pFaceList[cnt].B	// 'C: XX' <- 순서 주의
			);

			cnt++;
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	// 오류 검증 : 삼각형 개수만큼 읽어야 한다.
	if (cnt != node->m_FaceCnt)
	{
		// error
		return CHK_ERROR;
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// MESH_TVERT
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadTVertList(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT num = 0;
	float num2 = 0;
	float temp1, temp2 = 0.0f;
	UINT cnt = 0;

	node->m_pTVertList = new TVERT_ASE[node->m_TVertexCnt];
	assert(node->m_pTVertList != NULL);
	ZeroMemory(node->m_pTVertList, sizeof(TVERT_ASE) * node->m_TVertexCnt);

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MESH_TVERT"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_TVERT %d %f %f %f",
				&num,
				&temp1,
				&temp2,
				&num2
			);
			node->m_pTVertList[num].uvPos.x = temp1;
			node->m_pTVertList[num].uvPos.y = 1 - temp2;

			cnt++;
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// MESH_TFACE
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadTFaceList(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT num = 0;
	UINT cnt = 0;

	node->m_pTFaceList = new FACE_ASE[node->m_TFaceCnt];
	assert(node->m_pTFaceList != NULL);
	ZeroMemory(node->m_pTFaceList, sizeof(FACE_ASE) * node->m_TFaceCnt);

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MESH_TFACE"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_TFACE %d %d %d %d",
				&num,
				&node->m_pTFaceList[cnt].A,
				&node->m_pTFaceList[cnt].C,
				&node->m_pTFaceList[cnt].B
			);

			cnt++;
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// 노말 리스트 읽기
//
// [읽기 내역]
		// *MESH_FACENORMAL 0	0.0000	0.0000	-1.0000
// 
//////////////////////////////////////////////////////////////////////////
int	ParserAse::_LoadNormalList(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT num = 0;
	UINT numExtra = 0;
	UINT facecnt = 0;
	UINT vtxcnt = 0;

	VECTOR3 nrm = VECTOR3(0, 0, 0);
	MATRIX mTM = node->m_NodeTM;

	// 노말 리스트 생성 및 초기화
	node->m_pNormalList = new NORMAL_ASE[node->m_NormalCnt];
	assert(node->m_pNormalList != NULL);
	ZeroMemory(node->m_pNormalList, sizeof(NORMAL_ASE) * node->m_NormalCnt);

	// 노말 개수만큼 읽어야 한다.
	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		// *MESH_FACENORMAL 0	0.0000	0.0000	-1.0000
		if (EQUAL(token, L"*MESH_FACENORMAL"))
		{
			/// 여기는 현재 파싱만 할 뿐 쓰이지 않는다.
			///*
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_FACENORMAL %d %f %f %f",
				&num,
				&node->m_pNormalList[num].vfaceNormal.x,
				&node->m_pNormalList[num].vfaceNormal.z,	// <- 순서 주의 
				&node->m_pNormalList[num].vfaceNormal.y		// <- 순서 주의
			);
			//*/

			facecnt++;
			continue;
		}
		// *MESH_VERTEXNORMAL 0	0.0000	0.0000	-1.0000
		else if (EQUAL(token, L"*MESH_VERTEXNORMAL"))
		{
			/// 어차피 Face List 기준 Face 별 정점 선택하고 노말은 순차적으로 넣어주면 되기 때문에 
			/// 굳이 index로 가지고 있을 필요가 없다.


			/// 노말 위치 바꾼 최종 데이터는 여기가 아니라 풀 버텍스 구성할 때이기 때문에
			/// 거기서 교정해주어야 한다.
			///
			/// 노말 교정 시도해본 것
			/*
			_stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f", &num, &nrm.x, &nrm.z, &nrm.y);

			///nrm = nrm * mTM;
			XMVECTOR vec = XMVector3TransformNormal(XMLoadFloat3(&nrm), XMLoadFloat4x4(&mTM));
			XMStoreFloat3(&nrm, XMVector3Normalize(vec));

			node->m_pNormalList[num].vtxNormal[vtxcnt] = nrm;
			//*/


			/// 노말 교정 직접 곱해보기
			/*
			_stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f", &num, &nrm.x, &nrm.z, &nrm.y);

			XMVECTOR vNrm = XMLoadFloat3(&nrm);
			VECTOR3  newNrm = VECTOR3(0, 0, 0);
			XMMATRIX newTM = XMMatrixIdentity();

			newTM = XMLoadFloat4x4(&node->m_mTM);

			newNrm.x = nrm.x * node->m_mTM._11 + nrm.y * node->m_mTM._21 + nrm.z * node->m_mTM._31;
			newNrm.y = nrm.x * node->m_mTM._12 + nrm.y * node->m_mTM._22 + nrm.z * node->m_mTM._32;
			newNrm.z = nrm.x * node->m_mTM._13 + nrm.y * node->m_mTM._23 + nrm.z * node->m_mTM._33;

			XMStoreFloat3(&newNrm, XMVector3Normalize(XMLoadFloat3(&newNrm)));

			node->m_pNormalList[num].vtxNormal[vtxcnt] = newNrm;
			//*/

			/// 노말 교정 이전 코드
			/// 읽을 때 노말B와 노말C의 위치 변경
			///*
			bCheck = _stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f",
				&numExtra,
				&node->m_pNormalList[num].vtxNormal[0].x,
				&node->m_pNormalList[num].vtxNormal[0].z,		// <- 순서 주의 
				&node->m_pNormalList[num].vtxNormal[0].y		// <- 순서 주의
			);
			//*/

			_ReadLine(m_CurrLine);				// 1줄 읽기 
			bCheck = _stscanf(m_CurrLine, L"%s", token);

			bCheck = _stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f",
				&numExtra,
				&node->m_pNormalList[num].vtxNormal[2].x,
				&node->m_pNormalList[num].vtxNormal[2].z,		// <- 순서 주의 
				&node->m_pNormalList[num].vtxNormal[2].y		// <- 순서 주의
			);

			_ReadLine(m_CurrLine);				// 1줄 읽기 
			bCheck = _stscanf(m_CurrLine, L"%s", token);

			bCheck = _stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f",
				&numExtra,
				&node->m_pNormalList[num].vtxNormal[1].x,
				&node->m_pNormalList[num].vtxNormal[1].z,		// <- 순서 주의 
				&node->m_pNormalList[num].vtxNormal[1].y		// <- 순서 주의
			);

			//vtxcnt++;

			//if (vtxcnt >= 3)
			//	vtxcnt = 0;

			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	// 오류 검증 : 삼각형 개수만큼 읽어야 한다.
	if (facecnt != node->m_NormalCnt)
	{
		// error
		return CHK_ERROR;
	}

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// 정점 컬러 - Diffuse color  - 읽기 
//
// [읽기 내역]
// *MESH_NUMCVERTEX 6
// *MESH_CVERTLIST {
//		*MESH_VERTCOL 0	1.0000	1.0000	1.0000
//      
//////////////////////////////////////////////////////////////////////////
int	ParserAse::_LoadCVertexList(NodeAse* node)
{
	TCHAR token[256] = L"";
	int cnt;

	// 정점 색상 개수 읽기
	BOOL bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMCVERTEX %d", &cnt);		//'*MESH_NUMCVERTEX 6'
	if (cnt <= 0) return CHK_ERROR;

	// 색상 데이터를 저장할 버퍼 구성
	node->m_pCVertList = new COLOR[cnt];
	if (node->m_pCVertList == NULL)
	{
		// error
		return CHK_ERROR;
	}

	int num;
	COLOR col(1, 1, 1, 1);

	//-------------------------------------------------------------------
	// 정점 색상 리스트 읽기
	//-------------------------------------------------------------------
	_ReadLine(m_CurrLine);				// 1줄 읽기 -> '*MESH_CVERTLIST {'

	for (int i = 0; i < cnt; i++)
	{
		_ReadLine(m_CurrLine);			// 1줄 읽기 -> '*MESH_VERTCOL 0	1.0000	1.0000	1.0000'
		bCheck = _stscanf(m_CurrLine, L"\t *MESH_VERTCOL %d %f %f %f", &num, &col.x, &col.y, &col.z);
		//_stscanf(m_CurrLine, L"\t *MESH_VERTCOL %d %f %f %f", &num, &col.r, &col.g, &col.b);

		node->m_pCVertList[num] = col;
	}

	// 닫기
	_ReadLine(m_CurrLine);			// 1줄 읽기 -> '}'

	return CHK_OK;
}




///////////////////////////////////////////////////////////////////////////
//
// 'face 별 정점 색상 인덱스 리스트' 정보 로드
//
// [읽기 내역]
// *MESH_NUMCVFACES 1
// *MESH_CFACELIST {
//			*MESH_CFACE 0	4	5	3
//		}
//
//////////////////////////////////////////////////////////////////////////
int	ParserAse::_LoadCFaceList(NodeAse* node)
{
	TCHAR token[256] = L"";
	int cnt = 0;

	// face 별 정점색 인덱스 개수 얻기 -> '*MESH_NUMCVFACES 1'
	BOOL bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMCVFACES %d", &cnt);
	if (cnt <= 0) return CHK_ERROR;

	// 정점색 - 인덱스 버퍼 구성 
	node->m_pCFaceList = new FACE_ASE[cnt];
	if (node->m_pCFaceList == NULL)
	{
		// error
		return CHK_ERROR;
	}

	//-------------------------------------------------------------------
	// face 별 정점색 인덱스 읽기
	//-------------------------------------------------------------------
	_ReadLine(m_CurrLine);			// 1줄읽기 -> '*MESH_CFACELIST {'

	int fn = 0;
	for (int i = 0; i < cnt; i++)
	{
		_ReadLine(m_CurrLine);		// 1줄 읽기 -> '*MESH_CFACE 0 4 5 3' 

		bCheck = _stscanf(m_CurrLine, L"\t *MESH_CFACE %d %d %d %d",
			&fn,								// face 번호. 
			&node->m_pCFaceList[i].A,		// A C B 순(CCW) 으로 처리. 순서주의.
			&node->m_pCFaceList[i].C,
			&node->m_pCFaceList[i].B
		);
	}

	// 닫기
	_ReadLine(m_CurrLine);		//'}'

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// 애니메이션 관련 로드
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadAnimation(NodeAse* node)
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*NODE_NAME"))
		{
			continue;
		}

		else if (EQUAL(token, L"*CONTROL_POS_TRACK"))
		{
			node->m_isTrans = TRUE;
			_LoadAnimPosTrack(node);
			continue;
		}

		else if (EQUAL(token, L"*CONTROL_ROT_TRACK"))
		{
			node->m_isRot = TRUE;
			_LoadAnimRotTrack(node);
			continue;
		}

		else if (EQUAL(token, L"*CONTROL_SCALE_TRACK"))
		{
			node->m_isScale = TRUE;
			_LoadAnimScaleTrack(node);
			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 애니메이션 관련 로드 - Trans
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadAnimPosTrack(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT  num = 0;

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*CONTROL_POS_SAMPLE"))
		{
			int tickTime = 0;
			VECTOR3 pos = VECTOR3(0, 0, 0);
			bCheck = _stscanf(m_CurrLine, L"\t *CONTROL_POS_SAMPLE %d %f %f %f",
				&tickTime,
				&pos.x,
				&pos.z,
				&pos.y
			);

			if (tickTime != 0 && num == 0)
			{
				KEY_POS firstKey;
				firstKey.tickTime = 0;
				firstKey.Pos = pos;
				node->m_AniKeyPos.push_back(firstKey);
				num++;
			}

			KEY_POS nowKey;
			nowKey.tickTime = tickTime;
			nowKey.Pos = pos;
			node->m_AniKeyPos.push_back(nowKey);


			/// 받은 데이터 넣어줘야함
			//node->m_AniKey[num].tickTime = tickTime;
			//node->m_AniKey[num].Pos = pos;
			num++;

			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 애니메이션 관련 로드 - Rot
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadAnimRotTrack(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT  num = 0;
	KEY_ROT nowKey;

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1줄 읽기 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*CONTROL_ROT_SAMPLE"))
		{
			int tickTime = 0;
			VECTOR3 rot = VECTOR3(0, 0, 0);
			float angle = 0.0f;
			bCheck = _stscanf(m_CurrLine, L"\t *CONTROL_ROT_SAMPLE %d %f %f %f %f",
				&tickTime,
				&rot.x,
				&rot.z,
				&rot.y,
				&angle
			);

			//if (rot.x == 0 && rot.y == 0 && rot.z == 0 && angle == 0)
			//{
			//	num++;
			//	continue;
			//}
			nowKey.ZeroFrame = false;

			if (tickTime != 0 && num == 0)
			{
				KEY_ROT firstKey;
				firstKey.tickTime = 0;
				firstKey.RotAxis = rot;
				firstKey.RotAngle = angle;
				node->m_AniKeyRot.push_back(firstKey);
				num++;

				firstKey.ZeroFrame = true;
				nowKey.ZeroFrame = true;
			}

			nowKey.tickTime = tickTime;
			nowKey.RotAxis = rot;
			nowKey.RotAngle = angle;
			node->m_AniKeyRot.push_back(nowKey);

			/// 받은 데이터 넣어줘야함
			//node->m_AniKey[num].tickTime = tickTime;
			//node->m_AniKey[num].RotAxis = rot;
			//node->m_AniKey[num].RotAngle = angle;
			num++;

			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	//if (node->m_AniKey[0].RotAxis.x == 0
	//	&& node->m_AniKey[0].RotAxis.y == 0
	//	&& node->m_AniKey[0].RotAxis.z == 0
	//	&& node->m_AniKey[0].RotAngle == 0)
	//{
	//	node->m_AniKey[0].RotAxis = node->m_AniKey[1].RotAxis;
	//	node->m_AniKey[0].RotAngle = node->m_AniKey[1].RotAngle;
	//}

	return CHK_OK;
}


int ParserAse::SetQuaternionRotation(NodeAse* node)
{
	//for (int i = 0; i < KEY_MAX_; i++)
	for (unsigned int i = 0; i < node->m_AniKeyRot.size(); i++)
	{
		//ynConsole::Write(L"For i : %d\n", i);

		//1. 구면보간(Sphereical Interpolation) 을 위해 쿼터니온으로 변환.
		VECTOR axis = XMLoadFloat3(&node->m_AniKeyRot[i].RotAxis);
		QUATERNIONA q;
		if (node->m_AniKeyRot[i].RotAngle != 0)
		{
			q = XMQuaternionRotationAxis(axis, node->m_AniKeyRot[i].RotAngle);
		}

		//2.절대 쿼터니온으로 환산 & 저장.
		if (i == 0)
		{
			node->m_AniKeyRot[i].RotQ = q;	// 1번째 키는 "절대값" 으로 판정가능 : 바로 저장.
		}
		else
		{
			if (i == 1 && node->m_AniKeyRot[i].ZeroFrame == true)
			{
				node->m_AniKeyRot[i].RotQ = q;
			}
			else
			{
				node->m_AniKeyRot[i].RotQ = XMQuaternionMultiply(node->m_AniKeyRot[i - 1].RotQ, q);
			}
		}

		//이후, 애니메이션 처리시, 
		//3.구면보간, 중간쿼터니온 계산. 
		//4.보간된 쿼터니온에서 보간된 회전행렬 계산. 
		//  -> 렌더링... 
	}

	/// 이걸 왜 위에 for문 안에 넣었을까..
	for (unsigned int j = 0; j < node->m_Childs.size(); j++)
	{
		SetQuaternionRotation(node->m_Childs[j]);
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 애니메이션 관련 로드 - Scale
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadAnimScaleTrack(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT  num = 0;

	while (1)
	{
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*CONTROL_SCALE_SAMPLE"))
		{
			int tickTime = 0;
			VECTOR3 scale = VECTOR3(0, 0, 0);
			VECTOR3 scaleAxis = VECTOR3(0, 0, 0);
			float angle = 0.0f;
			bCheck = _stscanf(m_CurrLine, L"\t *CONTROL_SCALE_SAMPLE %d %f %f %f %f %f %f %f",
				&tickTime,
				&scale.x,
				&scale.z,
				&scale.y,
				&scaleAxis.x,
				&scaleAxis.z,
				&scaleAxis.y,
				&angle
			);


			if (tickTime != 0 && num == 0)
			{
				KEY_SCALE firstKey;
				firstKey.tickTime = 0;
				firstKey.Scale = scale;
				firstKey.ScaleAxis = scaleAxis;
				firstKey.ScaleAngle = angle;
				node->m_AniKeyScale.push_back(firstKey);
				num++;
			}

			KEY_SCALE nowKey;
			nowKey.tickTime = tickTime;
			nowKey.Scale = scale;
			nowKey.ScaleAxis = scaleAxis;
			nowKey.ScaleAngle = angle;
			node->m_AniKeyScale.push_back(nowKey);

			/// 받은 데이터 넣어줘야함
			//node->m_AniKey[num].Scale = scale;
			//node->m_AniKey[num].ScaleAxis = scaleAxis;
			//node->m_AniKey[num].ScaleAngle = angle;
			num++;

			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}

int ParserAse::_LoadSkin(NodeAse* node)
{
	TCHAR token[256] = L"";
	int weightCnt = 0;
	int skinNum = 0;
	int temp = 0;
	int weightNum = 0;
	node->m_pSkinList = new SKIN_ASE[node->m_VertexCnt];
	ZeroMemory(node->m_pSkinList, sizeof(SKIN_ASE) * node->m_VertexCnt);

	while (1)
	{
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		//
		if (EQUAL(token, L"*VERTEX"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t*VERTEX ID: %d W: %d",
				&node->m_pSkinList[skinNum].vtxID,
				&node->m_pSkinList[skinNum].weightCnt);

			node->m_pSkinList[skinNum].weightList = new WEIGHT_ASE[node->m_pSkinList[skinNum].weightCnt];

			_LoadSkinWeight(&node->m_pSkinList[skinNum]);

			skinNum++;

			continue;
		}

		// 닫기
		if (EQUAL(token, L"}")) break;				// '}'
	}

	return CHK_OK;
}

int ParserAse::_LoadSkinWeight(SKIN_ASE* nowSkin)
{
	TCHAR token[256] = L"";
	int temp = 0;
	int weightNum = 0;

	while (1)
	{
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		//
		if (EQUAL(token, L"*WEIGHT"))
		{
			WEIGHT_ASE* nowWeightList = &nowSkin->weightList[weightNum];

			bCheck = _stscanf(m_CurrLine, L"\t *WEIGHT %d %f BONE: \"%[^\"]\"",
				&temp,
				&nowWeightList->weight,
				&nowWeightList->boneName);

			weightNum++;
			///continue;
		}

		if (weightNum == nowSkin->weightCnt)
			break;
	}

	return CHK_OK;
}

int ParserAse::_CreateHierarchy()
{
	// Bone 먼저 처리
	for (unsigned int i = 0; i < m_pModel->m_pBoneList.size(); i++)
	{
		TCHAR* nowParentName = m_pModel->m_pBoneList[i]->m_NodeParentName;
		NodeAse* findParent = nullptr;
		// 부모가 없다. 내가 최상위
		if (EQUAL(nowParentName, L"NA"))
		{
			m_pModel->m_RootList.push_back(m_pModel->m_pBoneList[i]);
			///m_pModel->m_Root = m_pModel->m_pBoneList[i];
			m_pModel->m_pBoneList[i]->m_Parent = nullptr;
		}
		else
		{
			// 부모 이름으로 노드를 찾는다.
			findParent = m_pModel->FindNodeByName(nowParentName);

			// 그 노드를 나의 부모에 등록한다.
			m_pModel->m_pBoneList[i]->m_Parent = findParent;

			// 부모 노드에 나를 등록한다.
			findParent->m_Childs.push_back(m_pModel->m_pBoneList[i]);
		}
	}

	// Node 처리
	for (unsigned int i = 0; i < m_pModel->m_pNodeList.size(); i++)
	{
		TCHAR* nowParentName = m_pModel->m_pNodeList[i]->m_NodeParentName;
		NodeAse* findParent = nullptr;
		// 부모가 없다. 내가 최상위
		if (EQUAL(nowParentName, L"NA"))
		{
			m_pModel->m_RootList.push_back(m_pModel->m_pNodeList[i]);
			///m_pModel->m_Root = m_pModel->m_pNodeList[i];
			m_pModel->m_pNodeList[i]->m_Parent = nullptr;
		}
		else
		{
			// 부모 이름으로 노드를 찾는다.
			findParent = m_pModel->FindNodeByName(nowParentName);

			// 그 노드를 나의 부모에 등록한다.
			m_pModel->m_pNodeList[i]->m_Parent = findParent;

			// 부모 노드에 나를 등록한다.
			findParent->m_Childs.push_back(m_pModel->m_pNodeList[i]);
		}
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 노말 개수를 기준으로 다시 세팅을 한다.
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_SetNewVertexList()
{
	for (unsigned int i = 0; i < m_pModel->m_pNodeList.size(); i++)
	{
		m_pModel->m_pNodeList[i]->SetNewVertexList();

		// 디버그용 데이터
		m_pModel->m_FullVtxCnt += m_pModel->m_pNodeList[i]->GetFullVertexCnt();
		m_pModel->m_OriginalVtxCnt += m_pModel->m_pNodeList[i]->GetVertexCnt();
		//m_pModel->m_WeldVtxCnt += m_pModel->m_pNodeL[i]->GetWeldVertexCnt();
		m_pModel->m_TotalFaceCnt += m_pModel->m_pNodeList[i]->GetFaceCnt();
	}

	for (unsigned int i = 0; i < m_pModel->m_pBoneList.size(); i++)
	{
		m_pModel->m_pBoneList[i]->SetNewVertexList();
	}

	return CHK_OK;
}
