#pragma once
#include "DataTypes.h"

using namespace dae;

class Camera final
{
public:
	Camera(float _fovAngle, const Vector3& _origin, float _aspectRatio);

	~Camera();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Camera(const Camera& other)					= default;
	Camera(Camera&& other) noexcept				= default;
	Camera& operator=(const Camera& other)		= default;
	Camera& operator=(Camera&& other) noexcept	= default;

	//------------------------------------------------
	// Member functions						
	//------------------------------------------------
	void CalculateViewMatrix();
	void CalculateProjectionMatrix();
	// TODO: start here. Copy paste the update function from the software rasterizer.

private:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------

	float m_TotalPitch{};
	float m_TotalYaw{};
	float m_FOV{};

	const float m_FovAngle{ };
	const float m_AspectRatio{};
	const float m_MovementSpeed{ 20.f };
	const float m_RotationSpeed{ 150.f };
	const float m_NearPlane{ 0.1f };
	const float m_FarPlane{ 100.f };

	Vector3 m_Origin{};
	Vector3 m_Forward{ Vector3::UnitZ };
	Vector3 m_Up{ Vector3::UnitY };
	Vector3 m_Right{ Vector3::UnitX };

	Matrix m_ViewMatrix{};
	Matrix m_InvViewMatrix{};
	Matrix m_ProjectionMatrix{};
};

