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
// 생성자
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
// 소멸자
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
// 정점버퍼 재구성
//
//////////////////////////////////////////////////////////////////////////
int ModelAse::_CreateVFBuffers()
{
	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 렌더링용 하드웨어 버퍼 (VB/IB) 만들기
//
//////////////////////////////////////////////////////////////////////////
HRESULT ModelAse::_CreateHWBuffers()
{
	return CHK_OK;
}



//////////////////////////////////////////////////////////////////////////
//
// 모델 정보를 재구성 하여, 렌더링용 버퍼를 구성한다
// 재질/텍스처 정보의 재구성 및 로드한다
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
// 기본 그리기 코드 : 필요하다면 외부에서 처리하는 것도 가능하다
//
//////////////////////////////////////////////////////////////////////////
// 노드(메쉬?)만 그리는 함수
int ModelAse::NodeDraw(float dTime/*=0.0f*/)
{
	for (unsigned int i = 0; i < m_pNodeList.size(); i++)
	{
		SetMaterial_New(m_pNodeList[i]);
		m_pNodeList[i]->Draw(dTime);
	}

	return CHK_OK;
}

// 뼈대만 그리는 함수
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

	// 부모가 없으면 그냥 분해. 있으면 부모의 역행렬을 곱해주고 분해.
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

	// 분해된 것들로 TM 재구성
	///XMStoreFloat4x4(&node->m_LocalTM, XMMatrixScalingFromVector(node->m_LocalVecScale) * XMMatrixRotationQuaternion(node->m_LocalVecRotate) * XMMatrixTranslationFromVector(node->m_LocalVecTrans));

	// 나중에 애니메이션에서 써야 한단다
	XMMatrixDecompose(&node->m_LocalVecScale, &node->m_LocalVecRotate, &node->m_LocalVecTrans, XMLoadFloat4x4(&node->m_LocalTM));


	// 자식들이 있다면 재귀함수 돌면서 TM 재구성
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
	// 이동 키가 있을 경우
	if (node->m_isTrans)
	{
		// 애니메이션 시간 조절
		// 구간 진행률 계산. 현재의 조건은 1초에 1구간 플레이
		float aniTime = dTime * m_AniSpeed;

		// 전체 진행 tickTime 계산
		//m_CurrTickTime += aniTime * 4800;
		node->m_CurrTickTime_Pos += aniTime * m_SceneTickPerFrame * m_SceneFrameSpeed;

		// 현재 구간 진행률 환산
		node->m_CurrTime_Pos = FrameTickTime_Pos(node->m_CurrKey_Pos, node->m_CurrTickTime_Pos, node);

		// 구간 진행률에 따른 다음 프레임 전환
		if (node->m_CurrTime_Pos > 1.0f)			// 구간 진행률 확인
		{
			node->m_CurrTime_Pos = 0.0f;			// 100% 가 지나면 초기화

			//마지막 구간 확인, 초기화, 반복플레이..
			///if (++m_CurrKey >= KEY_MAX_ - 1)
			if (++node->m_CurrKey_Pos >= node->m_AniKeyPos.size() - 1)
			{
				// 다른 요소들의 끝이 아닐때는 마지막 값을 유지한다.
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

		// 현재 구간의 계산
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

	// 애니메이션 키 값 또는 NodeTM의 분해성분으로 로컬TM을 만들어준다.
	///XMStoreFloat4x4(&node->m_LocalTM, scaleTM * rotateTM * transTM);

	for (unsigned int i = 0; i < node->m_Childs.size(); i++)
	{
		Animate_Pos(dTime, node->m_Childs[i]);
	}
}

void ModelAse::Animate_Rot(float dTime, NodeAse* node)
{
	// 회전 키가 있을 경우
	if (node->m_isRot)
	{
		// 애니메이션 시간 조절
		// 구간 진행률 계산. 현재의 조건은 1초에 1구간 플레이
		float aniTime = dTime * m_AniSpeed;

		// 전체 진행 tickTime 계산
		//m_CurrTickTime += aniTime * 4800;
		node->m_CurrTickTime_Rot += aniTime * m_SceneTickPerFrame * m_SceneFrameSpeed;

		// 현재 구간 진행률 환산
		node->m_CurrTime_Rot = FrameTickTime_Rot(node->m_CurrKey_Rot, node->m_CurrTickTime_Rot, node);

		// 구간 진행률에 따른 다음 프레임 전환
		if (node->m_CurrTime_Rot > 1.0f)			// 구간 진행률 확인
		{
			node->m_CurrTime_Rot = 0.0f;			// 100% 가 지나면 초기화

			//마지막 구간 확인, 초기화, 반복플레이..
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

		// 현재 구간의 계산
		KEY_ROT startKey = node->m_AniKeyRot[node->m_CurrKey_Rot];
		KEY_ROT endKey = node->m_AniKeyRot[node->m_CurrKey_Rot + 1];

		XMVECTOR quat = XMQuaternionSlerp(startKey.RotQ, endKey.RotQ, node->m_CurrTime_Rot);	//사원수 보간
		node->m_AnimRotate = XMMatrixRotationQuaternion(quat);
	}
	else
	{
		node->m_AnimRotate = XMMatrixRotationQuaternion(node->m_LocalVecRotate);
	}

	// 애니메이션 키 값 또는 NodeTM의 분해성분으로 로컬TM을 만들어준다.
	///XMStoreFloat4x4(&node->m_LocalTM, scaleTM * rotateTM * transTM);

	for (unsigned int i = 0; i < node->m_Childs.size(); i++)
	{
		Animate_Rot(dTime, node->m_Childs[i]);
	}
}

void ModelAse::Animate_Scale(float dTime, NodeAse* node)
{
	// 스케일 키가 있을 경우
	if (node->m_isScale)
	{
		// 애니메이션 시간 조절
		// 구간 진행률 계산. 현재의 조건은 1초에 1구간 플레이
		float aniTime = dTime * m_AniSpeed;

		// 전체 진행 tickTime 계산
		//m_CurrTickTime += aniTime * 4800;
		node->m_CurrTickTime_Scale += aniTime * m_SceneTickPerFrame * m_SceneFrameSpeed;

		// 현재 구간 진행률 환산
		node->m_CurrTime_Scale = FrameTickTime_Scale(node->m_CurrKey_Scale, node->m_CurrTickTime_Scale, node);

		// 구간 진행률에 따른 다음 프레임 전환
		if (node->m_CurrTime_Scale > 1.0f)			// 구간 진행률 확인
		{
			node->m_CurrTime_Scale = 0.0f;			// 100% 가 지나면 초기화

			//마지막 구간 확인, 초기화, 반복플레이..
			///if (++m_CurrKey >= KEY_MAX_ - 1)
			if (++node->m_CurrKey_Scale >= node->m_AniKeyScale.size() - 1)
			{
				node->m_CurrKey_Scale = 0;			// 시작 키 초기화
				node->m_CurrTickTime_Scale = 0;		// 구간 내 진행률 초기화
			}
		}

		// 현재 구간의 계산
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

	// 애니메이션 키 값 또는 NodeTM의 분해성분으로 로컬TM을 만들어준다.
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

	//// 최대 틱타임 계산
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

	// 끝이면 초기화를 한다.
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

	/// (총 진행시간 - 구간 시작) / (구간 끝 - 구간 시작)
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

	/// (총 진행시간 - 구간 시작) / (구간 끝 - 구간 시작)
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

	/// (총 진행시간 - 구간 시작) / (구간 끝 - 구간 시작)
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

	// 재질 설정
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
// ModelCreateFromASE : Ase Model 파일 로드/개체 생성 함수
//                      : 실패시 YN_Error 리턴
// 
//////////////////////////////////////////////////////////////////////////
int ModelCreateFromASE(ID3D11Device* pDev,			// [in] Device Handle
	const TCHAR* filename,		// [in] Ase 파일명
	MODELINFO* pInfo,		// [in] B3Model 정보구조체. 사용안함. 기본값 NULL.
	ModelAse** ppModel		// [out] 로딩후 리턴할 Model 개체의 이중포인터. 실패시 NULL 리턴.
)
{
	// 파서 생성
	ParserAse* pParser = new ParserAse;
	if (pParser == NULL)
	{
		*ppModel = NULL;
		return CHK_ERROR;
	}

	// ASE 객체 생성
	ModelAse* pModel = new ModelAse(pDev);
	assert(pModel != NULL);

	// ASE 데이터 읽기
	if (CHECKFAILED(pParser->Load(pDev, filename, pModel)))
	{
		SAFE_DELETE(pParser);
		SAFE_DELETE(pModel);
		*ppModel = NULL;
		return CHK_ERROR;
	}

	// 렌더링용 버퍼 구성 및 리소스 로딩
	if (CHECKFAILED(pModel->Generate()))
	{
		SAFE_DELETE(pParser);
		SAFE_DELETE(pModel);
		*ppModel = NULL;
		return CHK_ERROR;
	}

	// 작업 완료. 성공
	*ppModel = pModel;			// 모델 리턴
	SAFE_DELETE(pParser);		// 파서 제거

	return  CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// ynModelRelease : Ase Model 파일 메모리 해제 함수
//                 
//////////////////////////////////////////////////////////////////////////
void ModelRelease(ModelAse*& pModel)
{
	SAFE_DELETE(pModel);
	pModel = NULL;
}
