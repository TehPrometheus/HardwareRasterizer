#include "pch.h"
#include "Camera.h"




Camera::Camera(float _fovAngle, const Vector3& _origin, float _aspectRatio)
:
	m_Origin{ _origin },
	m_FovAngle{ _fovAngle },
	m_AspectRatio{_aspectRatio}
{
	m_FOV = tanf((m_FovAngle * TO_RADIANS) / 2.f);
}

Camera::~Camera()
{
}

void Camera::CalculateViewMatrix()
{
	Matrix rotationMatrix = Matrix::CreateRotationX(m_TotalPitch * TO_RADIANS) * Matrix::CreateRotationY(m_TotalYaw * TO_RADIANS);

	m_Forward = rotationMatrix.GetAxisZ();
	m_Up = rotationMatrix.GetAxisX();
	m_Right = rotationMatrix.GetAxisY();

	Matrix ONB
	{
		Vector4{ m_Right, 0.f},
		Vector4{ m_Up, 0.f},
		Vector4{m_Forward, 0.f},
		Vector4{ m_Origin ,1.f}
	};

	m_ViewMatrix = ONB;
	m_InvViewMatrix = Matrix::Inverse(m_ViewMatrix);
}

void Camera::CalculateProjectionMatrix()
{
	m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
}

