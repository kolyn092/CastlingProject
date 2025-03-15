#pragma once
#include "DXEngineDefine.h"

class Light : public AlignedAllocationPolicy<16>
{

public:
	XMVECTOR	m_Direction;	// ���� ����
	///XMVECTOR	m_Eye;			// �ü� ����
	XMVECTOR	m_Diffuse;		// �� ���� : Ȯ�걤 Diffuse Light
	XMVECTOR	m_Ambient;		// ���� ���� : �ֺ��� Ambient Light
	XMVECTOR	m_Specular;		// ���ݻ� ����	
	///FLOAT	m_Range;		// �� ���� �Ÿ�
	///FLOAT	m_Theta;		// ���� �ݰ�
	///BOOL		m_bOn;
	BOOL		m_bSpecOn;		// ���ݻ� ���뿩��

public:
	Light();
	Light(XMVECTOR dir, XMFLOAT4A diff, XMFLOAT4A amb, XMFLOAT4A spec, FLOAT range, FLOAT theta);
	~Light();

public:
	void DirectionLightSetting(XMVECTOR pos, FLOAT range, XMFLOAT4A diff, XMFLOAT4A amb, BOOL bOn);
	void PointLightSetting(XMVECTOR pos, FLOAT range, XMFLOAT4A diff, XMFLOAT4A amb, BOOL bOn);

};

