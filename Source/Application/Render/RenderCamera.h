#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

#include <vector>


namespace Matrix
{
	using namespace Math;
	enum class RenderCameraType : int
	{
		Editor,
		Motor
	};
	/*
	* 相机就是虚拟的照相机和摄像机，不但负责构建 3D 流水线中的各种变换矩阵，
	还负责裁剪掉相机不可见的物体来加快渲染速度
	*/
	class RenderCamera
	{
	public:
		RenderCameraType m_current_camera_type{ RenderCameraType::Editor };

		static const Vector3 X, Y, Z;

		Vector3    m_position{ 0.0f, 0.0f, 0.0f };
		Quaternion m_rotation{ 1, 0, 0, 0 };
		Quaternion m_invRotation{ 1, 0, 0, 0 };
		float      m_znear{ 1000.0f };
		float      m_zfar{ 0.1f };
		Vector3    m_up_axis{ Z };

		static constexpr float MIN_FOV{ 10.0f };
		static constexpr float MAX_FOV{ 89.0f };
		static constexpr int   MAIN_VIEW_MATRIX_INDEX{ 0 };

		std::vector<Matrix4x4> m_view_matrices{};
		Math::Matrix4x4 m_ViewMat; //相机矩阵 = 相机世界矩阵的逆矩阵
		Math::Matrix4x4 m_ProjMat; //投影矩阵

		void setCurrentCameraType(RenderCameraType type);
		void setMainViewMatrix(const Matrix4x4& view_matrix, RenderCameraType type = RenderCameraType::Editor);

		void move(Vector3 delta);
		void zoom(float offset); //set fov
		void lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);

		void setAspect(float aspect);
		void setFOVx(float fovx) { m_fovx = fovx; }

		Vector3    position() const { return m_position; }
		Quaternion rotation() const { return m_rotation; }

		Vector3   forward() const { return (m_invRotation * Y); }
		Vector3   up() const { return (m_invRotation * Z); }
		Vector3   right() const { return (m_invRotation * X); }
		Vector2   getFOV() const { return { m_fovx, m_fovy }; }
		Matrix4x4 getViewMatrix();
		Matrix4x4 getPersProjMatrix() const;
		Matrix4x4 getLookAtMatrix() const
		{
			Matrix4x4 LookatMat{};
			LookatMat.CreateFromLookAt(position(), position() + forward(), up());
			return LookatMat;
		}
		float     getFovYDeprecated() const { return m_fovy; }

	protected:
		float m_aspect{ 0.f };
		float m_fovx{ Degree(89.f).valueDegrees() };
		float m_fovy{ 0.f };

	};

	inline const Vector3 RenderCamera::X = { 1.0f, 0.0f, 0.0f };
	inline const Vector3 RenderCamera::Y = { 0.0f, 1.0f, 0.0f };
	inline const Vector3 RenderCamera::Z = { 0.0f, 0.0f, 1.0f };

} // namespace Piccolo
