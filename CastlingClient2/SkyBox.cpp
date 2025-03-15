#include "SkyBox.h"
#include "D3DEngine.h"

SkyBox::SkyBox()
{
	m_Shader = new Shader();

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,  0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_Shader->ShaderLoad(L"./fx/SkyBox.fx");
	m_Shader->CreateVBLayout(layout, ARRAYSIZE(layout));
	D3DEngine::GetIns()->LoadTexture(L"../Data/SkyBox/SkyBox3_1.dds", &m_TextureRV);

	VTX_SkyBox MeshSkyBox[] = {
		XMFLOAT2(-1.0f,	 1.0f),
		XMFLOAT2(1.0f,  1.0f),
		XMFLOAT2(-1.0f, -1.0f),
		XMFLOAT2(1.0f, -1.0f)
	};

	// ��������Ʈ (����) ���� ���� ���� : �ʱ� 1���� ����
	if (m_pSkyBoxVB == nullptr)
	{
		D3DEngine::GetIns()->CreateVB(MeshSkyBox, sizeof(MeshSkyBox), &m_pSkyBoxVB);
		D3DEngine::GetIns()->CreateDynamicConstantBuffer(sizeof(cbDEFAULT_SkyBox), &cbDef_SkyBox, &m_pCB_SkyBox);
		D3DEngine::GetIns()->CreateDynamicConstantBuffer(sizeof(cbExtraData_SkyBox), &cbExt_SkyBox, &m_pExtCB_SkyBox);
	}
}

SkyBox::~SkyBox()
{
	SAFE_RELEASE(m_pSkyBoxVB);
	SAFE_RELEASE(m_TextureRV);
	SAFE_RELEASE(m_pCB_SkyBox);
	SAFE_RELEASE(m_pExtCB_SkyBox);

	SAFE_DELETE(m_Shader);
}

void SkyBox::Update()
{

}

void SkyBox::Draw()
{
	D3DEngine* _d3d = D3DEngine::GetIns();
	Camera* cam = _d3d->m_Camera;

	cbDef_SkyBox.mViewInv = XMMatrixInverse(NULL, cam->mView);
	cbDef_SkyBox.mProjInv = XMMatrixInverse(NULL, cam->mProj);

	//cbExt_SkyBox.diffuse = XMVectorSet(0.9f, 0.5f, 0.1f, 0.4f);
	//cbExt_SkyBox.diffuse = XMVectorSet(0.3f, 0.3f, 0.3f, 0.4f);
	cbExt_SkyBox.diffuse = XMVectorSet(1, 1, 1, 1);
	cbExt_SkyBox.z_far = cam->Far;

	_d3d->UpdateDynamicConstantBuffer(m_pCB_SkyBox, &cbDef_SkyBox, sizeof(cbDEFAULT_SkyBox));
	_d3d->UpdateDynamicConstantBuffer(m_pExtCB_SkyBox, &cbExt_SkyBox, sizeof(cbExtraData_SkyBox));

	// ��������Ʈ �޽� ����. (����)
	UINT stride = sizeof(VTX_SkyBox);
	UINT offset = 0;

	ID3D11DeviceContext* pDXDC = _d3d->GetDXDC();

	pDXDC->IASetVertexBuffers(0, 1, &m_pSkyBoxVB, &stride, &offset);
	pDXDC->IASetInputLayout(m_Shader->m_pVBLayout);
	pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* pSampler = _d3d->GetSampler(SS_CLAMP);
	pDXDC->PSSetSamplers(0, 1, &pSampler);

	//���̴� ����. : ǥ�� ���� ���̴� ���. 
	pDXDC->VSSetShader(m_Shader->m_pVS, nullptr, 0);
	pDXDC->PSSetShader(m_Shader->m_pPS, nullptr, 0);

	//������� ���� 
	pDXDC->VSSetConstantBuffers(0, 1, &m_pCB_SkyBox);			//��� ���� ����.(VS)
	pDXDC->PSSetConstantBuffers(1, 1, &m_pExtCB_SkyBox);			//��� ���� ����.(VS)
	//���̴� ���ҽ� ����.
	pDXDC->PSSetShaderResources(0, 1, &m_TextureRV);			//PS �� �ؽ�ó ����

	// �׸���         
	pDXDC->Draw(4, 0);
}
