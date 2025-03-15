#pragma once

#include "DXEngineDefine.h"
#include "NodeAse.h"
#include <vector>

#define ASEMODEL_VER_   3.1		//Ver.3.00

// 파싱내역
//Vertex Position
//Vertex Index (Face List)
//Vertex Color
//Vertex Color Index (C-Face List)

// 파서 선행 선언
class ParserAse;
class NodeAse;



//////////////////////////////////////////////////////////////////////////
//
// 재질 구조체
//
//////////////////////////////////////////////////////////////////////////
struct MATERIAL_ASE
{
	TCHAR MaterialName[256];
	TCHAR MaterialClass[256];
	COLOR Diffuse;
	COLOR Ambient;
	COLOR SpecularCol;
	COLOR Specular;
	FLOAT Power;

	TCHAR						m_TextureName[256];
	ID3D11ShaderResourceView* m_Texture;

	ID3D11SamplerState* m_pSampler;
};



//////////////////////////////////////////////////////////////////////////
//
// class ModelAse : ASE Model 파일 클래스
//                   
//////////////////////////////////////////////////////////////////////////
class ModelAse
{
	friend class ParserAse;

public:
	// 메쉬 데이터
	TCHAR	m_FileName[256];	// 파일명 

	// 한 모델의 전체 정점, 인덱스 개수
	UINT	m_OriginalVtxCnt;	// 원본 전체 정점 개수
	UINT	m_FullVtxCnt;		// 풀 버텍스 정점 개수
	UINT	m_WeldVtxCnt;		// 최적화 전체 정점 개수
	UINT	m_TotalFaceCnt;		// 페이스 개수

	///std::vector<NodeAse*> m_pNodeList;
	///NodeAse* m_Root;
	std::vector<NodeAse*> m_RootList;
	std::vector<NodeAse*> m_pNodeList;
	std::vector<NodeAse*> m_pBoneList;

	// 기하 데이터 (임시) 버퍼
	MATERIAL_ASE* m_pMaterialList;
	UINT			m_MaterialCnt;

	// 디바이스 인터페이스
	ID3D11Device* m_pDev;

	// 애니메이션
	int		m_SceneFirstFrame;
	int		m_SceneLastFrame;
	int		m_SceneFrameSpeed;
	int		m_SceneTickPerFrame;

	float	m_AniTime;				// 전체 애니메이션 플레이 시간

	//float	m_CurrTime;				// 현재 구간 진행률
	//DWORD	m_CurrKey;				// 현재 애니 시작 키 : tickTime 으로 환산, 처리하는 것을 권장
	//DWORD	m_CurrTickTime;			// 현재(까지 플레이된) 틱타임
	//DWORD	m_TotTickTime;			// 총 틱타임

	bool m_isEnd;

public:
	float	m_AniSpeed;


protected:
	// 정점 버퍼 재구성
	int _CreateVFBuffers();

	// 렌더링 버퍼 생성
	HRESULT _CreateHWBuffers();


public:
	//렌더링 버퍼 구성 및 리소스 로딩
	int Generate();


public:
	// 생성자 및 소멸자
	ModelAse(ID3D11Device* pDev);
	virtual ~ModelAse();

	const TCHAR* GetFileName() const { return m_FileName; }

public:
	// 일반 메소드
	///virtual int Draw(float dTime = 0.0f);			// 기본 그리기 코드
	int NodeDraw(float dTime/*=0.0f*/);
	int BoneDraw(float dTime/*=0.0f*/);
	int CreateTM(NodeAse* node);
	void AnimateAll(float dTime);
	void AnimateResetAll();
	void Animate_Pos(float dTime, NodeAse* node);
	void Animate_Rot(float dTime, NodeAse* node);
	int GetCurrTick(float dTime);
	void Animate_Scale(float dTime, NodeAse* node);
	void Animate_Reset(NodeAse* node);
	float FrameTickTime_Pos(DWORD currKey, DWORD currTickTime, NodeAse* node);
	float FrameTickTime_Rot(DWORD currKey, DWORD currTickTime, NodeAse* node);
	float FrameTickTime_Scale(DWORD currKey, DWORD currTickTime, NodeAse* node);
	NodeAse* FindNodeByName(TCHAR* name);
	int ModelDebugData();
	int SetMaterial_New(NodeAse* node);

	float _Lerp(float& v0, float& v1, float& a);
	VECTOR3 Vec3Lerp(VECTOR3& v0, VECTOR3& v1, float& a);
};



//////////////////////////////////////////////////////////////////////////
//
// Model File 정보 구조체 (API 확장 대응용) 
//
//////////////////////////////////////////////////////////////////////////
struct MODELINFO
{
	//추후 필요 데이터 추가 예정.  
	//...
};


// 모델 로드, 생성 함수 (ver.ASE)
//int  ModelCreateFromASE(ID3D11Device* pDev, TCHAR* filename, MODELINFO* pInfo, ModelAse** ppModel);
int ModelCreateFromASE(ID3D11Device* pDev, /* [in] Device Handle */ const TCHAR* filename, /* [in] Ase 파일명 */ MODELINFO* pInfo, /* [in] B3Model 정보구조체. 사용안함. 기본값 NULL. */ ModelAse** ppModel /* [out] 로딩후 리턴할 Model 개체의 이중포인터. 실패시 NULL 리턴. */);

// 예나 모델 제거
void ModelRelease(ModelAse*& ppModel);

// 구형 호환 재정의
typedef ModelAse				YnAseModel;
typedef ParserAse				YnAseParser;
