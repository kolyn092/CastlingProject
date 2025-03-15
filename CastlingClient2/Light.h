#pragma once
#include "DXEngineDefine.h"

class Light : public AlignedAllocationPolicy<16>
{

public:
	XMVECTOR	m_Direction;	// 빛의 방향
	///XMVECTOR	m_Eye;			// 시선 방향
	XMVECTOR	m_Diffuse;		// 주 광량 : 확산광 Diffuse Light
	XMVECTOR	m_Ambient;		// 보조 광량 : 주변광 Ambient Light
	XMVECTOR	m_Specular;		// 정반사 광량	
	///FLOAT	m_Range;		// 빛 도달 거리
	///FLOAT	m_Theta;		// 빛의 반경
	///BOOL		m_bOn;
	BOOL		m_bSpecOn;		// 정반사 적용여부

public:
	Light();
	Light(XMVECTOR dir, XMFLOAT4A diff, XMFLOAT4A amb, XMFLOAT4A spec, FLOAT range, FLOAT theta);
	~Light();

public:
	void DirectionLightSetting(XMVECTOR pos, FLOAT range, XMFLOAT4A diff, XMFLOAT4A amb, BOOL bOn);
	void PointLightSetting(XMVECTOR pos, FLOAT range, XMFLOAT4A diff, XMFLOAT4A amb, BOOL bOn);

};

