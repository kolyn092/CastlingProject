#include "ModelAse.h"
#include "ParserAse.h"
#include "string"
#include "ModelObject.h"
#include "D3DEngine.h"
#include "CGameProcess.h"
#include "NetworkCallback.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// Construction/Destruction
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// ������
//
//////////////////////////////////////////////////////////////////////////
ModelAse::ModelAse(ID3D11Device* pDev)
{
	_tcscpy(m_FileName, L"NA");

	m_OriginalVtxCnt = 0;
	m_FullVtxCnt = 0;
	m_WeldVtxCnt = 0;
	m_TotalFaceCnt = 0;

	m_pMaterialList = NULL;
	m_MaterialCnt = 0;

	///m_Root = nullptr;

	m_pDev = pDev;

	m_SceneFirstFrame = 0;
	m_SceneLastFrame = 0;
	m_SceneFrameSpeed = 0;
	m_SceneTickPerFrame = 0;

	m_AniSpeed = 1.0f;
	m_AniTime = 0.0f;

	m_isEnd = false;

	//m_CurrTickTime = 0;
}


//////////////////////////////////////////////////////////////////////////
//
// �Ҹ���
//
//////////////////////////////////////////////////////////////////////////
ModelAse::~ModelAse()
{
	///SAFE_DELETE(m_Root);
	SAFE_DELETE(m_pMaterialList);

	for (auto& i : m_pNodeList)
	{
		delete i;
	}

	for (auto& i : m_pBoneList)
	{
		delete i;
	}
}

//////////////////////////////////////////////////////////////////////////
//
// �������� �籸��
//
//////////////////////////////////////////////////////////////////////////
int ModelAse::_CreateVFBuffers()
{
	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// �������� �ϵ���� ���� (VB/IB) �����
//
//////////////////////////////////////////////////////////////////////////
HRESULT ModelAse::_CreateHWBuffers()
{
	return CHK_OK;
}



//////////////////////////////////////////////////////////////////////////
//
// �� ������ �籸�� �Ͽ�, �������� ���۸� �����Ѵ�
// ����/�ؽ�ó ������ �籸�� �� �ε��Ѵ�
//
//////////////////////////////////////////////////////////////////////////
int ModelAse::Generate()
{
	//for (int i = 0; i < m_pNodeList.size(); i++)
	//{
	//	m_pNodeList[i]->Generate();
	//}

	for (unsigned int i = 0; i < m_pNodeList.size(); i++)
	{
		m_pNodeList[i]->Generate();
	}

	for (unsigned int i = 0; i < m_pBoneList.size(); i++)
	{
		m_pBoneList[i]->Generate();
	}

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// �⺻ �׸��� �ڵ� : �ʿ��ϴٸ� �ܺο��� ó���ϴ� �͵� �����ϴ�
//
//////////////////////////////////////////////////////////////////////////
// ���(�޽�?)�� �׸��� �Լ�
int ModelAse::NodeDraw(float dTime/*=0.0f*/)
{
	for (unsigned int i = 0; i < m_pNodeList.size(); i++)
	{
		SetMaterial_New(m_pNodeList[i]);
		m_pNodeList[i]->Draw(dTime);
	}

	return CHK_OK;
}

// ���븸 �׸��� �Լ�
int ModelAse::BoneDraw(float dTime/*=0.0f*/)
{
	for (unsigned int i = 0; i < m_pBoneList.size(); i++)
	{
		SetMaterial_New(m_pBoneList[i]);
		m_pBoneList[i]->Draw(dTime);
	}

	return CHK_OK;
}

int ModelAse::CreateTM(NodeAse* node)
{
	///*
	XMMATRIX mTM = XMLoadFloat4x4(&node->m_NodeTM);

	// �θ� ������ �׳� ����. ������ �θ��� ������� �����ְ� ����.
	if (node->m_Parent == nullptr)
	{
		///XMMatrixDecompose(&node->m_LocalVecScale, &node->m_LocalVecRotate, &node->m_LocalVecTrans, mTM);

		node->m_LocalTM = node->m_NodeTM;
	}
	else
	{
		XMMATRIX parentTM = XMLoadFloat4x4(&node->m_Parent->m_NodeTM);
		///mTM = mTM * XMMatrixInverse(nullptr, parentTM);
		///XMMatrixDecompose(&node->m_LocalVecScale, &node->m_LocalVecRotate, &node->m_LocalVecTrans, mTM);

		XMStoreFloat4x4(&node->m_LocalTM, mTM * XMMatrixInverse(nullptr, parentTM));
	}

	// ���ص� �͵�� TM �籸��
	///XMStoreFloat4x4(&node->m_LocalTM, XMMatrixScalingFromVector(node->m_LocalVecScale) * XMMatrixRotationQuaternion(node->m_LocalVecRotate) * XMMatrixTranslationFromVector(node->m_LocalVecTrans));

	// ���߿� �ִϸ��̼ǿ��� ��� �Ѵܴ�
	XMMatrixDecompose(&node->m_LocalVecScale, &node->m_LocalVecRotate, &node->m_LocalVecTrans, XMLoadFloat4x4(&node->m_LocalTM));


	// �ڽĵ��� �ִٸ� ����Լ� ���鼭 TM �籸��
	for (unsigned int i = 0; i < node->m_Childs.size(); i++)
	{
		CreateTM(node->m_Childs[i]);
	}
	//*/

	return CHK_OK;
}

void ModelAse::AnimateAll(float dTime)
{
	for (unsigned int i = 0; i < m_RootList.size(); i++)
	{
		Animate_Pos(dTime, m_RootList[i]);
		Animate_Rot(dTime, m_RootList[i]);
		Animate_Scale(dTime, m_RootList[i]);
		///Animate_Reset(m_RootList[i]);
	}

	///Animate_Pos(dTime, m_Root);
	///Animate_Rot(dTime, m_Root);
	///Animate_Scale(dTime, m_Root);
	///Animate_Reset(m_Root);
}

void ModelAse::AnimateResetAll()
{
	for (unsigned int i = 0; i < m_RootList.size(); i++)
	{
		Animate_Reset(m_RootList[i]);
	}
}

int ModelAse::GetCurrTick(float dTime)
{
	//int TickPassed = dTime * m_SceneFrameSpeed * m_SceneTickPerFrame;
	//int first = m_FirstFrame * m_SceneTickPerFrame;
	//int last = m_LastFrame * m_SceneTickPerFrame;
	//
	//return TickPassed % (last - first) + first;
	return 1;
}


void ModelAse::Animate_Pos(float dTime, NodeAse* node)
{
	// �̵� Ű�� ���� ���
	if (node->m_isTrans)
	{
		// �ִϸ��̼� �ð� ����
		// ���� ����� ���. ������ ������ 1�ʿ� 1���� �÷���
		float aniTime = dTime * m_AniSpeed;

		// ��ü ���� tickTime ���
		//m_CurrTickTime += aniTime * 4800;
		node->m_CurrTickTime_Pos += aniTime * m_SceneTickPerFrame * m_SceneFrameSpeed;

		// ���� ���� ����� ȯ��
		node->m_CurrTime_Pos = FrameTickTime_Pos(node->m_CurrKey_Pos, node->m_CurrTickTime_Pos, node);

		// ���� ������� ���� ���� ������ ��ȯ
		if (node->m_CurrTime_Pos > 1.0f)			// ���� ����� Ȯ��
		{
			node->m_CurrTime_Pos = 0.0f;			// 100% �� ������ �ʱ�ȭ

			//������ ���� Ȯ��, �ʱ�ȭ, �ݺ��÷���..
			///if (++m_CurrKey >= KEY_MAX_ - 1)
			if (++node->m_CurrKey_Pos >= node->m_AniKeyPos.size() - 1)
			{
				// �ٸ� ��ҵ��� ���� �ƴҶ��� ������ ���� �����Ѵ�.
				if (node->m_AniKeyPos[node->m_CurrKey_Pos - 1].tickTime != m_SceneLastFrame * m_SceneTickPerFrame)
				{
					node->m_CurrKey_Pos--;

					CGameProcess* gameProcess = CGameProcess::GetIns();
					if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_ATTACK)
					{
						gameProcess->Reset_Animation_Attack();
						AnimateResetAll();
					}
					else if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_DIE)
					{
						gameProcess->Reset_Animation_Die();
						AnimateResetAll();

						if (setEvent)
						{
							resetEvent = true;
							setEvent = false;
							SetEvent(sendAnimationEvent);
						}
					}
				}
				else
				{
					//for (unsigned int i = 0; i < m_RootList.size(); i++)
					//{
					//	Animate_Reset(m_RootList[i]);
					//}
					//CGameProcess* gameProcess = CGameProcess::GetIns();
					//gameProcess->Reset_Animation_Attack();
				}
			}
		}

		// ���� ������ ���
		KEY_POS startKey = node->m_AniKeyPos[node->m_CurrKey_Pos];
		KEY_POS endKey = node->m_AniKeyPos[node->m_CurrKey_Pos + 1];


		VECTOR3 vPos;
		vPos = Vec3Lerp(startKey.Pos, endKey.Pos, node->m_CurrTime_Pos);
		node->m_AnimTrans = XMMatrixTranslationFromVector(XMLoadFloat3(&vPos));
	}
	else
	{
		node->m_AnimTrans = XMMatrixTranslationFromVector(node->m_LocalVecTrans);
	}

	// �ִϸ��̼� Ű �� �Ǵ� NodeTM�� ���ؼ������� ����TM�� ������ش�.
	///XMStoreFloat4x4(&node->m_LocalTM, scaleTM * rotateTM * transTM);

	for (unsigned int i = 0; i < node->m_Childs.size(); i++)
	{
		Animate_Pos(dTime, node->m_Childs[i]);
	}
}

void ModelAse::Animate_Rot(float dTime, NodeAse* node)
{
	// ȸ�� Ű�� ���� ���
	if (node->m_isRot)
	{
		// �ִϸ��̼� �ð� ����
		// ���� ����� ���. ������ ������ 1�ʿ� 1���� �÷���
		float aniTime = dTime * m_AniSpeed;

		// ��ü ���� tickTime ���
		//m_CurrTickTime += aniTime * 4800;
		node->m_CurrTickTime_Rot += aniTime * m_SceneTickPerFrame * m_SceneFrameSpeed;

		// ���� ���� ����� ȯ��
		node->m_CurrTime_Rot = FrameTickTime_Rot(node->m_CurrKey_Rot, node->m_CurrTickTime_Rot, node);

		// ���� ������� ���� ���� ������ ��ȯ
		if (node->m_CurrTime_Rot > 1.0f)			// ���� ����� Ȯ��
		{
			node->m_CurrTime_Rot = 0.0f;			// 100% �� ������ �ʱ�ȭ

			//������ ���� Ȯ��, �ʱ�ȭ, �ݺ��÷���..
			///if (++m_CurrKey >= KEY_MAX_ - 1)
			if (++node->m_CurrKey_Rot >= node->m_AniKeyRot.size() - 1)
			{
				if (node->m_AniKeyRot[node->m_CurrKey_Rot - 1].tickTime != m_SceneLastFrame * m_SceneTickPerFrame)
				{
					node->m_CurrKey_Rot--;

					CGameProcess* gameProcess = CGameProcess::GetIns();
					if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_ATTACK)
					{
						gameProcess->Reset_Animation_Attack();
						AnimateResetAll();
					}
					else if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_DIE)
					{
						gameProcess->Reset_Animation_Die();
						AnimateResetAll();

						if (setEvent)
						{
							resetEvent = true;
							setEvent = false;
							SetEvent(sendAnimationEvent);
						}
					}
				}
				else
				{
					//for (unsigned int i = 0; i < m_RootList.size(); i++)
					//{
					//	Animate_Reset(m_RootList[i]);
					//}
					//CGameProcess* gameProcess = CGameProcess::GetIns();
					//gameProcess->Reset_Animation_Attack();
				}
			}
		}

		// ���� ������ ���
		KEY_ROT startKey = node->m_AniKeyRot[node->m_CurrKey_Rot];
		KEY_ROT endKey = node->m_AniKeyRot[node->m_CurrKey_Rot + 1];

		XMVECTOR quat = XMQuaternionSlerp(startKey.RotQ, endKey.RotQ, node->m_CurrTime_Rot);	//����� ����
		node->m_AnimRotate = XMMatrixRotationQuaternion(quat);
	}
	else
	{
		node->m_AnimRotate = XMMatrixRotationQuaternion(node->m_LocalVecRotate);
	}

	// �ִϸ��̼� Ű �� �Ǵ� NodeTM�� ���ؼ������� ����TM�� ������ش�.
	///XMStoreFloat4x4(&node->m_LocalTM, scaleTM * rotateTM * transTM);

	for (unsigned int i = 0; i < node->m_Childs.size(); i++)
	{
		Animate_Rot(dTime, node->m_Childs[i]);
	}
}

void ModelAse::Animate_Scale(float dTime, NodeAse* node)
{
	// ������ Ű�� ���� ���
	if (node->m_isScale)
	{
		// �ִϸ��̼� �ð� ����
		// ���� ����� ���. ������ ������ 1�ʿ� 1���� �÷���
		float aniTime = dTime * m_AniSpeed;

		// ��ü ���� tickTime ���
		//m_CurrTickTime += aniTime * 4800;
		node->m_CurrTickTime_Scale += aniTime * m_SceneTickPerFrame * m_SceneFrameSpeed;

		// ���� ���� ����� ȯ��
		node->m_CurrTime_Scale = FrameTickTime_Scale(node->m_CurrKey_Scale, node->m_CurrTickTime_Scale, node);

		// ���� ������� ���� ���� ������ ��ȯ
		if (node->m_CurrTime_Scale > 1.0f)			// ���� ����� Ȯ��
		{
			node->m_CurrTime_Scale = 0.0f;			// 100% �� ������ �ʱ�ȭ

			//������ ���� Ȯ��, �ʱ�ȭ, �ݺ��÷���..
			///if (++m_CurrKey >= KEY_MAX_ - 1)
			if (++node->m_CurrKey_Scale >= node->m_AniKeyScale.size() - 1)
			{
				node->m_CurrKey_Scale = 0;			// ���� Ű �ʱ�ȭ
				node->m_CurrTickTime_Scale = 0;		// ���� �� ����� �ʱ�ȭ
			}
		}

		// ���� ������ ���
		KEY_SCALE startKey = node->m_AniKeyScale[node->m_CurrKey_Scale];
		KEY_SCALE endKey = node->m_AniKeyScale[node->m_CurrKey_Scale + 1];


		VECTOR3 vScale;
		vScale = Vec3Lerp(startKey.Scale, endKey.Scale, node->m_CurrTime_Scale);
		node->m_AnimScale = XMMatrixScalingFromVector(XMLoadFloat3(&vScale));
	}
	else
	{
		node->m_AnimScale = XMMatrixScalingFromVector(node->m_LocalVecScale);
	}

	// �ִϸ��̼� Ű �� �Ǵ� NodeTM�� ���ؼ������� ����TM�� ������ش�.
	if (node->m_Parent == nullptr)
	{
		XMStoreFloat4x4(&node->m_LocalTM, node->m_AnimScale * node->m_AnimRotate * node->m_AnimTrans);
	}
	else
	{
		XMStoreFloat4x4(&node->m_LocalTM, node->m_AnimScale * node->m_AnimRotate * node->m_AnimTrans);
	}


	for (unsigned int i = 0; i < node->m_Childs.size(); i++)
	{
		Animate_Scale(dTime, node->m_Childs[i]);
	}
}

void ModelAse::Animate_Reset(NodeAse* node)
{
	///*
	//DWORD maxTickTime_Result = 0;
	//DWORD maxTickTime_Pos = 0;
	//DWORD maxTickTime_Rot = 0;
	//DWORD maxTickTime_Scale = 0;

	//if (node->m_AniKeyPos.size() > 0)
	//{
	//	maxTickTime_Pos = node->m_AniKeyPos[node->m_AniKeyPos.size() - 1].tickTime;
	//}
	//if (node->m_AniKeyRot.size() > 0)
	//{
	//	maxTickTime_Rot = node->m_AniKeyRot[node->m_AniKeyRot.size() - 1].tickTime;
	//}
	//if (node->m_AniKeyScale.size() > 0)
	//{
	//	maxTickTime_Scale = node->m_AniKeyScale[node->m_AniKeyScale.size() - 1].tickTime;
	//}

	//// �ִ� ƽŸ�� ���
	//if (maxTickTime_Pos >= maxTickTime_Rot)
	//{
	//	if (maxTickTime_Pos >= maxTickTime_Scale)
	//	{
	//		maxTickTime_Result = maxTickTime_Pos;
	//	}
	//	else
	//	{
	//		maxTickTime_Result = maxTickTime_Scale;
	//	}
	//}
	//else
	//{
	//	if (maxTickTime_Rot >= maxTickTime_Scale)
	//	{
	//		maxTickTime_Result = maxTickTime_Rot;
	//	}
	//	else
	//	{
	//		maxTickTime_Result = maxTickTime_Scale;
	//	}
	//}

	//maxTickTime_Result = m_SceneLastFrame * m_SceneTickPerFrame;

	// ���̸� �ʱ�ȭ�� �Ѵ�.
	//if ((node->m_AniKeyPos.size() > 0 && (node->m_AniKeyPos[node->m_CurrKey_Pos + 1].tickTime == maxTickTime_Result))
	//	|| (node->m_AniKeyRot.size() > 0 && (node->m_AniKeyRot[node->m_CurrKey_Rot + 1].tickTime == maxTickTime_Result))
	//	|| (node->m_AniKeyScale.size() > 0 && (node->m_AniKeyScale[node->m_CurrKey_Scale + 1].tickTime == maxTickTime_Result)))
	//{
	node->m_CurrKey_Pos = 0;
	node->m_CurrTickTime_Pos = 0;
	node->m_CurrKey_Rot = 0;
	node->m_CurrTickTime_Rot = 0;
	node->m_CurrKey_Scale = 0;
	node->m_CurrTickTime_Scale = 0;

	for (unsigned int i = 0; i < node->m_Childs.size(); i++)
	{
		Animate_Reset(node->m_Childs[i]);
	}

	//	CGameProcess* gameProcess = CGameProcess::GetIns();
	//	if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_ATTACK)
	//	{
	//		gameProcess->Reset_Animation_Attack();
	//	}
	//	else if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_DIE)
	//	{
	//		gameProcess->Reset_Animation_Die();
	//	}
	//}
//*/

/*
if (++node->m_CurrKey_Pos >= node->m_AniKeyPos.size() - 1 || ++node->m_CurrKey_Rot >= node->m_AniKeyRot.size() - 1)
{
	if (node->m_AniKeyPos[node->m_CurrKey_Pos - 1].tickTime == m_SceneLastFrame * m_SceneTickPerFrame
		|| node->m_AniKeyRot[node->m_CurrKey_Rot - 1].tickTime == m_SceneLastFrame * m_SceneTickPerFrame)
	{
		m_isEnd = true;
	}

	for (unsigned int i = 0; i < node->m_Childs.size(); i++)
	{
		Animate_Reset(node->m_Childs[i]);
	}
}

if (m_isEnd)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();
	if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_ATTACK)
	{
		gameProcess->Reset_Animation_Attack();
	}
	else if (gameProcess->m_InGameState == eInGameState::ACTION_UNIT_ANIMATION_DIE)
	{
		gameProcess->Reset_Animation_Die();
	}
}
*/
}


float ModelAse::FrameTickTime_Pos(DWORD currKey, DWORD currTickTime, NodeAse* node)
{
	float currTime = 0.0f;

	/// (�� ����ð� - ���� ����) / (���� �� - ���� ����)
	KEY_POS startKey = node->m_AniKeyPos[node->m_CurrKey_Pos];
	KEY_POS endKey = node->m_AniKeyPos[node->m_CurrKey_Pos + 1];

	if (endKey.tickTime - startKey.tickTime == 0)
		return 1;

	currTime = (float)(currTickTime - startKey.tickTime) / (float)(endKey.tickTime - startKey.tickTime);

	return currTime;
}

float ModelAse::FrameTickTime_Rot(DWORD currKey, DWORD currTickTime, NodeAse* node)
{
	float currTime = 0.0f;

	/// (�� ����ð� - ���� ����) / (���� �� - ���� ����)
	KEY_ROT startKey = node->m_AniKeyRot[node->m_CurrKey_Rot];
	KEY_ROT endKey = node->m_AniKeyRot[node->m_CurrKey_Rot + 1];

	if (endKey.tickTime - startKey.tickTime == 0)
		return 1;

	currTime = (float)(currTickTime - startKey.tickTime) / (float)(endKey.tickTime - startKey.tickTime);

	return currTime;
}

float ModelAse::FrameTickTime_Scale(DWORD currKey, DWORD currTickTime, NodeAse* node)
{
	float currTime = 0.0f;

	/// (�� ����ð� - ���� ����) / (���� �� - ���� ����)
	KEY_SCALE startKey = node->m_AniKeyScale[node->m_CurrKey_Scale];
	KEY_SCALE endKey = node->m_AniKeyScale[node->m_CurrKey_Scale + 1];

	if (endKey.tickTime - startKey.tickTime == 0)
		return 1;

	currTime = (float)(currTickTime - startKey.tickTime) / (float)(endKey.tickTime - startKey.tickTime);

	return currTime;
}

float ModelAse::_Lerp(float& v0, float& v1, float& a)
{
	return (v0 * (1 - a) + (v1 * a));
}

VECTOR3 ModelAse::Vec3Lerp(VECTOR3& v0, VECTOR3& v1, float& a)
{
	float x, y, z;
	x = _Lerp(v0.x, v1.x, a);
	y = _Lerp(v0.y, v1.y, a);
	z = _Lerp(v0.z, v1.z, a);

	return VECTOR3(x, y, z);
}

NodeAse* ModelAse::FindNodeByName(TCHAR* name)
{
	for (unsigned int i = 0; i < m_pBoneList.size(); i++)
	{
		if (EQUAL(m_pBoneList[i]->GetNodeName(), name))
		{
			return m_pBoneList[i];
		}
	}

	for (unsigned int i = 0; i < m_pNodeList.size(); i++)
	{
		if (EQUAL(m_pNodeList[i]->GetNodeName(), name))
		{
			return m_pNodeList[i];
		}
	}

	return nullptr;
}

int ModelAse::ModelDebugData()
{
	return CHK_OK;
}


int ModelAse::SetMaterial_New(NodeAse* node)
{
	/*
	g_pDXDC->PSSetSamplers(0, 1, &g_pSampler[SS_CLAMP]);
	///g_pDXDC->PSSetSamplers(0, 1, &g_pSampler[SS_WRAP]);

	int textureIndex = node->GetMaterialRef();

	// ���� ����
	if (textureIndex != -1)
	{
		g_cbMtrl.Diffuse = m_pMaterialList[textureIndex].Diffuse;
		g_cbMtrl.Ambient = m_pMaterialList[textureIndex].Ambient;
		g_cbMtrl.Specular = m_pMaterialList[textureIndex].SpecularCol;
		g_cbMtrl.Power = m_pMaterialList[textureIndex].Power;
	}
	else
	{
		g_cbMtrl.Diffuse = COLOR(1, 1, 1, 1);
		g_cbMtrl.Ambient = COLOR(1, 1, 1, 1);
		g_cbMtrl.Specular = COLOR(1, 1, 1, 1);
		g_cbMtrl.Power = 30.0f;
	}

	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBMtrl, &g_cbMtrl, sizeof(cbMATERIAL));
	g_pDXDC->VSSetConstantBuffers(2, 1, &g_pCBMtrl);

	if (m_pMaterialList[textureIndex].m_Texture != nullptr)// && textureIndex != -1)
	{
		g_pDXDC->VSSetShader(g_pVS, nullptr, 0);
		g_pDXDC->PSSetShader(g_pPS, nullptr, 0);

		g_pDXDC->PSSetShaderResources(0, 1, &m_pMaterialList[textureIndex].m_Texture);
	}
	else
	{
		g_pDXDC->VSSetShader(g_pVS_NonTex, nullptr, 0);
		g_pDXDC->PSSetShader(g_pPS_NonTex, nullptr, 0);
	}
	//*/

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// ModelCreateFromASE : Ase Model ���� �ε�/��ü ���� �Լ�
//                      : ���н� YN_Error ����
// 
//////////////////////////////////////////////////////////////////////////
int ModelCreateFromASE(ID3D11Device* pDev,			// [in] Device Handle
	const TCHAR* filename,		// [in] Ase ���ϸ�
	MODELINFO* pInfo,		// [in] B3Model ��������ü. ������. �⺻�� NULL.
	ModelAse** ppModel		// [out] �ε��� ������ Model ��ü�� ����������. ���н� NULL ����.
)
{
	// �ļ� ����
	ParserAse* pParser = new ParserAse;
	if (pParser == NULL)
	{
		*ppModel = NULL;
		return CHK_ERROR;
	}

	// ASE ��ü ����
	ModelAse* pModel = new ModelAse(pDev);
	assert(pModel != NULL);

	// ASE ������ �б�
	if (CHECKFAILED(pParser->Load(pDev, filename, pModel)))
	{
		SAFE_DELETE(pParser);
		SAFE_DELETE(pModel);
		*ppModel = NULL;
		return CHK_ERROR;
	}

	// �������� ���� ���� �� ���ҽ� �ε�
	if (CHECKFAILED(pModel->Generate()))
	{
		SAFE_DELETE(pParser);
		SAFE_DELETE(pModel);
		*ppModel = NULL;
		return CHK_ERROR;
	}

	// �۾� �Ϸ�. ����
	*ppModel = pModel;			// �� ����
	SAFE_DELETE(pParser);		// �ļ� ����

	return  CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// ynModelRelease : Ase Model ���� �޸� ���� �Լ�
//                 
//////////////////////////////////////////////////////////////////////////
void ModelRelease(ModelAse*& pModel)
{
	SAFE_DELETE(pModel);
	pModel = NULL;
}
