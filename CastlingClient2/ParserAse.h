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
// class ParserAse : ASE Parsing 클래스
//                 : 각 구문을 읽고, 정보를 해석한다.				  
// 
//////////////////////////////////////////////////////////////////////////

class ParserAse
{
	//protected:
	//friend class AseModel;

protected:
	//-------------------------------------------------------------------
	// 파싱 관련 데이터 및 메소드
	//-------------------------------------------------------------------
	int	 m_LineCount;					// 총 파싱 라인 카운터 (+디버깅용)
	TCHAR m_CurrLine[256];				// 읽어들인 현재 라인 파싱용 임시버퍼.
	//TCHAR m_NodeName[256];			// 노드(메쉬) 이름

	//-------------------------------------------------------------------
	// 파싱된 정보의  저장 목표 개체 포인터
	//-------------------------------------------------------------------
	FILE* m_fp;						// [외부참조] ASE 파일 포인터
	AseModel* m_pModel;				// [외부참조] 파서가 접근할 최상위 ASE 개체 클래스 포인터

protected:
	// 파싱 함수: 한라인씩 읽는다.
	int _ReadLine(TCHAR* buff);

	// scene 정보 : 애니메이션 구현용
	//int _LoadScene(FILE* fp);		

	int _LoadMaterialList();
	// 지오메트리 파싱
	int _LoadGeomObject();

	// Geometry : 기하데이터(노드) 하위 파싱 함수			 
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
	// 파싱(Parsing) 최상위 레벨 함수
	//-------------------------------------------------------------------
	int Load(ID3D11Device* pDev, const TCHAR* filename, ModelAse* pModel);
	int _LoadSceneInfo();
	//int Load(FILE* fp);


	STATIC
		//-------------------------------------------------------------------
		//'directory' 문자열 처리 관련
		//-------------------------------------------------------------------
		static void GetFileName(const TCHAR* FullPath, TCHAR* FileName);		// 파일명 얻기 (ver.STL)
		//void GetFileName( TCHAR* FullPath, TCHAR* FileName);					// 파일명 얻기 (ver.C) 
	static void GetPath(const TCHAR* FullPathName, TCHAR* Path);			// path 얻기
	void GetPath1(const TCHAR* FullPathName, TCHAR* Path);


public:
	static int m_Count;
	static int m_Depth;

};

