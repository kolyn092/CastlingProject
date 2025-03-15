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
// Ư�� ���ϸ� 'FullPath' ���� '���� ���ϸ�' �� ���� ��ƿ �޼ҵ�
// <STL ����>
// EX) 'xx\\xxxx\\xxxx\\demo.jpg' �Ǵ� 'xx/xxxx/xxxx/demo.jpg' ���� 
//		'demo.jpg' ���
//
//////////////////////////////////////////////////////////////////////////
void ParserAse::GetFileName(
	const TCHAR* FullPath,	//'Full-Path' �� �ҽ� ������
	TCHAR* FileName		//'demo.jpg' �� ���ϵ� ������
)
{
	// ���ϸ� ����
	std::wstring name;
	name = FullPath;
	size_t pos = name.find_last_of('\\');
	if (pos <= 0)
	{
		pos = name.find_last_of('/');		// ���� ��, �����÷ε� �˻�
	}
	//if(pos <0) return;	

	_tcscpy(FileName, &name[pos + 1]);
}


//////////////////////////////////////////////////////////////////////////
//
// void ParserAse::GetPath(...)  
// 
// PathName ���� Path �� �и� <STL ����>
// ex) "xx/xxxx/demo.jpg" -> "xx/xxxx" �� ����
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

	_tcsncpy(Path, FullPathName, pos);		// pos ��ŭ ����
}

void ParserAse::GetPath1(const TCHAR* FullPathName, TCHAR* Path)
{
	std::wstring path;
	path = FullPathName;

	size_t pos = path.find_last_of('/');
	if (pos <= 0)
	{
		// ���н� '\\'�� �ٽ� �õ�
		pos = path.find_last_of('\\');
	}

	// ã�Ƴ� ���� �� '\\', '/' ���� ����
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
// ������ & �Ҹ���
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
// _ReadLine : ������ �� �پ� �д´�.
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_ReadLine(TCHAR* buff)
{
	_tcscpy(buff, L"******");		//���� Ŭ����. �񱳸����� �ӽ��ڵ�..
	_fgetts(buff, 256, m_fp);		//�о���� ������ ����.
	m_LineCount++;					//�о���� ���μ� ����..(������)

	return CHK_OK;

}


//////////////////////////////////////////////////////////////////////////
//
// ASE ���� �б� �ֻ��� ���� : ������ �Լ�
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::Load(ID3D11Device* pDev, const TCHAR* filename, ModelAse* pModel)
{
	TCHAR token[256] = L"";

	assert(pDev != NULL);

	// 1-1. ���� ����
	m_fp = _tfopen(filename, L"rt");
	if (m_fp == NULL)
	{
		return CHK_ERROR;
	}

	// 1-2. ���� ��� �˻�
	_ReadLine(m_CurrLine);				// 1�� �б�
	BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);	// ���� �ٿ��� ù��° '�ܾ�' �б�

	if (NOT_EQUAL(token, L"*3DSMAX_ASCIIEXPORT")) return CHK_ERROR;

	// �˻� �Ϸ�

	// 2. �ܺ� ���� ���
	assert(pModel != NULL);
	m_pModel = pModel;

	// 3. ���ϸ� ����
	ParserAse::GetFileName(filename, m_pModel->m_FileName);

	// 4. ���� ������, ���� ������ �б�
	while (1)
	{
		if (feof(m_fp)) break;

		_ReadLine(m_CurrLine);					// 1�� �б�
		bCheck = _stscanf(m_CurrLine, L"%s", token);		// ���� �ٿ��� ù��° '�ܾ�' �б�
		// �ּ�
		if (EQUAL(token, L"*COMMENT")) continue;	//*COMMENT

		// Scene ����
		else if (EQUAL(token, L"*SCENE"))
		{
			_LoadSceneInfo();
		}

		// ���� ������ - Node ���� �б�
		else if (EQUAL(token, L"*MATERIAL_LIST"))	//*MATERIAL_LIST
		{
			///OutputDebugStringW(L"MATERIAL_LIST");

			_LoadMaterialList();
			continue;
		}


		// ���ϵ����� - Node ���� �б�
		else if (EQUAL(token, L"*GEOMOBJECT"))		//*GEOMOBJECT
		{
			// �Լ� ���ο��� Geometry - Node ��ü ����. �ܺο� ���.
			_LoadGeomObject();
			continue;
		}

		//�� ������ �б� �ڵ带 �߰��� ����

	}

	// 5. ��ü ASE �۾� �б� �Ϸ� 
	fclose(m_fp);

	// ���� ���� �����
	_CreateHierarchy();

	// �븻 ������ ���� ������ ���� ����ֱ�
	_SetNewVertexList();

	// �ؽ�ó �ε�
	_LoadTexture(filename);

	// TM �����

	for (unsigned int i = 0; i < m_pModel->m_RootList.size(); i++)
	{
		m_pModel->CreateTM(m_pModel->m_RootList[i]);

		SetQuaternionRotation(m_pModel->m_RootList[i]);
	}

	///m_pModel->CreateTM(m_pModel->m_Root);

	///SetQuaternionRotation(m_pModel->m_Root);

	//return m_LineCount;		// �� �о���� ���μ� ���� (������)
	return CHK_OK;
}

int ParserAse::_LoadSceneInfo()
{
	TCHAR token[256] = L"";
	UINT materialNum = 0;

	// �� ���� Ȯ��
	if (m_pModel == NULL) return CHK_ERROR;

	while (1)
	{
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		// ��� �̸�
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

		// �ݱ�
		if (EQUAL(token, L"}")) break;

	}
	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// ���� ����
// MATERIAL_COUNT...
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadMaterialList()
{
	TCHAR token[256] = L"";
	UINT materialNum = 0;

	// �� ���� Ȯ��
	if (m_pModel == NULL) return CHK_ERROR;

	while (1)
	{
		_ReadLine(m_CurrLine);					// 1�� �б� 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);		// ���� �ٿ��� ù��° '�ܾ�' �б�

		// ��� �̸�
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

		// ��� ���
		else if (EQUAL(token, L"*MATERIAL"))			//*MATERIAL
		{
			_LoadMaterial(materialNum);
			materialNum++;
			continue;
		}

		// �ݱ�
		if (EQUAL(token, L"}")) break;			// '}' - *MATERIAL_LIST �������� ��
	}


	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// ������Ʈ�� ���� �б�
//
// [�б� ����]
// *GEOMOBJECT {	....	}
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadGeomObject()
{
	///OutputDebugStringW(L"_LoadGeomObject");

	TCHAR token[256] = L"";

	// �� ���� Ȯ��
	if (m_pModel == NULL) return CHK_ERROR;

	NodeAse* nowNode = new NodeAse();

	while (1)
	{
		_ReadLine(m_CurrLine);					// 1�� �б� 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);		// ���� �ٿ��� ù��° '�ܾ�' �б�

		// ��� �̸�
		if (EQUAL(token, L"*NODE_NAME"))			//*NODE_NAME
		{
			bCheck = _stscanf(m_CurrLine, L"\t *NODE_NAME \"%[^\"]\"", nowNode->m_NodeName);
			continue;
		}

		// �θ� �̸�
		else if (EQUAL(token, L"*NODE_PARENT"))			//*NODE_PARENT
		{
			bCheck = _stscanf(m_CurrLine, L"\t *NODE_PARENT \"%[^\"]\"", nowNode->m_NodeParentName);
			continue;
		}

		// ��� ���
		else if (EQUAL(token, L"*NODE_TM"))			//*NODE_TM
		{
			_LoadTM(nowNode);
			continue;
		}

		// �޽� ����
		else if (EQUAL(token, L"*MESH"))				//*MESH
		{
			///OutputDebugStringW(L"_LoadMesh");
			_LoadMesh(nowNode);
			continue;
		}

		// ��Ÿ Ű���� ����..
		//*PROP_MOTIONBLUR 0
		//*PROP_CASTSHADOW 1
		//*PROP_RECVSHADOW 1

		else if (EQUAL(token, L"*TM_ANIMATION"))
		{
			_LoadAnimation(nowNode);
			continue;
		}

		// ���� ��� ������ ����ϴ���...?
		else if (EQUAL(token, L"*MATERIAL_REF"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MATERIAL_REF %d", &nowNode->m_MaterialRef);
			continue;
		}

		// ��Ų
		else if (EQUAL(token, L"*SKIN"))
		{
			_LoadSkin(nowNode);
		}

		// �ݱ�
		if (EQUAL(token, L"}")) break;			// '}' - *GEOMOBJECT �������� ��
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
// TM ��� ���� �б�
// 
// [ �б� ����]
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

		// �ݱ�
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
// �޽� ���� �б�
//
// GeomObject {   
//       Mesh  { ...   <--���⼭ ���� �Ľ�
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadMesh(NodeAse* node)
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		//TIMEVALUE �̻��, ����.  			//*TIMEVALUE :  


		// ���� ����
		if (EQUAL(token, L"*MESH_NUMVERTEX"))				//'*MESH_NUMVERTEX 3'
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMVERTEX %d", &node->m_VertexCnt);
			continue;
		}

		// ���̽� ����
		else if (EQUAL(token, L"*MESH_NUMFACES"))				//'*MESH_NUMFACES 1'
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMFACES %d", &node->m_FaceCnt);
			continue;
		}

		// ���� ����Ʈ �ε�	
		else if (EQUAL(token, L"*MESH_VERTEX_LIST"))				//'*MESH_VERTEX_LIST {'
		{
			node->m_NormalCnt = node->m_FaceCnt * 3;
			_LoadVertexList(node);
			continue;
		}

		// ���̽��� ���� ����Ʈ (Index Buffer) ���� �ε�
		else if (EQUAL(token, L"*MESH_FACE_LIST"))				//'*MESH_FACE_LIST {'
		{
			_LoadFaceList(node);
			continue;
		}

		// UV ���� �ε�
		else if (EQUAL(token, L"*MESH_NUMTVERTEX"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMTVERTEX %d", &node->m_TVertexCnt);
			continue;
		}

		// UV ���� �ε�
		else if (EQUAL(token, L"*MESH_TVERTLIST"))
		{
			_LoadTVertList(node);
			continue;
		}

		// UV ���� �ε�
		else if (EQUAL(token, L"*MESH_NUMTVFACES"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMTVFACES %d", &node->m_TFaceCnt);
			continue;
		}

		// UV ���� �ε�
		else if (EQUAL(token, L"*MESH_TFACELIST"))
		{
			_LoadTFaceList(node);
			continue;
		}

		// �븻 ���� �ε�
		else if (EQUAL(token, L"*MESH_NORMALS"))				//'*MESH_NORMALS {'
		{
			///OutputDebugStringW(L"MESH_NORMALS");
			_LoadNormalList(node);
			continue;
		}

		// ���� �� ����Ʈ
		else if (EQUAL(token, L"*MESH_NUMCVERTEX"))				//'*MESH_NUMCVERTEX 6'
		{
			_LoadCVertexList(node);
			continue;
		}

		// ���̽��� ������ �ε��� ����Ʈ �ε�
		else if (EQUAL(token, L"*MESH_NUMCVFACES"))				//'*MESH_NUMCVFACES 1'
		{
			_LoadCFaceList(node);
			continue;
		}

		// �ݱ�
		if (EQUAL(token, L"}")) break;			//'}'  
	}

	return CHK_OK;

}


//////////////////////////////////////////////////////////////////////////
//
// ���� ���� �б�
//
// [�б� ����]
// 	*MATERIAL 0 {
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadMaterial(UINT num)
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1�� �б� 
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

		// ��Ʈ��
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

		// �ݱ�
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
		_ReadLine(m_CurrLine);				// 1�� �б� 
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

		// �ݱ�
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
		_ReadLine(m_CurrLine);				// 1�� �б� 
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

		// �ݱ�
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// �ؽ��� �ε�
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

		/// �ؽ�ó ���� �� ó���� �̷��� ������ϳ�..? 
		if (EQUAL(m_pModel->m_pMaterialList[i].m_TextureName, L""))
			continue;

		_tcscat(texFilename, m_pModel->m_pMaterialList[i].m_TextureName);
		D3DEngine::GetIns()->LoadTexture(texFilename, &m_pModel->m_pMaterialList[i].m_Texture);
	}

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// ���� ����Ʈ ���
//
//  [�б� ����]
// *MESH_VERTEX_LIST {
//			*MESH_VERTEX    0	10.0000	0.0000	0.0000  <-- ���⼭ ���� �д´�.
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadVertexList(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT cnt = 0;
	UINT num = 0;

	VECTOR3 pos;

	//���� ����Ʈ ���� �� �ʱ�ȭ. 
	node->m_pVertList = new VERTEX_ASE[node->m_VertexCnt];
	assert(node->m_pVertList != NULL);
	ZeroMemory(node->m_pVertList, sizeof(VERTEX_ASE) * node->m_VertexCnt);

	// ���� ������ŭ �о�� �Ѵ�.
	while (1)
	{
		// 1�� �б� ->  '*MESH_VERTEX  0	10.0000	0.0000	0.0000'		 
		_ReadLine(m_CurrLine);
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MESH_VERTEX"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_VERTEX %d %f %f %f",
				&num,
				&pos.x,
				&pos.z,	// <- ��������! Z�� Y �ݴ��  
				&pos.y	// <- ��������! 
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

		// �ݱ�
		if (EQUAL(token, L"}")) break;			//'}'  
	}

	// ���� ���� : ���� ������ŭ ������ ���ƾ� �Ѵ�.
	if (cnt != node->m_VertexCnt)
	{
		// error
		return CHK_ERROR;
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// ���̽� ����Ʈ �б� : '�ε���' ���۸� ���� �ʿ��ϴ�.
//
// [�б� ����]
//	*MESH_FACE  379:	A:  410		B:  408		C:  409		AB:    1  BC:    1 CA:    0	  *MESH_SMOOTHING 1 	*MESH_MTLID 3
// 
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadFaceList(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT num = 0;
	UINT cnt = 0;

	// ���̽� ����Ʈ ���� �� �ʱ�ȭ
	node->m_pFaceList = new FACE_ASE[node->m_FaceCnt];
	assert(node->m_pFaceList != NULL);
	ZeroMemory(node->m_pFaceList, sizeof(FACE_ASE) * node->m_FaceCnt);

	// �ﰢ�� ������ŭ �о�� �Ѵ�.
	while (1)
	{
		_ReadLine(m_CurrLine);				// 1�� �б� 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		// *MESH_FACE  379:  A: 410  B: 408  C: 409  AB: 1   BC: 1  CA: 0	*MESH_SMOOTHING 1 	*MESH_MTLID 3
		if (EQUAL(token, L"*MESH_FACE"))
		{
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_FACE %d:  A: %d  B: %d  C: %d",
				//" AB: %d  BC: %d  CA: %d "			 --> ����..
				//" *MESH_SMOOTHING %d  *MESH_MTLID %d", --> ����.. 
				&num,			// '*MESH_FACE 379: ���̽� ��ȣ. IB �� �װͰ� ����. ����'
				&node->m_pFaceList[cnt].A,	// 'A: XX' 
				&node->m_pFaceList[cnt].C,	// 'B: XX' <- ���� ���� 
				&node->m_pFaceList[cnt].B	// 'C: XX' <- ���� ����
			);

			cnt++;
			continue;
		}

		// �ݱ�
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	// ���� ���� : �ﰢ�� ������ŭ �о�� �Ѵ�.
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
		_ReadLine(m_CurrLine);				// 1�� �б� 
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

		// �ݱ�
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
		_ReadLine(m_CurrLine);				// 1�� �б� 
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

		// �ݱ�
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// �븻 ����Ʈ �б�
//
// [�б� ����]
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

	// �븻 ����Ʈ ���� �� �ʱ�ȭ
	node->m_pNormalList = new NORMAL_ASE[node->m_NormalCnt];
	assert(node->m_pNormalList != NULL);
	ZeroMemory(node->m_pNormalList, sizeof(NORMAL_ASE) * node->m_NormalCnt);

	// �븻 ������ŭ �о�� �Ѵ�.
	while (1)
	{
		_ReadLine(m_CurrLine);				// 1�� �б� 
		BOOL bCheck = _stscanf(m_CurrLine, L"%s", token);

		// *MESH_FACENORMAL 0	0.0000	0.0000	-1.0000
		if (EQUAL(token, L"*MESH_FACENORMAL"))
		{
			/// ����� ���� �Ľ̸� �� �� ������ �ʴ´�.
			///*
			bCheck = _stscanf(m_CurrLine, L"\t *MESH_FACENORMAL %d %f %f %f",
				&num,
				&node->m_pNormalList[num].vfaceNormal.x,
				&node->m_pNormalList[num].vfaceNormal.z,	// <- ���� ���� 
				&node->m_pNormalList[num].vfaceNormal.y		// <- ���� ����
			);
			//*/

			facecnt++;
			continue;
		}
		// *MESH_VERTEXNORMAL 0	0.0000	0.0000	-1.0000
		else if (EQUAL(token, L"*MESH_VERTEXNORMAL"))
		{
			/// ������ Face List ���� Face �� ���� �����ϰ� �븻�� ���������� �־��ָ� �Ǳ� ������ 
			/// ���� index�� ������ ���� �ʿ䰡 ����.


			/// �븻 ��ġ �ٲ� ���� �����ʹ� ���Ⱑ �ƴ϶� Ǯ ���ؽ� ������ ���̱� ������
			/// �ű⼭ �������־�� �Ѵ�.
			///
			/// �븻 ���� �õ��غ� ��
			/*
			_stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f", &num, &nrm.x, &nrm.z, &nrm.y);

			///nrm = nrm * mTM;
			XMVECTOR vec = XMVector3TransformNormal(XMLoadFloat3(&nrm), XMLoadFloat4x4(&mTM));
			XMStoreFloat3(&nrm, XMVector3Normalize(vec));

			node->m_pNormalList[num].vtxNormal[vtxcnt] = nrm;
			//*/


			/// �븻 ���� ���� ���غ���
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

			/// �븻 ���� ���� �ڵ�
			/// ���� �� �븻B�� �븻C�� ��ġ ����
			///*
			bCheck = _stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f",
				&numExtra,
				&node->m_pNormalList[num].vtxNormal[0].x,
				&node->m_pNormalList[num].vtxNormal[0].z,		// <- ���� ���� 
				&node->m_pNormalList[num].vtxNormal[0].y		// <- ���� ����
			);
			//*/

			_ReadLine(m_CurrLine);				// 1�� �б� 
			bCheck = _stscanf(m_CurrLine, L"%s", token);

			bCheck = _stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f",
				&numExtra,
				&node->m_pNormalList[num].vtxNormal[2].x,
				&node->m_pNormalList[num].vtxNormal[2].z,		// <- ���� ���� 
				&node->m_pNormalList[num].vtxNormal[2].y		// <- ���� ����
			);

			_ReadLine(m_CurrLine);				// 1�� �б� 
			bCheck = _stscanf(m_CurrLine, L"%s", token);

			bCheck = _stscanf(m_CurrLine, L"\t\t *MESH_VERTEXNORMAL %d %f %f %f",
				&numExtra,
				&node->m_pNormalList[num].vtxNormal[1].x,
				&node->m_pNormalList[num].vtxNormal[1].z,		// <- ���� ���� 
				&node->m_pNormalList[num].vtxNormal[1].y		// <- ���� ����
			);

			//vtxcnt++;

			//if (vtxcnt >= 3)
			//	vtxcnt = 0;

			continue;
		}

		// �ݱ�
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	// ���� ���� : �ﰢ�� ������ŭ �о�� �Ѵ�.
	if (facecnt != node->m_NormalCnt)
	{
		// error
		return CHK_ERROR;
	}

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// ���� �÷� - Diffuse color  - �б� 
//
// [�б� ����]
// *MESH_NUMCVERTEX 6
// *MESH_CVERTLIST {
//		*MESH_VERTCOL 0	1.0000	1.0000	1.0000
//      
//////////////////////////////////////////////////////////////////////////
int	ParserAse::_LoadCVertexList(NodeAse* node)
{
	TCHAR token[256] = L"";
	int cnt;

	// ���� ���� ���� �б�
	BOOL bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMCVERTEX %d", &cnt);		//'*MESH_NUMCVERTEX 6'
	if (cnt <= 0) return CHK_ERROR;

	// ���� �����͸� ������ ���� ����
	node->m_pCVertList = new COLOR[cnt];
	if (node->m_pCVertList == NULL)
	{
		// error
		return CHK_ERROR;
	}

	int num;
	COLOR col(1, 1, 1, 1);

	//-------------------------------------------------------------------
	// ���� ���� ����Ʈ �б�
	//-------------------------------------------------------------------
	_ReadLine(m_CurrLine);				// 1�� �б� -> '*MESH_CVERTLIST {'

	for (int i = 0; i < cnt; i++)
	{
		_ReadLine(m_CurrLine);			// 1�� �б� -> '*MESH_VERTCOL 0	1.0000	1.0000	1.0000'
		bCheck = _stscanf(m_CurrLine, L"\t *MESH_VERTCOL %d %f %f %f", &num, &col.x, &col.y, &col.z);
		//_stscanf(m_CurrLine, L"\t *MESH_VERTCOL %d %f %f %f", &num, &col.r, &col.g, &col.b);

		node->m_pCVertList[num] = col;
	}

	// �ݱ�
	_ReadLine(m_CurrLine);			// 1�� �б� -> '}'

	return CHK_OK;
}




///////////////////////////////////////////////////////////////////////////
//
// 'face �� ���� ���� �ε��� ����Ʈ' ���� �ε�
//
// [�б� ����]
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

	// face �� ������ �ε��� ���� ��� -> '*MESH_NUMCVFACES 1'
	BOOL bCheck = _stscanf(m_CurrLine, L"\t *MESH_NUMCVFACES %d", &cnt);
	if (cnt <= 0) return CHK_ERROR;

	// ������ - �ε��� ���� ���� 
	node->m_pCFaceList = new FACE_ASE[cnt];
	if (node->m_pCFaceList == NULL)
	{
		// error
		return CHK_ERROR;
	}

	//-------------------------------------------------------------------
	// face �� ������ �ε��� �б�
	//-------------------------------------------------------------------
	_ReadLine(m_CurrLine);			// 1���б� -> '*MESH_CFACELIST {'

	int fn = 0;
	for (int i = 0; i < cnt; i++)
	{
		_ReadLine(m_CurrLine);		// 1�� �б� -> '*MESH_CFACE 0 4 5 3' 

		bCheck = _stscanf(m_CurrLine, L"\t *MESH_CFACE %d %d %d %d",
			&fn,								// face ��ȣ. 
			&node->m_pCFaceList[i].A,		// A C B ��(CCW) ���� ó��. ��������.
			&node->m_pCFaceList[i].C,
			&node->m_pCFaceList[i].B
		);
	}

	// �ݱ�
	_ReadLine(m_CurrLine);		//'}'

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// �ִϸ��̼� ���� �ε�
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadAnimation(NodeAse* node)
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1�� �б� 
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

		// �ݱ�
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// �ִϸ��̼� ���� �ε� - Trans
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadAnimPosTrack(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT  num = 0;

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1�� �б� 
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


			/// ���� ������ �־������
			//node->m_AniKey[num].tickTime = tickTime;
			//node->m_AniKey[num].Pos = pos;
			num++;

			continue;
		}

		// �ݱ�
		if (EQUAL(token, L"}")) break;			//'}' 
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// �ִϸ��̼� ���� �ε� - Rot
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_LoadAnimRotTrack(NodeAse* node)
{
	TCHAR token[256] = L"";
	UINT  num = 0;
	KEY_ROT nowKey;

	while (1)
	{
		_ReadLine(m_CurrLine);				// 1�� �б� 
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

			/// ���� ������ �־������
			//node->m_AniKey[num].tickTime = tickTime;
			//node->m_AniKey[num].RotAxis = rot;
			//node->m_AniKey[num].RotAngle = angle;
			num++;

			continue;
		}

		// �ݱ�
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

		//1. ���麸��(Sphereical Interpolation) �� ���� ���ʹϿ����� ��ȯ.
		VECTOR axis = XMLoadFloat3(&node->m_AniKeyRot[i].RotAxis);
		QUATERNIONA q;
		if (node->m_AniKeyRot[i].RotAngle != 0)
		{
			q = XMQuaternionRotationAxis(axis, node->m_AniKeyRot[i].RotAngle);
		}

		//2.���� ���ʹϿ����� ȯ�� & ����.
		if (i == 0)
		{
			node->m_AniKeyRot[i].RotQ = q;	// 1��° Ű�� "���밪" ���� �������� : �ٷ� ����.
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

		//����, �ִϸ��̼� ó����, 
		//3.���麸��, �߰����ʹϿ� ���. 
		//4.������ ���ʹϿ¿��� ������ ȸ����� ���. 
		//  -> ������... 
	}

	/// �̰� �� ���� for�� �ȿ� �־�����..
	for (unsigned int j = 0; j < node->m_Childs.size(); j++)
	{
		SetQuaternionRotation(node->m_Childs[j]);
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// �ִϸ��̼� ���� �ε� - Scale
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

			/// ���� ������ �־������
			//node->m_AniKey[num].Scale = scale;
			//node->m_AniKey[num].ScaleAxis = scaleAxis;
			//node->m_AniKey[num].ScaleAngle = angle;
			num++;

			continue;
		}

		// �ݱ�
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

		// �ݱ�
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
	// Bone ���� ó��
	for (unsigned int i = 0; i < m_pModel->m_pBoneList.size(); i++)
	{
		TCHAR* nowParentName = m_pModel->m_pBoneList[i]->m_NodeParentName;
		NodeAse* findParent = nullptr;
		// �θ� ����. ���� �ֻ���
		if (EQUAL(nowParentName, L"NA"))
		{
			m_pModel->m_RootList.push_back(m_pModel->m_pBoneList[i]);
			///m_pModel->m_Root = m_pModel->m_pBoneList[i];
			m_pModel->m_pBoneList[i]->m_Parent = nullptr;
		}
		else
		{
			// �θ� �̸����� ��带 ã�´�.
			findParent = m_pModel->FindNodeByName(nowParentName);

			// �� ��带 ���� �θ� ����Ѵ�.
			m_pModel->m_pBoneList[i]->m_Parent = findParent;

			// �θ� ��忡 ���� ����Ѵ�.
			findParent->m_Childs.push_back(m_pModel->m_pBoneList[i]);
		}
	}

	// Node ó��
	for (unsigned int i = 0; i < m_pModel->m_pNodeList.size(); i++)
	{
		TCHAR* nowParentName = m_pModel->m_pNodeList[i]->m_NodeParentName;
		NodeAse* findParent = nullptr;
		// �θ� ����. ���� �ֻ���
		if (EQUAL(nowParentName, L"NA"))
		{
			m_pModel->m_RootList.push_back(m_pModel->m_pNodeList[i]);
			///m_pModel->m_Root = m_pModel->m_pNodeList[i];
			m_pModel->m_pNodeList[i]->m_Parent = nullptr;
		}
		else
		{
			// �θ� �̸����� ��带 ã�´�.
			findParent = m_pModel->FindNodeByName(nowParentName);

			// �� ��带 ���� �θ� ����Ѵ�.
			m_pModel->m_pNodeList[i]->m_Parent = findParent;

			// �θ� ��忡 ���� ����Ѵ�.
			findParent->m_Childs.push_back(m_pModel->m_pNodeList[i]);
		}
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// �븻 ������ �������� �ٽ� ������ �Ѵ�.
//
//////////////////////////////////////////////////////////////////////////
int ParserAse::_SetNewVertexList()
{
	for (unsigned int i = 0; i < m_pModel->m_pNodeList.size(); i++)
	{
		m_pModel->m_pNodeList[i]->SetNewVertexList();

		// ����׿� ������
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
