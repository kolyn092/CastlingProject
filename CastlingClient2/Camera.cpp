#include "Camera.h"

Camera::Camera(UINT width, UINT height, eCameraMode mode /*= eCameraMode::Default*/, eCameraView view /*= eCameraView::Perspective*/)
	: mAttach(Matrix::Identity), AspectRatio(0.0f), Far(0.0f), FOV(0.0f), IsAttach(false), Near(0.0f)
{
	Matrix mLocal;
	switch (mode)
	{
		case Camera::eCameraMode::Default:
			Position = Vector3(0, 2, -10);
			Rotation = Vector3::Zero;
			Scale = Vector3::One;

			mLocal = Matrix::CreateScale(Scale) * XMMatrixRotationRollPitchYawFromVector(Rotation) * Matrix::CreateTranslation(Position);

			mView = mLocal.Invert();
			break;
		case Camera::eCameraMode::Follow:
		{
			Eye = Vector3(0, 8, 8);
			LookAt = Vector3(0, 0, -1.5f);///Vector3::Zero;
			Up = Vector3::Up;

			mView = XMMatrixLookAtLH(Eye, LookAt, Up);
		}
		break;
	}

	switch (view)
	{
		case Camera::eCameraView::Perspective:
			FOV = 60;
			AspectRatio = (float)width / height;
			Near = 1;
			Far = 500;
			mProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), AspectRatio, Near, Far);
			break;
		case Camera::eCameraView::Orthographic:
			mProj = XMMatrixOrthographicLH((float)width, (float)height, Near, Far);
			break;
	}

	m_mode = mode;
	speed = 3.0f;
}

Camera::~Camera()
{

}

// 자유시점
void Camera::UpdateDefault(float dTime)
{
	//move
	if (IsKeyDown('W'))
	{
		Position += Vector3::TransformNormal(Vector3::UnitZ, XMMatrixRotationRollPitchYawFromVector(Rotation)) * dTime * speed;
	}
	if (IsKeyDown('S'))
	{
		Position -= Vector3::TransformNormal(Vector3::UnitZ, XMMatrixRotationRollPitchYawFromVector(Rotation)) * dTime * speed;
	}
	if (IsKeyDown('A'))
	{
		Position += Vector3::TransformNormal(Vector3::Left, XMMatrixRotationRollPitchYawFromVector(Rotation)) * dTime * speed;
	}
	if (IsKeyDown('D'))
	{
		Position += Vector3::TransformNormal(Vector3::Right, XMMatrixRotationRollPitchYawFromVector(Rotation)) * dTime * speed;
	}
	if (IsKeyDown('Q'))
	{
		Position += Vector3::TransformNormal(Vector3::UnitY, XMMatrixRotationRollPitchYawFromVector(Rotation)) * dTime * speed;
	}
	if (IsKeyDown('E'))
	{
		Position -= Vector3::TransformNormal(Vector3::UnitY, XMMatrixRotationRollPitchYawFromVector(Rotation)) * dTime * speed;
	}
	if (IsKeyDown(VK_OEM_PLUS))
	{
		speed += 0.1f;
	}
	if (IsKeyDown(VK_OEM_MINUS))
	{
		speed -= 0.1f;
	}

	//rotation
	if (IsKeyDown(VK_HOME))
	{
		Rotation.x -= XM_PIDIV4 * dTime;
	}
	if (IsKeyDown(VK_END))
	{
		Rotation.x += XM_PIDIV4 * dTime;
	}
	if (IsKeyDown(VK_DELETE))
	{
		Rotation.y -= XM_PIDIV4 * dTime;
	}
	if (IsKeyDown(VK_NEXT))
	{
		Rotation.y += XM_PIDIV4 * dTime;
	}

	//zoom
	if (IsKeyDown(VK_INSERT))
	{
		FOV -= 1 * dTime;
	}
	if (IsKeyDown(VK_PRIOR))
	{
		FOV += 1 * dTime;
	}
	Matrix mLocal = Matrix::CreateScale(Scale) * XMMatrixRotationRollPitchYawFromVector(Rotation) * Matrix::CreateTranslation(Position);

	mView = mLocal.Invert();
	mProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), AspectRatio, Near, Far);
}

void Camera::UpdateFollow(float dTime)
{
	View = LookAt - Eye;
	View = XMVector3Normalize(View);
	Horz = XMVector3Cross(Up, View);
	Horz = XMVector3Normalize(Horz);

	// 카메라 회전
	static float mov = 25.0f;
	static const float rot = XM_PI / 4.0f;
	Vector3 camPos = Vector3::Zero;
	Vector3 camRot(0, 0, 0);

	if (IsKeyDown('W'))	camRot.x += rot * dTime;
	if (IsKeyDown('S'))	camRot.x -= rot * dTime;
	if (IsKeyDown('E'))	FOV -= mov * dTime;
	if (IsKeyDown('Q'))	FOV += mov * dTime;

	Eye = camPos + Eye;

	Matrix mRotX, mRotY, mRotZ;
	mRotX = XMMatrixRotationAxis(Horz, camRot.x);
	mRotY = XMMatrixRotationAxis(Up, camRot.y);
	Matrix mRot = mRotX * mRotY;

	//최종 카메라 시점 계산
	Vector3 Eye1 = XMVector3TransformCoord(Eye, mRot);

	// 시야 각도 제어
	if (Eye1.y > 2.0f && Eye1.y < 9.5f)
		Eye = Eye1;

	if (FOV < 65)
		FOV = 65;

	if (FOV > 85)
		FOV = 85;

	mView = XMMatrixLookAtLH(Eye, LookAt, Up);
	mProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), AspectRatio, Near, Far);
}

void Camera::Attach(Matrix m)
{
	mAttach = m;
}

void Camera::Detach()
{
	mAttach = Matrix::Identity;
}
