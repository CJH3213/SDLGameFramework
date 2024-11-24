#pragma once

#include <math.h>

namespace Math
{
	//近似最小值
#define EPSILON 0.000001

//定义π
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//算出弧度与角度的比率
#define RAD_TO_DEG (180.0 / M_PI)

		//是否接近0
	extern bool NearZero(float f);
	//弧度转为角度
	extern double ToDegrees(double radian);

	//获取两者大值
	template<typename T>
	auto Max(const T& a, const T& b)
	{
		return b > a ? b : a;
	}

	//获取两者小值
	template<typename T>
	auto Min(const T& a, const T& b)
	{
		return b < a ? b : a;
	}

	// 限范围（输出值范围只会在min和max之间）;
	template<typename T>
	T Clamp(const T& v, const T& min, const T& max)
	{
		return v < min ? min : (v > max ? max : v);
	}
}
