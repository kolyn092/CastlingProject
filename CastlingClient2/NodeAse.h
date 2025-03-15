#pragma once
#include "ModelAse.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
//
// 정점 구조체 및 포멧
//
//////////////////////////////////////////////////////////////////////////
struct NEW_VERTEX
{
	VECTOR3 vPos;
	VECTOR3 vNormal;
	///COLOR dwColor;
	VECTOR2 vUV;
};

struct VERTEX_ASE
{
	VECTOR3 vPos;
	COLOR   dwColor;
};
//#define FVF_ASE (D3DFVF_XYZ | D3DFVF_DIFFUSE)


struct TVERT_ASE
{
	VECTOR2 uvPos;
};

//////////////////////////////////////////////////////////////////////////
//
// Face 구조체  
//
//////////////////////////////////////////////////////////////////////////
struct FACE_ASE
{
	DWORD A, B, C;		//32Bit..
};


//////////////////////////////////////////////////////////////////////////
//
// 노말 구조체
//
//////////////////////////////////////////////////////////////////////////
struct NORMAL_ASE
{
	VECTOR3 vfaceNormal;
	VECTOR3 vtxNormal[3];
};

struct NORMAL_LINE_ASE
{
	VECTOR3 pos;
};

struct WEIGHT_ASE
{
	TCHAR boneName[256];
	float weight;
};

struct SKIN_ASE
{
	int vtxID;
	int weightCnt;
	WEIGHT_ASE* weightList;
};


//////////////////////////////////////////////////////////////////////////
//
// 애니메이션 키 관련 구조체
//
//////////////////////////////////////////////////////////////////////////
#define KEY_MAX_  21			// 키 21개 0 ~ 16000
typedef Vector4	QUATERNIONA;

struct KEY
{
	DWORD			tickTime;		// 3DS MAX Animation TickTime (48000tick/sec)
	VECTOR3			Pos;			// 이동  
	VECTOR3			Scale;			// 스케일 
	VECTOR3			ScaleAxis;		// 스케일축 
	float			ScaleAngle;		// 스케일각
	VECTOR3			RotAxis;		// 회전축
	float			RotAngle;		// 회전각
	QUATERNIONA		RotQ;			// 회전-쿼터니온(4성분) 

};

struct KEY_POS
{
	DWORD			tickTime;		// 3DS MAX Animation TickTime (48000tick/sec)
	VECTOR3			Pos;			// 이동  
};

struct KEY_ROT
{
	DWORD			tickTime;		// 3DS MAX Animation TickTime (48000tick/sec)
	VECTOR3			RotAxis;		// 회전축
	float			RotAngle;		// 회전각
	QUATERNIONA		RotQ;			// 회전-쿼터니온(4성분) 

	bool			ZeroFrame;
};

struct KEY_SCALE
{
	DWORD			tickTime;		// 3DS MAX Animation TickTime (48000tick/sec)
	VECTOR3			Scale;			// 스케일 
	VECTOR3			ScaleAxis;		// 스케일축 
	float			ScaleAngle;		// 스케일각
};


//////////////////////////////////////////////////////////////////////////
//
// class NodeAse : Model Node 클래스
//                   
//////////////////////////////////////////////////////////////////////////
class NodeAse// : public AlignedAllocationPolicy<16>
{
	friend class ParserAse;

public:
	TCHAR	m_NodeName[256];	// 노드(메쉬) 이름
	TCHAR	m_NodeParentName[256];	// 노드(부모) 이름
	UINT	m_VertexCnt;		// 정점 개수
	UINT	m_FaceCnt;			// 페이스 개수
	UINT	m_NormalCnt;		// 노말 개수
	UINT	m_IndexCnt;			// 인덱스로 그려질 총 정점개수. 보통 Face * 3 개
	UINT	m_TVertexCnt;
	UINT	m_TFaceCnt;

public:
	// 변환 행렬 정보 : "NodeTM"
	MATRIX	m_NodeTM;
	MATRIX	m_LocalTM;

	MATRIX	m_mTrans;
	MATRIX  m_mScale;
	MATRIX	m_mRot;

	XMMATRIX m_LocalTrans;
	XMMATRIX m_LocalRotate;
	XMMATRIX m_LocalScale;

	XMVECTOR m_LocalVecTrans;
	XMVECTOR m_LocalVecRotate;
	XMVECTOR m_LocalVecScale;

	BOOL	m_isTrans;
	BOOL	m_isRot;
	BOOL	m_isScale;

public:
	VECTOR3 m_vPos;
	VECTOR3	m_vScale;
	VECTOR3 m_vScaleAxis;
	FLOAT	m_vScaleAxisAng;
	VECTOR3 m_vRot;
	VECTOR3 m_vRotAxis;
	FLOAT	m_vRotAngle;

	// 기하 데이터 (임시) 버퍼
	NEW_VERTEX* m_pNewVertexList;	// 노말 개수에 맞춰서 재구성한 정점 리스트 : VB 용
	FACE_ASE* m_pNewFaceList;		// 노말 개수에 맞춰서 재구성한 FACE 리스트 : IB 용
	VERTEX_ASE* m_pVertList;		// 정점 리스트 : VB 용 
	FACE_ASE* m_pFaceList;			// FACE 리스트: IB 용 
	NORMAL_ASE* m_pNormalList;		// 노말 리스트
	COLOR* m_pCVertList;			// 정점 색상 리스트
	FACE_ASE* m_pCFaceList;			// 정점 색상-인덱스 리스트
	TVERT_ASE* m_pTVertList;
	FACE_ASE* m_pTFaceList;
	SKIN_ASE* m_pSkinList;

	NORMAL_LINE_ASE* m_pNormalLineList;

	// 현재 노드가 어떤 재질을 사용하는지 알고 있어야 한다.
	int m_MaterialRef;

	// 렌더링 (H/W) 버퍼   
	LPVERTEXBUFFER	m_pVB;
	LPVERTEXBUFFER	m_pNewVB;			// 노말 개수에 맞춰서 재구성할 정점 버퍼
	LPVERTEXBUFFER	m_pWeldVB;			// Weld가 적용된 정점 버퍼
	LPVERTEXBUFFER	m_pNormalLineVB;	// 디버깅용 노말 정점 버퍼
	LPINDEXBUFFER	m_pIB;
	LPINDEXBUFFER	m_pNewIB;			// 노말 개수에 맞춰서 재구성할 인덱스 버퍼
	LPINDEXBUFFER	m_pWeldIB;			// Weld가 적용된 인덱스 버퍼

public:
	NodeAse* m_Parent;
	std::vector<NodeAse*> m_Childs;

public:
	// 애니메이션
	///KEY		m_AniKey[KEY_MAX_];
	std::vector<KEY_POS> m_AniKeyPos;
	std::vector<KEY_ROT> m_AniKeyRot;
	std::vector<KEY_SCALE> m_AniKeyScale;

	XMMATRIX m_AnimTrans;
	XMMATRIX m_AnimRotate;
	XMMATRIX m_AnimScale;

	float	m_CurrTime_Pos;				// 현재 구간 진행률
	DWORD	m_CurrKey_Pos;				// 현재 애니 시작 키 : tickTime 으로 환산, 처리하는 것을 권장
	DWORD	m_CurrTickTime_Pos;			// 현재(까지 플레이된) 틱타임
	DWORD	m_TotTickTime_Pos;			// 총 틱타임

	float	m_CurrTime_Rot;				// 현재 구간 진행률
	DWORD	m_CurrKey_Rot;				// 현재 애니 시작 키 : tickTime 으로 환산, 처리하는 것을 권장
	DWORD	m_CurrTickTime_Rot;			// 현재(까지 플레이된) 틱타임
	DWORD	m_TotTickTime_Rot;			// 총 틱타임

	float	m_CurrTime_Scale;			// 현재 구간 진행률
	DWORD	m_CurrKey_Scale;			// 현재 애니 시작 키 : tickTime 으로 환산, 처리하는 것을 권장
	DWORD	m_CurrTickTime_Scale;		// 현재(까지 플레이된) 틱타임
	DWORD	m_TotTickTime_Scale;		// 총 틱타임

public:
	NodeAse();
	virtual ~NodeAse();

public:
	int _CreateVFBuffers();
	HRESULT _CreateHWBuffers();
	int Generate();
	int Draw(float dTime/*=0.0f*/);
	XMMATRIX GetWorldTM();
	int NormalLineDraw();
	int SetNewVertexList();
	int InputNormalDebugData();
	int RepairNormal();
	int GetFullVertexCnt();
	void SetTrans(VECTOR3 Pos);
	void SetRot(QUATERNIONA Quat);
	void SetScale(VECTOR3 Scale);

public:
	// 접근 메소드
	const MATRIX& GetTM() const { return m_NodeTM; }
	const VECTOR3 GetPos() const { return m_vPos; }
	const VECTOR3 GetRot() const { return m_vRot; }
	const LPVERTEXBUFFER& GetVB() const { return m_pVB; }
	const LPINDEXBUFFER& GetIB() const { return m_pIB; }
	const UINT GetVBStrideInByte() const { return sizeof(VERTEX_ASE); }
	const UINT GetFaceCnt() const { return m_FaceCnt; }
	const UINT GetVertexCnt() const { return m_VertexCnt; }
	const UINT GetIndexedCnt() const { return m_IndexCnt; }
	const UINT GetMaterialRef() const { return m_MaterialRef; }
	const TCHAR* GetParentName() const { return m_NodeParentName; }
	const TCHAR* GetNodeName() const { return m_NodeName; }
	const NodeAse* GetParent() const { return m_Parent; }
};

