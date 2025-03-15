#include "D3DEngine.h"
#include "NodeAse.h"

NodeAse::NodeAse()
{
	_tcscpy(m_NodeName, L"NA");
	_tcscpy(m_NodeParentName, L"NA");

	m_VertexCnt = 0;
	m_FaceCnt = 0;
	m_NormalCnt = 0;
	m_IndexCnt = 0;
	m_TVertexCnt = 0;
	m_TFaceCnt = 0;

	m_NodeTM = Matrix::Identity;
	m_LocalTM = Matrix::Identity;
	m_mTrans = m_mScale = m_mRot = Matrix::Identity;

	m_LocalTrans = XMMatrixIdentity();
	m_LocalRotate = XMMatrixIdentity();
	m_LocalScale = XMMatrixIdentity();

	m_LocalVecTrans = Vector4::Zero;
	m_LocalVecRotate = Vector4::Zero;
	m_LocalVecScale = Vector4::Zero;

	m_isTrans = FALSE;
	m_isRot = FALSE;
	m_isScale = FALSE;

	m_vPos = VECTOR3(0, 0, 0);
	m_vScale = VECTOR3(1, 1, 1);
	m_vScaleAxis = VECTOR3(0, 0, 0);
	m_vScaleAxisAng = 0.0f;
	m_vRot = VECTOR3(0, 0, 0);
	m_vRotAxis = VECTOR3(0, 0, 0);
	m_vRotAngle = 0.0f;

	m_pNewVertexList = nullptr;
	m_pNewFaceList = nullptr;
	m_pVertList = nullptr;
	m_pFaceList = nullptr;
	m_pNormalList = nullptr;
	m_pCVertList = nullptr;
	m_pCFaceList = nullptr;
	m_pTVertList = nullptr;
	m_pTFaceList = nullptr;
	m_pSkinList = nullptr;

	m_pNormalLineList = nullptr;

	m_MaterialRef = -1;

	m_pVB = nullptr;
	m_pNewVB = nullptr;
	m_pWeldVB = nullptr;
	m_pNormalLineVB = nullptr;
	m_pIB = nullptr;
	m_pNewIB = nullptr;
	m_pWeldIB = nullptr;

	m_Parent = nullptr;

	// 애니메이션
	//ASSERT(m_AniKey != NULL);
	//ZeroMemory(m_AniKey, sizeof(KEY) * KEY_MAX_);

	m_AnimTrans = XMMatrixIdentity();
	m_AnimScale = XMMatrixIdentity();
	m_AnimRotate = XMMatrixIdentity();

	m_CurrTime_Pos = 0.0f;
	m_CurrKey_Pos = 0;
	m_CurrTickTime_Pos = 0;
	m_TotTickTime_Pos = 0;

	m_CurrTime_Rot = 0.0f;
	m_CurrKey_Rot = 0;
	m_CurrTickTime_Rot = 0;
	m_TotTickTime_Rot = 0;

	m_CurrTime_Scale = 0.0f;
	m_CurrKey_Scale = 0;
	m_CurrTickTime_Scale = 0;
	m_TotTickTime_Scale = 0;

}

NodeAse::~NodeAse()
{
	// 임시 버퍼 삭제
	SAFE_DELETE(m_pNewVertexList);
	SAFE_DELETE(m_pNewFaceList);
	SAFE_DELETE(m_pVertList);
	SAFE_DELETE(m_pFaceList);
	SAFE_DELETE(m_pNormalList);
	SAFE_DELETE(m_pCVertList);
	SAFE_DELETE(m_pCFaceList);
	SAFE_DELETE(m_pTVertList);
	SAFE_DELETE(m_pTFaceList);

	SAFE_DELETE(m_pNormalLineList);

	// 렌더링 버퍼 제거
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pNewVB);
	SAFE_RELEASE(m_pWeldVB);
	SAFE_RELEASE(m_pNormalLineVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pNewIB);
	SAFE_RELEASE(m_pWeldIB);

	///SAFE_DELETE(m_Parent);
	///m_Parent = nullptr;
}

//////////////////////////////////////////////////////////////////////////
//
// 정점버퍼 재구성
//
//////////////////////////////////////////////////////////////////////////
int NodeAse::_CreateVFBuffers()
{
	int vn = 0;
	int cn = 0;

	// 정점색이 구성되지 않았을 경우
	if (m_pCFaceList == NULL)
	{
		// 정점색은 무조건, '흰색' 처리
		for (UINT i = 0; i < m_VertexCnt; i++)
			m_pVertList[i].dwColor = COLOR(1, 1, 1, 1);

		return 0;
	}

	// 정점색이 있을 경우
	// 정점 리스트에 정점색을 등록한다.
	for (UINT i = 0; i < m_FaceCnt; i++)
	{
		vn = m_pFaceList[i].A;							// 페이스 별 정점 번호 얻기
		cn = m_pCFaceList[i].A;							// 페이스 별 정점색 번호 얻기	
		m_pVertList[vn].dwColor = m_pCVertList[cn];		// 정점에 정점색 설정

		vn = m_pFaceList[i].B;
		cn = m_pCFaceList[i].B;
		m_pVertList[vn].dwColor = m_pCVertList[cn];

		vn = m_pFaceList[i].C;
		cn = m_pCFaceList[i].C;
		m_pVertList[vn].dwColor = m_pCVertList[cn];
	}

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// 렌더링용 하드웨어 버퍼 (VB/IB) 만들기
//
//////////////////////////////////////////////////////////////////////////
HRESULT NodeAse::_CreateHWBuffers()
{
	HRESULT res = S_OK;

	// 정점 버퍼 만들기
	//UINT size = sizeof(VERTEX_ASE) * m_VertexCnt;
	//res = CreateVB(g_pDevice, m_pVertList, size, &m_pVB);

	// 인덱스 버퍼 만들기
	//size = sizeof(FACE_ASE) * m_FaceCnt;
	//res = CreateIB(g_pDevice, m_pFaceList, size, &m_pIB);

	// 정점 버퍼 만들기 (노말 개수로 새로 만드는 것)
	UINT size = sizeof(NEW_VERTEX) * m_FaceCnt * 3;
	res = D3DEngine::GetIns()->CreateVB(m_pNewVertexList, size, &m_pNewVB);

	// 인덱스 버퍼 만들기 (노말 개수로 새로 만드는 것)
	size = sizeof(FACE_ASE) * m_FaceCnt;
	res = D3DEngine::GetIns()->CreateIB(m_pNewFaceList, size, &m_pNewIB);

	// 노말 디버깅용 버퍼 만들기
	size = sizeof(NORMAL_LINE_ASE) * m_NormalCnt * 2;
	res = D3DEngine::GetIns()->CreateVB(m_pNormalLineList, size, &m_pNormalLineVB);

	return res;
}

//////////////////////////////////////////////////////////////////////////
//
// 모델 정보를 재구성 하여, 렌더링용 버퍼를 구성한다
// 재질/텍스처 정보의 재구성 및 로드한다
//
//////////////////////////////////////////////////////////////////////////
int NodeAse::Generate()
{
	// 정점버퍼 재구성
	_CreateVFBuffers();

	// 하드웨어 버퍼 생성
	HRESULT res = _CreateHWBuffers();
	if (FAILED(res))
	{
		///ynError(res, L"[Error] NodeAse::_CreateHWBuffers() 실패 \n"
		///	"Ase File=%s", m_FileName);
	}

	// 인덱스 버퍼로 그려질 총 정점 개수
	m_IndexCnt = m_FaceCnt * 3;

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// 기본 그리기 코드 : 필요하다면 외부에서 처리하는 것도 가능하다
//
//////////////////////////////////////////////////////////////////////////
int NodeAse::Draw(float dTime/*=0.0f*/)
{
	/*
	UINT stride = sizeof(NEW_VERTEX);
	UINT offset = 0;
	g_pDXDC->IASetVertexBuffers(0, 1, &m_pNewVB, &stride, &offset);
	g_pDXDC->IASetIndexBuffer(m_pNewIB, DXGI_FORMAT_R32_UINT, offset);

	// 입력 레이아웃 설정
	g_pDXDC->IASetInputLayout(g_pVBLayout);

	// 기하 위상 구조 설정
	g_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 조명 설정
	g_pDXDC->VSSetConstantBuffers(1, 1, &g_pCBLit);
	UpdateDynamicConstantBuffer(g_pDXDC, g_pCBLit, &g_cbLit, sizeof(cbLIGHT));

	//
	//XMMATRIX mTM = XMLoadFloat4x4(&m_LocalTM);
	//if (m_Parent)
	//{
	//	mTM = mTM * XMLoadFloat4x4(&m_Parent->m_LocalTM);
	//}
	//else
	//{
	//	mTM = mTM;// *m_mUserTM;
	//}
	//

	XMMATRIX mTM = GetWorldTM();

	SetTransform(TS_WORLD, mTM);

	// 그리기
	g_pDXDC->DrawIndexed(m_IndexCnt, 0, 0);
	//g_pDXDC->Draw(m_IndexCnt * 3, 0);
	//*/
	return CHK_OK;
}

XMMATRIX NodeAse::GetWorldTM()
{
	XMMATRIX mLocalTM = XMLoadFloat4x4(&m_LocalTM);

	if (m_Parent)
	{
		return mLocalTM * m_Parent->GetWorldTM();
	}
	else
	{
		return mLocalTM;
	}
}

int NodeAse::NormalLineDraw()
{
	//UINT stride = sizeof(NORMAL_LINE_ASE);
	//UINT offset = 0;
	//g_pDXDC->IASetVertexBuffers(0, 1, &m_pNormalLineVB, &stride, &offset);

	//// 입력 레이아웃 설정
	//g_pDXDC->IASetInputLayout(g_pVBNormalLineLayout);

	//// 기하 위상 구조 설정
	//g_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//g_pDXDC->VSSetShader(g_pVS_Line, nullptr, 0);
	//g_pDXDC->PSSetShader(g_pPS_Line, nullptr, 0);

	//g_pDXDC->Draw(m_NormalCnt * 2, 0);

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// 노말 개수를 기준으로 다시 세팅을 한다.
//
//////////////////////////////////////////////////////////////////////////
int NodeAse::SetNewVertexList()
{
	// 새로운 버텍스 리스트 생성 및 초기화
	UINT newVtxCnt = m_FaceCnt * 3;
	m_pNewVertexList = new NEW_VERTEX[newVtxCnt];
	assert(m_pNewVertexList != NULL);
	ZeroMemory(m_pNewVertexList, sizeof(NEW_VERTEX) * newVtxCnt);

	UINT nrmLineCnt = m_NormalCnt * 2;
	m_pNormalLineList = new NORMAL_LINE_ASE[nrmLineCnt];
	assert(m_pNormalLineList != NULL);
	ZeroMemory(m_pNormalLineList, sizeof(NORMAL_LINE_ASE) * nrmLineCnt);

	m_pNewFaceList = new FACE_ASE[m_FaceCnt];
	assert(m_pNewFaceList != NULL);
	ZeroMemory(m_pNewFaceList, sizeof(FACE_ASE) * m_FaceCnt);

	for (UINT i = 0; i < m_FaceCnt; i++)
	{
		/// 페이스 순서대로 넣기 0~35
		m_pNewFaceList[i].A = (i * 3);
		m_pNewFaceList[i].B = (i * 3) + 1;
		m_pNewFaceList[i].C = (i * 3) + 2;

		/// 노말 순서대로 넣기
		m_pNewVertexList[(i * 3)].vNormal = m_pNormalList[i].vtxNormal[0];
		m_pNewVertexList[(i * 3) + 1].vNormal = m_pNormalList[i].vtxNormal[1];
		m_pNewVertexList[(i * 3) + 2].vNormal = m_pNormalList[i].vtxNormal[2];

		/// 버텍스 순서는 facelist 이용
		int nowIndexA = m_pFaceList[i].A;
		int nowIndexB = m_pFaceList[i].B;
		int nowIndexC = m_pFaceList[i].C;

		m_pNewVertexList[(i * 3)].vPos = m_pVertList[nowIndexA].vPos;
		m_pNewVertexList[(i * 3) + 1].vPos = m_pVertList[nowIndexB].vPos;
		m_pNewVertexList[(i * 3) + 2].vPos = m_pVertList[nowIndexC].vPos;

		/// uv 데이터 넣기
		if (m_pTFaceList != nullptr)
		{
			nowIndexA = m_pTFaceList[i].A;
			nowIndexB = m_pTFaceList[i].B;
			nowIndexC = m_pTFaceList[i].C;
		}

		if (m_pTVertList != nullptr)
		{
			m_pNewVertexList[(i * 3)].vUV = m_pTVertList[nowIndexA].uvPos;
			m_pNewVertexList[(i * 3) + 1].vUV = m_pTVertList[nowIndexB].uvPos;
			m_pNewVertexList[(i * 3) + 2].vUV = m_pTVertList[nowIndexC].uvPos;
		}

	}

	/// 노말 디버깅용 데이터 넣기
	InputNormalDebugData();

	/// 노말 교정
	RepairNormal();

	return CHK_OK;
}


int NodeAse::InputNormalDebugData()
{
	for (UINT i = 0; i < m_NormalCnt; i++)
	{
		m_pNormalLineList[(i * 2)].pos = m_pNewVertexList[i].vPos;
		m_pNormalLineList[(i * 2) + 1].pos = m_pNewVertexList[i].vPos + m_pNewVertexList[i].vNormal;
	}
	return CHK_OK;
}

int NodeAse::RepairNormal()
{
	for (UINT i = 0; i < m_FaceCnt; i++)
	{
		XMVECTOR vNrm = XMLoadFloat3(&m_pNewVertexList[(i * 3)].vNormal);
		VECTOR3  newNrm = VECTOR3(0, 0, 0);
		XMMATRIX newTM = XMLoadFloat4x4(&m_NodeTM);

		XMVECTOR vec = XMVector3TransformNormal(vNrm, newTM);
		XMStoreFloat3(&newNrm, XMVector3Normalize(vec));

		m_pNewVertexList[(i * 3)].vNormal = newNrm;

		//////////////////////////////////////////////////////////////////////////

		vNrm = XMLoadFloat3(&m_pNewVertexList[(i * 3) + 1].vNormal);
		newNrm = VECTOR3(0, 0, 0);
		newTM = XMLoadFloat4x4(&m_NodeTM);

		vec = XMVector3TransformNormal(vNrm, newTM);
		XMStoreFloat3(&newNrm, XMVector3Normalize(vec));

		m_pNewVertexList[(i * 3) + 1].vNormal = newNrm;

		//////////////////////////////////////////////////////////////////////////

		vNrm = XMLoadFloat3(&m_pNewVertexList[(i * 3) + 2].vNormal);
		newNrm = VECTOR3(0, 0, 0);
		newTM = XMLoadFloat4x4(&m_NodeTM);

		vec = XMVector3TransformNormal(vNrm, newTM);
		XMStoreFloat3(&newNrm, XMVector3Normalize(vec));

		m_pNewVertexList[(i * 3) + 2].vNormal = newNrm;
	}

	return CHK_OK;
}

int NodeAse::GetFullVertexCnt()
{
	return m_FaceCnt * 3;
}

void NodeAse::SetTrans(VECTOR3 Pos)
{
	XMMATRIX trans = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	XMStoreFloat4x4(&m_mTrans, trans);
}

void NodeAse::SetRot(QUATERNIONA Quat)
{
	XMMATRIX rot = XMMatrixRotationQuaternion(Quat);
	XMStoreFloat4x4(&m_mRot, rot);
}

void NodeAse::SetScale(VECTOR3 Scale)
{
	XMMATRIX scale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMStoreFloat4x4(&m_mScale, scale);
}