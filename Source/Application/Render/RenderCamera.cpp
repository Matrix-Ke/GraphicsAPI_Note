#include "RenderCamera.h"
#include "Math/CMathInterface.h"

namespace Matrix
{
	void RenderCamera::setCurrentCameraType(RenderCameraType type)
	{
		m_current_camera_type = type;
	}

	void RenderCamera::setMainViewMatrix(const Matrix4x4& view_matrix, RenderCameraType type)
	{
		m_current_camera_type = type;
		m_view_matrices[MAIN_VIEW_MATRIX_INDEX] = view_matrix;

		Vector3 s = Vector3(view_matrix[0][0], view_matrix[0][1], view_matrix[0][2]);
		Vector3 u = Vector3(view_matrix[1][0], view_matrix[1][1], view_matrix[1][2]);
		Vector3 f = Vector3(-view_matrix[2][0], -view_matrix[2][1], -view_matrix[2][2]);
		m_position = s * (-view_matrix[0][3]) + u * (-view_matrix[1][3]) + f * view_matrix[2][3];
	}

	void RenderCamera::move(Vector3 delta) { m_position += delta; }

	void RenderCamera::zoom(float offset)
	{
		// > 0 = zoom in (decrease FOV by <offset> angles)clamp
		m_fovx = Math::Clamp(m_fovx - offset, MIN_FOV, MAX_FOV);
	}

	void RenderCamera::lookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
	{
		//代码计算参考： https://zhuanlan.zhihu.com/p/66384929
		//旋转矩阵是正交矩阵，旋转矩阵的转置便是它的逆(R, U ,D)^T ， 将摄像机坐标系旋转至世界坐标系了。 
		/*
		*			| Right_x,    R_y, R_z, 0 |  | 1 , 0 , 0 , -P_x |
		* lookat =  | Up_x,       U_y, U_z, 0 |  | 0 , 1 , 0 , -P_y |
		*			| Direction_x,D_y, D_z, 0 |	 | 0 , 0 , 1 , -P_z |
		*			| 0 ,		  0, 	0, 1  |	 | 0 , 0 , 0 , 1	|
		*/
		//目前向量都为列向量，所有矩阵都是左乘，最右边的矩阵代表第一步操作移至原点，旁边那个矩阵则代表第二步操作将坐标系重合。
		m_position = eye;

		// model rotation
		// maps vectors to camera space (x, y, z)
		Vector3 z = (target - eye).Normalize();
		Vector3 x = z.Cross(up).Normalize();
		Vector3 y = z.Cross(x).Normalize();

		Math::Matrix4x4  M_Inverse;
		Math::Matrix4x4  M_Translate;
		for (size_t i = 0; i < 3; i++)
		{
			M_Inverse[0][i] = x[i];
			M_Inverse[0][i] = y[i];
			M_Inverse[0][i] = y[i];
			M_Translate[i][3] = -target[i];
		}
		Math::Matrix4x4   view = M_Inverse * M_Translate;
	}

	Matrix4x4 RenderCamera::getViewMatrix()
	{
		Matrix4x4 view_matrix;
		switch (m_current_camera_type)
		{
		case RenderCameraType::Editor:
			view_matrix.CreateFromLookAt(position(), position() + forward(), up());
			break;
		case RenderCameraType::Motor:
			view_matrix = m_view_matrices[MAIN_VIEW_MATRIX_INDEX];
			break;
		default:
			break;
		}
		return view_matrix;
	}

	Matrix4x4 RenderCamera::getPersProjMatrix() const
	{
		Matrix4x4 fix_mat(1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		Matrix4x4 proj_mat;
		proj_mat.CreatePerspective(Math::DegreeToRadian(m_fovy), m_aspect, m_znear, m_zfar);
		//Matrix4x4 proj_mat = fix_mat * Math::makePerspectiveMatrix(Radian(Degree(m_fovy)), m_aspect, m_znear, m_zfar);

		return proj_mat;
	}

	void RenderCamera::setAspect(float aspect)
	{
		m_aspect = aspect;

		// 1 / tan(fovy * 0.5) / aspect = 1 / tan(fovx * 0.5)
		// 1 / tan(fovy * 0.5) = aspect / tan(fovx * 0.5)
		// tan(fovy * 0.5) = tan(fovx * 0.5) / aspect

		//m_fovy = Math::Radian(Math::Atan(Math::tan(Radian(Degree(m_fovx) * 0.5f)) / m_aspect) * 2.0f).valueDegrees();
	}
}