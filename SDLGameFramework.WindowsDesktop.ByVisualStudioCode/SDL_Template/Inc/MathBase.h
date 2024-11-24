#pragma once

#include <math.h>

namespace Math
{
	//������Сֵ
#define EPSILON 0.000001

//�����
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//���������Ƕȵı���
#define RAD_TO_DEG (180.0 / M_PI)

		//�Ƿ�ӽ�0
	extern bool NearZero(float f);
	//����תΪ�Ƕ�
	extern double ToDegrees(double radian);

	//��ȡ���ߴ�ֵ
	template<typename T>
	auto Max(const T& a, const T& b)
	{
		return b > a ? b : a;
	}

	//��ȡ����Сֵ
	template<typename T>
	auto Min(const T& a, const T& b)
	{
		return b < a ? b : a;
	}

	// �޷�Χ�����ֵ��Χֻ����min��max֮�䣩;
	template<typename T>
	T Clamp(const T& v, const T& min, const T& max)
	{
		return v < min ? min : (v > max ? max : v);
	}
}
