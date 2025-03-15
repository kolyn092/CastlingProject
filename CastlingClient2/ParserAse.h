#pragma once

#include "vector"
#include "list"
using namespace std;


class ModelAse;
typedef ModelAse  Ase;
typedef ModelAse  AseModel;


#define STATIC

//////////////////////////////////////////////////////////////////////////
//
// class ParserAse : ASE Parsing Ŭ����
//                 : �� ������ �а�, ������ �ؼ��Ѵ�.				  
// 
//////////////////////////////////////////////////////////////////////////

class ParserAse
{
	//protected:
	//friend class AseModel;

protected:
	//-------------------------------------------------------------------
	// �Ľ� ���� ������ �� �޼ҵ�
	//-------------------------------------------------------------------
	int	 m_LineCount;					// �� �Ľ� ���� ī���� (+������)
	TCHAR m_CurrLine[256];				// �о���� ���� ���� �Ľ̿� �ӽù���.
	//TCHAR m_NodeName[256];			// ���(�޽�) �̸�

	//-------------------------------------------------------------------
	// �Ľ̵� ������  ���� ��ǥ ��ü ������
	//-------------------------------------------------------------------
	FILE* m_fp;						// [�ܺ�����] ASE ���� ������
	AseModel* m_pModel;				// [�ܺ�����] �ļ��� ������ �ֻ��� ASE ��ü Ŭ���� ������

protected:
	// �Ľ� �Լ�: �Ѷ��ξ� �д´�.
	int _ReadLine(TCHAR* buff);

	// scene ���� : �ִϸ��̼� ������
	//int _LoadScene(FILE* fp);		

	int _LoadMaterialList();
	// ������Ʈ�� �Ľ�
	int _LoadGeomObject();

	// Geometry : ���ϵ�����(���) ���� �Ľ� �Լ�			 
	int _LoadTM(NodeAse* node);
	int _LoadMesh(NodeAse* node);
	int _LoadMaterial(UINT num);
	int _LoadMapDiffuse(UINT num);
	int _LoadMapReflect();
	//int _LoadTexture(TCHAR* filename);
	int _LoadTexture(const TCHAR* filename);
	int _LoadVertexList(NodeAse* node);
	int _LoadFaceList(NodeAse* node);
	int _LoadTVertList(NodeAse* node);
	int _LoadTFaceList(NodeAse* node);
	int _LoadCVertexList(NodeAse* node);
	int _LoadCFaceList(NodeAse* node);
	int _LoadAnimation(NodeAse* node);
	int _LoadAnimPosTrack(NodeAse* node);
	int _LoadAnimRotTrack(NodeAse* node);
	int SetQuaternionRotation(NodeAse* node);
	int _LoadAnimScaleTrack(NodeAse* node);
	int _LoadSkin(NodeAse* node);
	int _LoadSkinWeight(SKIN_ASE* nowSkin);
	int _CreateHierarchy();
	int _LoadNormalList(NodeAse* node);
	int _SetNewVertexList();

public:
	ParserAse();
	~ParserAse();

public:
	//-------------------------------------------------------------------
	// �Ľ�(Parsing) �ֻ��� ���� �Լ�
	//-------------------------------------------------------------------
	int Load(ID3D11Device* pDev, const TCHAR* filename, ModelAse* pModel);
	int _LoadSceneInfo();
	//int Load(FILE* fp);


	STATIC
		//-------------------------------------------------------------------
		//'directory' ���ڿ� ó�� ����
		//-------------------------------------------------------------------
		static void GetFileName(const TCHAR* FullPath, TCHAR* FileName);		// ���ϸ� ��� (ver.STL)
		//void GetFileName( TCHAR* FullPath, TCHAR* FileName);					// ���ϸ� ��� (ver.C) 
	static void GetPath(const TCHAR* FullPathName, TCHAR* Path);			// path ���
	void GetPath1(const TCHAR* FullPathName, TCHAR* Path);


public:
	static int m_Count;
	static int m_Depth;

};

