#pragma once
#include "Vector3.h"
#include <initializer_list>

struct Matrix4x1
{
	float Mat[4][1] = { 0 };

	Matrix4x1() = default;
	Matrix4x1(const Vector3& v3)
	{
		Mat[0][0] = v3.X;
		Mat[1][0] = v3.Y;
		Mat[2][0] = v3.Z;
		Mat[3][0] = 1.0f;
	}

	inline operator Vector3() const
	{
		return { Mat[0][0], Mat[1][0], Mat[2][0] };
	}

	inline Matrix4x1& NormalizationThis()
	{
		Mat[0][0] /= Mat[3][0];
		Mat[1][0] /= Mat[3][0];
		Mat[2][0] /= Mat[3][0];
		Mat[3][0] = 0.0f;
		return (*this);
	}

	inline Matrix4x1 Normalization() const
	{
		Matrix4x1 M(*this);
		return M.NormalizationThis();
	}
};

struct Matrix4x4
{
	float Mat[4][4] = { 0 };

	Matrix4x4() = default;
	Matrix4x4(std::initializer_list<float> list);

	inline Matrix4x4& operator*=(const float f)
	{
		float* p = &Mat[0][0];

		for (int i = 0; i < 4 * 4; i++, p++)
			*p = *p * f;

		return (*this);
	}

	inline Matrix4x4 operator*(const float f) const
	{
		Matrix4x4 M(*this);
		float* p = &M.Mat[0][0];

		for (int i = 0; i < 4 * 4; i++, p++)
			*p = *p * f;

		return M;
	}

	inline Matrix4x4& operator*=(const Matrix4x4& RM)
	{
		for (size_t ya = 0; ya < 4; ya++)
			for (size_t xb = 0; xb < 4; xb++)
				for (size_t yb = 0; yb < 4; yb++)
					Mat[ya][xb] += Mat[ya][yb] * RM.Mat[yb][xb];

		return (*this);
	}

	inline Matrix4x4 operator*(const Matrix4x4& RM) const
	{
		Matrix4x4 M;

		for (size_t ya = 0; ya < 4; ya++)
			for (size_t xb = 0; xb < 4; xb++)
				for (size_t yb = 0; yb < 4; yb++)
					M.Mat[ya][xb] += Mat[ya][yb] * RM.Mat[yb][xb];

		return M;
	}

	inline Matrix4x1 operator*(const Matrix4x1& RM) const
	{
		Matrix4x1 M;

		for (size_t ya = 0; ya < 4; ya++)
			for (size_t yb = 0; yb < 4; yb++)
				M.Mat[ya][0] += Mat[ya][yb] * RM.Mat[yb][0];

		return M;
	}

	// 构造单位矩阵，对角线为1，其余为0
	inline static Matrix4x4 Eye()
	{
		Matrix4x4 M;
		M.Mat[0][0] = 1.0f;
		M.Mat[1][1] = 1.0f;
		M.Mat[2][2] = 1.0f;
		M.Mat[3][3] = 1.0f;
		return M;
	}

	// 构造全1矩阵
	inline static Matrix4x4 Ones()
	{
		return {
			1, 1, 1, 1,
			1, 1, 1, 1,
			1, 1, 1, 1,
			1, 1, 1, 1 };
	}

	// 构造全0矩阵
	inline static Matrix4x4 Zeros()
	{
		return {
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0 };
	}

	// 构造移动矩阵
	inline static Matrix4x4 Translate(const Vector3& v3)
	{
		Matrix4x4 M;
		M.Mat[0][3] = v3.X;
		M.Mat[1][3] = v3.Y;
		M.Mat[2][3] = v3.Z;
		M.Mat[0][0] = 1.0f;
		M.Mat[1][1] = 1.0f;
		M.Mat[2][2] = 1.0f;
		M.Mat[3][3] = 1.0f;
		return M;
	}

	// 构造缩放矩阵
	inline static Matrix4x4 Scale(const Vector3& v3)
	{
		Matrix4x4 M;
		M.Mat[0][0] = v3.X;
		M.Mat[1][1] = v3.Y;
		M.Mat[2][2] = v3.Z;
		M.Mat[3][3] = 1.0f;
		return M;
	}

	// 构造旋转矩阵，绕X轴
	inline static Matrix4x4 RotateX(float a)
	{
		Matrix4x4 M;
		M.Mat[1][1] = cosf(a);
		M.Mat[1][2] = -sinf(a);
		M.Mat[2][1] = sinf(a);
		M.Mat[2][2] = cosf(a);
		M.Mat[0][0] = 1.0f;
		M.Mat[3][3] = 1.0f;
		return M;
	}

	// 构造旋转矩阵，绕Y轴
	inline static Matrix4x4 RotateY(float a)
	{
		// 注意：Y轴旋转矩阵sin的正负和其他旋转矩阵不同
		Matrix4x4 M;
		M.Mat[0][0] = cosf(a);
		M.Mat[0][2] = sinf(a);
		M.Mat[2][0] = -sinf(a);
		M.Mat[2][2] = cosf(a);
		M.Mat[1][1] = 1.0f;
		M.Mat[3][3] = 1.0f;
		return M;
	}

	// 构造旋转矩阵，绕Z轴
	inline static Matrix4x4 RotateZ(float a)
	{
		Matrix4x4 M;
		M.Mat[0][0] = cosf(a);
		M.Mat[0][1] = -sinf(a);
		M.Mat[1][0] = sinf(a);
		M.Mat[1][1] = cosf(a);
		M.Mat[2][2] = 1.0f;
		M.Mat[3][3] = 1.0f;
		return M;
	}

	// 构造视口矩阵，将摄像机旋转到原点
	static Matrix4x4 ViewMatrix(Vector3 eye, Vector3 at);

	// 构造透视矩阵，焦点
	inline static Matrix4x4 Perspective(float n)
	{
		Matrix4x4 M;
		M.Mat[0][0] = n;
		M.Mat[1][1] = n;
		M.Mat[2][2] = n;	// Z轴不用计算的
		M.Mat[3][2] = -1.0f;
		M.Mat[3][3] = 1.0f;
		return M;
	}
};

