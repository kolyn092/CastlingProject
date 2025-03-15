#include "Light.h"

Light::Light()
{
	XMVECTOR dir = XMVectorSet(1, -3, 2, 0);						//빛 방향.(4성분 주의)
	m_Direction = XMVector3Normalize(-dir);			//빛 방향.(Normalize 및 방향 전환)

	///m_Eye = XMVectorZero();
	//m_Diffuse = XMVectorSet(0.9f, 0.5f, 0.1f, 1);
	m_Diffuse = XMLoadFloat4A(&XMFLOAT4A(1, 1, 1, 1));
	//m_Ambient = XMLoadFloat4A(&XMFLOAT4A(0.2f, 0.2f, 0.2f, 1.0f));
	m_Ambient = XMLoadFloat4A(&XMFLOAT4A(1, 1, 1, 1.0f));
	m_Specular = XMLoadFloat4A(&XMFLOAT4A(1, 1, 1, 1));
	///m_Range = 1000.0f;
	///m_Theta = XMConvertToRadians(30);
	///m_bOn = TRUE;
	m_bSpecOn = TRUE;
}

Light::Light(XMVECTOR dir, XMFLOAT4A diff, XMFLOAT4A amb, XMFLOAT4A spec, FLOAT range, FLOAT theta)
{
	XMVECTOR _dir = dir;						//빛 방향.(4성분 주의)
	m_Direction = XMVector3Normalize(-_dir);	//빛 방향.(Normalize 및 방향 전환)

	///m_Eye = XMVectorZero();
	m_Diffuse = XMLoadFloat4A(&diff);
	m_Ambient = XMLoadFloat4A(&amb);
	m_Specular = XMLoadFloat4A(&spec);
	///m_Range = range;
	///m_Theta = XMConvertToRadians(theta);
	///m_bOn = TRUE;
	m_bSpecOn = TRUE;
}

Light::~Light()
{

}

void Light::DirectionLightSetting(XMVECTOR pos, FLOAT range, XMFLOAT4A diff, XMFLOAT4A amb, BOOL bOn)
{

}

void Light::PointLightSetting(XMVECTOR pos, FLOAT range, XMFLOAT4A diff, XMFLOAT4A amb, BOOL bOn)
{

}

