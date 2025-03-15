#pragma once
#include "ModelAse.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
//
// ���� ����ü �� ����
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
// Face ����ü  
//
//////////////////////////////////////////////////////////////////////////
struct FACE_ASE
{
	DWORD A, B, C;		//32Bit..
};


//////////////////////////////////////////////////////////////////////////
//
// �븻 ����ü
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
// �ִϸ��̼� Ű ���� ����ü
//
//////////////////////////////////////////////////////////////////////////
#define KEY_MAX_  21			// Ű 21�� 0 ~ 16000
typedef Vector4	QUATERNIONA;

struct KEY
{
	DWORD			tickTime;		// 3DS MAX Animation TickTime (48000tick/sec)
	VECTOR3			Pos;			// �̵�  
	VECTOR3			Scale;			// ������ 
	VECTOR3			ScaleAxis;		// �������� 
	float			ScaleAngle;		// �����ϰ�
	VECTOR3			RotAxis;		// ȸ����
	float			RotAngle;		// ȸ����
	QUATERNIONA		RotQ;			// ȸ��-���ʹϿ�(4����) 

};

struct KEY_POS
{
	DWORD			tickTime;		// 3DS MAX Animation TickTime (48000tick/sec)
	VECTOR3			Pos;			// �̵�  
};

struct KEY_ROT
{
	DWORD			tickTime;		// 3DS MAX Animation TickTime (48000tick/sec)
	VECTOR3			RotAxis;		// ȸ����
	float			RotAngle;		// ȸ����
	QUATERNIONA		RotQ;			// ȸ��-���ʹϿ�(4����) 

	bool			ZeroFrame;
};

struct KEY_SCALE
{
	DWORD			tickTime;		// 3DS MAX Animation TickTime (48000tick/sec)
	VECTOR3			Scale;			// ������ 
	VECTOR3			ScaleAxis;		// �������� 
	float			ScaleAngle;		// �����ϰ�
};


//////////////////////////////////////////////////////////////////////////
//
// class NodeAse : Model Node Ŭ����
//                   
//////////////////////////////////////////////////////////////////////////
class NodeAse// : public AlignedAllocationPolicy<16>
{
	friend class ParserAse;

public:
	TCHAR	m_NodeName[256];	// ���(�޽�) �̸�
	TCHAR	m_NodeParentName[256];	// ���(�θ�) �̸�
	UINT	m_VertexCnt;		// ���� ����
	UINT	m_FaceCnt;			// ���̽� ����
	UINT	m_NormalCnt;		// �븻 ����
	UINT	m_IndexCnt;			// �ε����� �׷��� �� ��������. ���� Face * 3 ��
	UINT	m_TVertexCnt;
	UINT	m_TFaceCnt;

public:
	// ��ȯ ��� ���� : "NodeTM"
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

	// ���� ������ (�ӽ�) ����
	NEW_VERTEX* m_pNewVertexList;	// �븻 ������ ���缭 �籸���� ���� ����Ʈ : VB ��
	FACE_ASE* m_pNewFaceList;		// �븻 ������ ���缭 �籸���� FACE ����Ʈ : IB ��
	VERTEX_ASE* m_pVertList;		// ���� ����Ʈ : VB �� 
	FACE_ASE* m_pFaceList;			// FACE ����Ʈ: IB �� 
	NORMAL_ASE* m_pNormalList;		// �븻 ����Ʈ
	COLOR* m_pCVertList;			// ���� ���� ����Ʈ
	FACE_ASE* m_pCFaceList;			// ���� ����-�ε��� ����Ʈ
	TVERT_ASE* m_pTVertList;
	FACE_ASE* m_pTFaceList;
	SKIN_ASE* m_pSkinList;

	NORMAL_LINE_ASE* m_pNormalLineList;

	// ���� ��尡 � ������ ����ϴ��� �˰� �־�� �Ѵ�.
	int m_MaterialRef;

	// ������ (H/W) ����   
	LPVERTEXBUFFER	m_pVB;
	LPVERTEXBUFFER	m_pNewVB;			// �븻 ������ ���缭 �籸���� ���� ����
	LPVERTEXBUFFER	m_pWeldVB;			// Weld�� ����� ���� ����
	LPVERTEXBUFFER	m_pNormalLineVB;	// ������ �븻 ���� ����
	LPINDEXBUFFER	m_pIB;
	LPINDEXBUFFER	m_pNewIB;			// �븻 ������ ���缭 �籸���� �ε��� ����
	LPINDEXBUFFER	m_pWeldIB;			// Weld�� ����� �ε��� ����

public:
	NodeAse* m_Parent;
	std::vector<NodeAse*> m_Childs;

public:
	// �ִϸ��̼�
	///KEY		m_AniKey[KEY_MAX_];
	std::vector<KEY_POS> m_AniKeyPos;
	std::vector<KEY_ROT> m_AniKeyRot;
	std::vector<KEY_SCALE> m_AniKeyScale;

	XMMATRIX m_AnimTrans;
	XMMATRIX m_AnimRotate;
	XMMATRIX m_AnimScale;

	float	m_CurrTime_Pos;				// ���� ���� �����
	DWORD	m_CurrKey_Pos;				// ���� �ִ� ���� Ű : tickTime ���� ȯ��, ó���ϴ� ���� ����
	DWORD	m_CurrTickTime_Pos;			// ����(���� �÷��̵�) ƽŸ��
	DWORD	m_TotTickTime_Pos;			// �� ƽŸ��

	float	m_CurrTime_Rot;				// ���� ���� �����
	DWORD	m_CurrKey_Rot;				// ���� �ִ� ���� Ű : tickTime ���� ȯ��, ó���ϴ� ���� ����
	DWORD	m_CurrTickTime_Rot;			// ����(���� �÷��̵�) ƽŸ��
	DWORD	m_TotTickTime_Rot;			// �� ƽŸ��

	float	m_CurrTime_Scale;			// ���� ���� �����
	DWORD	m_CurrKey_Scale;			// ���� �ִ� ���� Ű : tickTime ���� ȯ��, ó���ϴ� ���� ����
	DWORD	m_CurrTickTime_Scale;		// ����(���� �÷��̵�) ƽŸ��
	DWORD	m_TotTickTime_Scale;		// �� ƽŸ��

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
	// ���� �޼ҵ�
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

