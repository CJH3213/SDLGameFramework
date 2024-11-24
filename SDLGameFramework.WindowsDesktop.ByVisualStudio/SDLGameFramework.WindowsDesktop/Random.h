#pragma once
#include <random>
#include <type_traits>

class Random
{
public:
	/// <summary>
	/// 随机数生成器（单例类）
	/// 封装C++11的random库的一种产生随机数方法
	/// random_device + mt19937 + uniform_int_distribution
	/// </summary>
	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
	static T Range(T min, T max);

};

template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int>>
inline T Random::Range(T min, T max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	if constexpr (std::is_integral_v<T>)
	{
		std::uniform_int_distribution<T> dis(min, max);
		return dis(gen);
	}
	else
	{
		std::uniform_real_distribution<T> dis(min, max);
		return dis(gen);
	}
}
