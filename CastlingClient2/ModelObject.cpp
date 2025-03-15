#include "locale.h"		// 로케일 설정
#include "D3DEngine.h"
#include "ModelAse.h"
#include "ModelObject.h"
#include "CGameProcess.h"
#include "NetworkCallback.h"


ModelObject::ModelObject()
{
	m_ObjType = eObjectType::NONE;
	m_pObj = nullptr;

	m_UserTM = XMMatrixIdentity();

	m_vPos = VECTOR3(0, 0, 0);
	m_vRot = VECTOR3(0, 0, 0);
	m_vScale = VECTOR3(1, 1, 1);

	///m_isPicking = FALSE;

	m_AniState = eAnimationState::NONE;
}

ModelObject::~ModelObject()
{

}


//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
BOOL ModelObject::ModelUpdate(float dTime /* = 0 */)
{
	//-------------------------------------------------------------------
	// 변환 행렬 설정
	//-------------------------------------------------------------------
	XMMATRIX mScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	XMMATRIX mRot = XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z);
	XMMATRIX mTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

	m_UserTM = mScale * mRot * mTrans;

	/// 애니메이션
	m_pObj->AnimateAll(dTime);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
BOOL ModelObject::ModelDraw(int materialIndex, bool isTile, bool isPicking)//float dTime /* = 0 */)
{
	D3DEngine* _d3d = D3DEngine::GetIns();
	ID3D11DeviceContext* _dc = _d3d->GetDXDC();

	ID3D11BlendState* _pBlendABON = _d3d->GetBlend(BS_AB_ON);
	ID3D11BlendState* _pBlendABOFF = _d3d->GetBlend(BS_AB_OFF);

	// AB ON
	_dc->OMSetBlendState(_pBlendABON, 0, 0xFFFFFFFF);

	for (unsigned int i = 0; i < m_pObj->m_pNodeList.size(); i++)
	{
		ID3D11DeviceContext* pDXDC = _d3d->GetDXDC();
		ID3D11SamplerState* pSampler = _d3d->GetSampler(SS_CLAMP);

		pDXDC->PSSetSamplers(0, 1, &pSampler);

		int textureIndex = m_pObj->m_pNodeList[i]->GetMaterialRef();

		if (materialIndex != -1)
		{
			if (isTile == true)
			{
				textureIndex = materialIndex;
			}
			else
			{
				textureIndex = textureIndex + (m_pObj->m_MaterialCnt / 2);

				/// ASE에서 텍스처때문에 임의로 만든 오브젝트를 출력할 경우 생기는 문제를 막아두었다.
				if (textureIndex >= m_pObj->m_MaterialCnt)
				{
					textureIndex = 0;
				}
			}
		}

		// 재질 설정
		cbMATERIAL Temp;

		Temp.Diffuse = COLOR(1, 1, 1, 1);
		Temp.Ambient = COLOR(0.5, 0.5, 0.5, 1);
		Temp.Specular = COLOR(1, 1, 1, 1);
		Temp.Power = 30.0f;

		if (isPicking == TRUE)
		{
			/// 피킹할때 표시되는 타일 색 변경
			Temp.Diffuse = COLOR(0, 1, 0, 1);
			Temp.Ambient = COLOR(0, 1, 0, 1);
			Temp.Specular = COLOR(1, 1, 1, 1);
			Temp.Power = 30.0f;
		}

		Shader* defaultShader = _d3d->m_TexLambertShader;
		Shader* nonTexShader = _d3d->m_NonTexShader;

		_d3d->UpdateDynamicConstantBuffer(_d3d->m_pCBMtrl, &Temp, sizeof(cbMATERIAL));
		pDXDC->VSSetConstantBuffers(2, 1, &_d3d->m_pCBMtrl);
		pDXDC->PSSetConstantBuffers(2, 1, &_d3d->m_pCBMtrl);

		if (m_pObj->m_pMaterialList[textureIndex].m_Texture != nullptr && textureIndex != -1)
		{
			pDXDC->VSSetShader(defaultShader->m_pVS, nullptr, 0);
			pDXDC->PSSetShader(defaultShader->m_pPS, nullptr, 0);

			// 입력 레이아웃 설정
			pDXDC->IASetInputLayout(defaultShader->m_pVBLayout);

			pDXDC->PSSetShaderResources(0, 1, &m_pObj->m_pMaterialList[textureIndex].m_Texture);
		}
		else
		{
			pDXDC->VSSetShader(nonTexShader->m_pVS, nullptr, 0);
			pDXDC->PSSetShader(nonTexShader->m_pPS, nullptr, 0);

			// 입력 레이아웃 설정
			pDXDC->IASetInputLayout(nonTexShader->m_pVBLayout);
		}

		UINT stride = sizeof(NEW_VERTEX);
		UINT offset = 0;
		pDXDC->IASetVertexBuffers(0, 1, &m_pObj->m_pNodeList[i]->m_pNewVB, &stride, &offset);
		pDXDC->IASetIndexBuffer(m_pObj->m_pNodeList[i]->m_pNewIB, DXGI_FORMAT_R32_UINT, offset);
		//*/

		// 기하 위상 구조 설정
		pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cbLIGHT* light = &(_d3d->m_cbLit);
		if (m_ObjType == eObjectType::BACKGROUNDROCK)
		{
			light->Ambient = XMLoadFloat4A(&XMFLOAT4A(0.2f, 0.2f, 0.2f, 1.0f));
		}
		else
		{
			light->Ambient = XMLoadFloat4A(&XMFLOAT4A(1, 1, 1, 1.0f));
		}

		// 조명 설정
		_d3d->UpdateDynamicConstantBuffer(_d3d->m_pCBLit, &_d3d->m_cbLit, sizeof(cbLIGHT));
		pDXDC->VSSetConstantBuffers(1, 1, &_d3d->m_pCBLit);
		pDXDC->PSSetConstantBuffers(1, 1, &_d3d->m_pCBLit);

		XMMATRIX mTM = m_pObj->m_pNodeList[i]->GetWorldTM() * m_UserTM;

		///SetTransform(TS_WORLD, mTM);
		_d3d->m_cbDef.mTM = mTM;

		_d3d->m_cbDef.mWV = _d3d->m_cbDef.mTM * _d3d->m_cbDef.mView;
		_d3d->m_cbDef.mWVP = _d3d->m_cbDef.mTM * _d3d->m_cbDef.mView * _d3d->m_cbDef.mProj;

		//셰이더 상수 버퍼 갱신.
		_d3d->UpdateDynamicConstantBuffer(_d3d->m_pCBDef, &_d3d->m_cbDef, sizeof(cbDEFAULT));
		pDXDC->VSSetConstantBuffers(0, 1, &_d3d->m_pCBDef);
		pDXDC->PSSetConstantBuffers(0, 1, &_d3d->m_pCBDef);

		// 그리기
		pDXDC->DrawIndexed(m_pObj->m_pNodeList[i]->m_IndexCnt, 0, 0);
	}

	// 렌더링 옵션 복구.     
	_dc->OMSetBlendState(_pBlendABOFF, 0, 0xFFFFFFFF);

	return TRUE;
}


BOOL ModelObject::CastleModelDraw(int materialIndex, int castleLevel)
{
	if (castleLevel == 0)
	{
		castleLevel += 1;
	}
	else if (castleLevel == 1)
	{
		castleLevel += 2;
	}
	else if (castleLevel == 2)
	{
		castleLevel += 3;
	}

	D3DEngine* _d3d = D3DEngine::GetIns();
	ID3D11DeviceContext* _dc = _d3d->GetDXDC();

	ID3D11BlendState* _pBlendABON = _d3d->GetBlend(BS_AB_ON);
	ID3D11BlendState* _pBlendABOFF = _d3d->GetBlend(BS_AB_OFF);

	// AB ON
	_dc->OMSetBlendState(_pBlendABON, 0, 0xFFFFFFFF);

	for (unsigned int i = 0; i < castleLevel; i++)
	{
		ID3D11DeviceContext* pDXDC = _d3d->GetDXDC();
		ID3D11SamplerState* pSampler = _d3d->GetSampler(SS_CLAMP);

		pDXDC->PSSetSamplers(0, 1, &pSampler);
		///g_pDXDC->PSSetSamplers(0, 1, &g_pSampler[SS_WRAP]);

		int textureIndex = m_pObj->m_pNodeList[i]->GetMaterialRef();

		if (materialIndex != -1)
		{
			textureIndex = textureIndex + (m_pObj->m_MaterialCnt / 2);

			/// ASE에서 텍스처때문에 임의로 만든 오브젝트를 출력할 경우 생기는 문제를 막아두었다.
			if (textureIndex >= m_pObj->m_MaterialCnt)
			{
				textureIndex = 0;
			}

		}

		// 재질 설정
		cbMATERIAL Temp;

		Temp.Diffuse = COLOR(1, 1, 1, 1);
		Temp.Ambient = COLOR(0.5, 0.5, 0.5, 1);
		Temp.Specular = COLOR(1, 1, 1, 1);
		Temp.Power = 30.0f;

		Shader* defaultShader = _d3d->m_TexLambertShader;
		Shader* nonTexShader = _d3d->m_NonTexShader;

		D3DEngine::GetIns()->UpdateDynamicConstantBuffer(_d3d->m_pCBMtrl, &Temp, sizeof(cbMATERIAL));
		pDXDC->VSSetConstantBuffers(2, 1, &_d3d->m_pCBMtrl);
		pDXDC->PSSetConstantBuffers(2, 1, &_d3d->m_pCBMtrl);

		if (m_pObj->m_pMaterialList[textureIndex].m_Texture != nullptr && textureIndex != -1)
		{
			pDXDC->VSSetShader(defaultShader->m_pVS, nullptr, 0);
			pDXDC->PSSetShader(defaultShader->m_pPS, nullptr, 0);

			// 입력 레이아웃 설정
			pDXDC->IASetInputLayout(defaultShader->m_pVBLayout);

			pDXDC->PSSetShaderResources(0, 1, &m_pObj->m_pMaterialList[textureIndex].m_Texture);
		}
		else
		{
			pDXDC->VSSetShader(nonTexShader->m_pVS, nullptr, 0);
			pDXDC->PSSetShader(nonTexShader->m_pPS, nullptr, 0);

			// 입력 레이아웃 설정
			pDXDC->IASetInputLayout(nonTexShader->m_pVBLayout);
		}

		UINT stride = sizeof(NEW_VERTEX);
		UINT offset = 0;
		pDXDC->IASetVertexBuffers(0, 1, &m_pObj->m_pNodeList[i]->m_pNewVB, &stride, &offset);
		pDXDC->IASetIndexBuffer(m_pObj->m_pNodeList[i]->m_pNewIB, DXGI_FORMAT_R32_UINT, offset);
		//*/

		// 기하 위상 구조 설정
		pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 조명 설정
		D3DEngine::GetIns()->UpdateDynamicConstantBuffer(_d3d->m_pCBLit, &_d3d->m_cbLit, sizeof(cbLIGHT));
		pDXDC->VSSetConstantBuffers(1, 1, &_d3d->m_pCBLit);
		pDXDC->PSSetConstantBuffers(1, 1, &_d3d->m_pCBLit);

		XMMATRIX mTM = m_pObj->m_pNodeList[i]->GetWorldTM() * m_UserTM;

		///SetTransform(TS_WORLD, mTM);
		_d3d->m_cbDef.mTM = mTM;

		_d3d->m_cbDef.mWV = _d3d->m_cbDef.mTM * _d3d->m_cbDef.mView;
		_d3d->m_cbDef.mWVP = _d3d->m_cbDef.mTM * _d3d->m_cbDef.mView * _d3d->m_cbDef.mProj;

		//셰이더 상수 버퍼 갱신.
		D3DEngine::GetIns()->UpdateDynamicConstantBuffer(_d3d->m_pCBDef, &_d3d->m_cbDef, sizeof(cbDEFAULT));
		pDXDC->VSSetConstantBuffers(0, 1, &_d3d->m_pCBDef);
		pDXDC->PSSetConstantBuffers(0, 1, &_d3d->m_pCBDef);

		// 그리기
		pDXDC->DrawIndexed(m_pObj->m_pNodeList[i]->m_IndexCnt, 0, 0);
	}

	// 렌더링 옵션 복구.     
	_dc->OMSetBlendState(_pBlendABOFF, 0, 0xFFFFFFFF);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// 디버깅 데이터 Info
//
//////////////////////////////////////////////////////////////////////////
int ModelObject::ModelShowInfo(int scrWidth, int scrHeight, float dTime)
{
	return 0;
}
