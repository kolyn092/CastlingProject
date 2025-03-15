#pragma once
#include "DXEngineDefine.h"

class Camera
{

public:
	Matrix mView, mProj;
	enum class eCameraMode
	{
		Default,
		Follow,
	};
	enum class eCameraView
	{
		Perspective,
		Orthographic,
	};

public:
	union {
		struct {
			Vector3 Position;
			Vector3 Rotation;
			Vector3 Scale;
		};
		struct {
			Vector3 Eye;
			Vector3 LookAt;
			Vector3 Up;
			Vector3 Horz;
			Vector3 View;
		};
	};
	
public:
	float FOV;
	float AspectRatio;
	float Near;
	float Far;
	float speed;

	eCameraMode m_mode;

	bool IsAttach;
	Matrix mAttach;

public:
	Camera(UINT width, UINT height, eCameraMode mode = eCameraMode::Follow, eCameraView view = eCameraView::Perspective);
	~Camera();

public:
	void UpdateDefault(float dTime);
	void UpdateFollow(float dTime);
	void Attach(Matrix m);
	void Detach();
};

