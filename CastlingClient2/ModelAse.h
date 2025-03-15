#pragma once

#include "DXEngineDefine.h"
#include "NodeAse.h"
#include <vector>

#define ASEMODEL_VER_   3.1		//Ver.3.00

// �Ľ̳���
//Vertex Position
//Vertex Index (Face List)
//Vertex Color
//Vertex Color Index (C-Face List)

// �ļ� ���� ����
class ParserAse;
class NodeAse;



//////////////////////////////////////////////////////////////////////////
//
// ���� ����ü
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
// class ModelAse : ASE Model ���� Ŭ����
//                   
//////////////////////////////////////////////////////////////////////////
class ModelAse
{
	friend class ParserAse;

public:
	// �޽� ������
	TCHAR	m_FileName[256];	// ���ϸ� 

	// �� ���� ��ü ����, �ε��� ����
	UINT	m_OriginalVtxCnt;	// ���� ��ü ���� ����
	UINT	m_FullVtxCnt;		// Ǯ ���ؽ� ���� ����
	UINT	m_WeldVtxCnt;		// ����ȭ ��ü ���� ����
	UINT	m_TotalFaceCnt;		// ���̽� ����

	///std::vector<NodeAse*> m_pNodeList;
	///NodeAse* m_Root;
	std::vector<NodeAse*> m_RootList;
	std::vector<NodeAse*> m_pNodeList;
	std::vector<NodeAse*> m_pBoneList;

	// ���� ������ (�ӽ�) ����
	MATERIAL_ASE* m_pMaterialList;
	UINT			m_MaterialCnt;

	// ����̽� �������̽�
	ID3D11Device* m_pDev;

	// �ִϸ��̼�
	int		m_SceneFirstFrame;
	int		m_SceneLastFrame;
	int		m_SceneFrameSpeed;
	int		m_SceneTickPerFrame;

	float	m_AniTime;				// ��ü �ִϸ��̼� �÷��� �ð�

	//float	m_CurrTime;				// ���� ���� �����
	//DWORD	m_CurrKey;				// ���� �ִ� ���� Ű : tickTime ���� ȯ��, ó���ϴ� ���� ����
	//DWORD	m_CurrTickTime;			// ����(���� �÷��̵�) ƽŸ��
	//DWORD	m_TotTickTime;			// �� ƽŸ��

	bool m_isEnd;

public:
	float	m_AniSpeed;


protected:
	// ���� ���� �籸��
	int _CreateVFBuffers();

	// ������ ���� ����
	HRESULT _CreateHWBuffers();


public:
	//������ ���� ���� �� ���ҽ� �ε�
	int Generate();


public:
	// ������ �� �Ҹ���
	ModelAse(ID3D11Device* pDev);
	virtual ~ModelAse();

	const TCHAR* GetFileName() const { return m_FileName; }

public:
	// �Ϲ� �޼ҵ�
	///virtual int Draw(float dTime = 0.0f);			// �⺻ �׸��� �ڵ�
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
// Model File ���� ����ü (API Ȯ�� ������) 
//
//////////////////////////////////////////////////////////////////////////
struct MODELINFO
{
	//���� �ʿ� ������ �߰� ����.  
	//...
};


// �� �ε�, ���� �Լ� (ver.ASE)
//int  ModelCreateFromASE(ID3D11Device* pDev, TCHAR* filename, MODELINFO* pInfo, ModelAse** ppModel);
int ModelCreateFromASE(ID3D11Device* pDev, /* [in] Device Handle */ const TCHAR* filename, /* [in] Ase ���ϸ� */ MODELINFO* pInfo, /* [in] B3Model ��������ü. ������. �⺻�� NULL. */ ModelAse** ppModel /* [out] �ε��� ������ Model ��ü�� ����������. ���н� NULL ����. */);

// ���� �� ����
void ModelRelease(ModelAse*& ppModel);

// ���� ȣȯ ������
typedef ModelAse				YnAseModel;
typedef ParserAse				YnAseParser;
