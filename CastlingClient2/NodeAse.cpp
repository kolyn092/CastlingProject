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

	// �ִϸ��̼�
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
	// �ӽ� ���� ����
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

	// ������ ���� ����
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
// �������� �籸��
//
//////////////////////////////////////////////////////////////////////////
int NodeAse::_CreateVFBuffers()
{
	int vn = 0;
	int cn = 0;

	// �������� �������� �ʾ��� ���
	if (m_pCFaceList == NULL)
	{
		// �������� ������, '���' ó��
		for (UINT i = 0; i < m_VertexCnt; i++)
			m_pVertList[i].dwColor = COLOR(1, 1, 1, 1);

		return 0;
	}

	// �������� ���� ���
	// ���� ����Ʈ�� �������� ����Ѵ�.
	for (UINT i = 0; i < m_FaceCnt; i++)
	{
		vn = m_pFaceList[i].A;							// ���̽� �� ���� ��ȣ ���
		cn = m_pCFaceList[i].A;							// ���̽� �� ������ ��ȣ ���	
		m_pVertList[vn].dwColor = m_pCVertList[cn];		// ������ ������ ����

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
// �������� �ϵ���� ���� (VB/IB) �����
//
//////////////////////////////////////////////////////////////////////////
HRESULT NodeAse::_CreateHWBuffers()
{
	HRESULT res = S_OK;

	// ���� ���� �����
	//UINT size = sizeof(VERTEX_ASE) * m_VertexCnt;
	//res = CreateVB(g_pDevice, m_pVertList, size, &m_pVB);

	// �ε��� ���� �����
	//size = sizeof(FACE_ASE) * m_FaceCnt;
	//res = CreateIB(g_pDevice, m_pFaceList, size, &m_pIB);

	// ���� ���� ����� (�븻 ������ ���� ����� ��)
	UINT size = sizeof(NEW_VERTEX) * m_FaceCnt * 3;
	res = D3DEngine::GetIns()->CreateVB(m_pNewVertexList, size, &m_pNewVB);

	// �ε��� ���� ����� (�븻 ������ ���� ����� ��)
	size = sizeof(FACE_ASE) * m_FaceCnt;
	res = D3DEngine::GetIns()->CreateIB(m_pNewFaceList, size, &m_pNewIB);

	// �븻 ������ ���� �����
	size = sizeof(NORMAL_LINE_ASE) * m_NormalCnt * 2;
	res = D3DEngine::GetIns()->CreateVB(m_pNormalLineList, size, &m_pNormalLineVB);

	return res;
}

//////////////////////////////////////////////////////////////////////////
//
// �� ������ �籸�� �Ͽ�, �������� ���۸� �����Ѵ�
// ����/�ؽ�ó ������ �籸�� �� �ε��Ѵ�
//
//////////////////////////////////////////////////////////////////////////
int NodeAse::Generate()
{
	// �������� �籸��
	_CreateVFBuffers();

	// �ϵ���� ���� ����
	HRESULT res = _CreateHWBuffers();
	if (FAILED(res))
	{
		///ynError(res, L"[Error] NodeAse::_CreateHWBuffers() ���� \n"
		///	"Ase File=%s", m_FileName);
	}

	// �ε��� ���۷� �׷��� �� ���� ����
	m_IndexCnt = m_FaceCnt * 3;

	return CHK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// �⺻ �׸��� �ڵ� : �ʿ��ϴٸ� �ܺο��� ó���ϴ� �͵� �����ϴ�
//
//////////////////////////////////////////////////////////////////////////
int NodeAse::Draw(float dTime/*=0.0f*/)
{
	/*
	UINT stride = sizeof(NEW_VERTEX);
	UINT offset = 0;
	g_pDXDC->IASetVertexBuffers(0, 1, &m_pNewVB, &stride, &offset);
	g_pDXDC->IASetIndexBuffer(m_pNewIB, DXGI_FORMAT_R32_UINT, offset);

	// �Է� ���̾ƿ� ����
	g_pDXDC->IASetInputLayout(g_pVBLayout);

	// ���� ���� ���� ����
	g_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���� ����
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

	// �׸���
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

	//// �Է� ���̾ƿ� ����
	//g_pDXDC->IASetInputLayout(g_pVBNormalLineLayout);

	//// ���� ���� ���� ����
	//g_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//g_pDXDC->VSSetShader(g_pVS_Line, nullptr, 0);
	//g_pDXDC->PSSetShader(g_pPS_Line, nullptr, 0);

	//g_pDXDC->Draw(m_NormalCnt * 2, 0);

	return CHK_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// �븻 ������ �������� �ٽ� ������ �Ѵ�.
//
//////////////////////////////////////////////////////////////////////////
int NodeAse::SetNewVertexList()
{
	// ���ο� ���ؽ� ����Ʈ ���� �� �ʱ�ȭ
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
		/// ���̽� ������� �ֱ� 0~35
		m_pNewFaceList[i].A = (i * 3);
		m_pNewFaceList[i].B = (i * 3) + 1;
		m_pNewFaceList[i].C = (i * 3) + 2;

		/// �븻 ������� �ֱ�
		m_pNewVertexList[(i * 3)].vNormal = m_pNormalList[i].vtxNormal[0];
		m_pNewVertexList[(i * 3) + 1].vNormal = m_pNormalList[i].vtxNormal[1];
		m_pNewVertexList[(i * 3) + 2].vNormal = m_pNormalList[i].vtxNormal[2];

		/// ���ؽ� ������ facelist �̿�
		int nowIndexA = m_pFaceList[i].A;
		int nowIndexB = m_pFaceList[i].B;
		int nowIndexC = m_pFaceList[i].C;

		m_pNewVertexList[(i * 3)].vPos = m_pVertList[nowIndexA].vPos;
		m_pNewVertexList[(i * 3) + 1].vPos = m_pVertList[nowIndexB].vPos;
		m_pNewVertexList[(i * 3) + 2].vPos = m_pVertList[nowIndexC].vPos;

		/// uv ������ �ֱ�
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

	/// �븻 ������ ������ �ֱ�
	InputNormalDebugData();

	/// �븻 ����
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