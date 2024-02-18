#pragma once
#include <chrono>

class Time
{

private:
	using SysClock = std::chrono::system_clock;
	static SysClock::time_point mBeginTimePoint;

public:
	//Time()
	//{
	//	mBeginTimePoint = SysClock::now();
	//}

	static inline void Init()
	{
		mBeginTimePoint = SysClock::now();
	}

	static inline double Time_s()
	{
		auto timePoint = SysClock::now();
		auto timeStamp = timePoint.time_since_epoch().count();
		std::chrono::duration<double> diff = timePoint - mBeginTimePoint;
		return diff.count();
	}
};

