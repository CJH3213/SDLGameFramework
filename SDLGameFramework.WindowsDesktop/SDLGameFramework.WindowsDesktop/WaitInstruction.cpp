#include "WaitInstruction.h"
#include <iostream>
#include <SDL.h>

std::vector<WaitForSeconds> WaitForSeconds::mCoVec;

std::vector<CoRet::CoHandleType> WaitForFixedUpdate::mCoVec;
double WaitForFixedUpdate::mFiexedInterval = 0.02;	// 固定更新 50Hz
double WaitForFixedUpdate::mLastFixedTime = 0.0;

std::vector<CoRet::CoHandleType> WaitForEndOfFrame::mCoVec;
double WaitForEndOfFrame::mFiexedInterval = 0.0166;	// 帧更新 60Hz
double WaitForEndOfFrame::mLastFixedTime = 0.0;

void WaitForSeconds::CheckResume()
{
	auto& vec = WaitForSeconds::mCoVec;
	std::vector<CoRet::CoHandleType> resumeVec;

	double nowTime = Time::Time_s();
	for (auto it = vec.begin(); it != vec.end();)
	{
		if ((*it).mCoHandle.promise().mStopFlag == true)
		{
			(*it).mCoHandle.destroy();
			it = vec.erase(it);
		}
		else if ((*it).mNextTime <= nowTime)	//
		{
			resumeVec.emplace_back((*it).mCoHandle);
			it = vec.erase(it);
		}
		else it++;
	}

	for (auto& it : resumeVec)
	{
		it.resume();
	}
}

double WaitForFixedUpdate::CheckResume()
{
	double nowTime = Time::Time_s();
	decltype(mCoVec) resumeVec;

	// 执行所有FixedUpdate()回调
	for (; nowTime - mLastFixedTime >= mFiexedInterval; )
	{
		mLastFixedTime += mFiexedInterval;

		// 取出所有挂起的协程句柄到resumeVec，清空mCoVec用于储存新的挂起
		// 这样将 遍历容器 和 储存挂起的容器 分开来
		
		resumeVec.clear();
		std::swap(mCoVec, resumeVec);
		mCoVec.clear();

		// 销毁已经标记终止的协程（遍历整个容器开销会不会大了些）
		for (auto it = resumeVec.begin(); it != resumeVec.end(); )
		{
			if ((*it).promise().mStopFlag == true)
			{
				(*it).destroy();
				it = resumeVec.erase(it);
			}
			else ++it;
		}

		//std::cout << "固定更新, 现在时间：" << nowTime <<
		//	"\t间隔:" << nowTime - mLastFixedTime << "\n";
		for (auto& coRect : resumeVec)
		{
			coRect.resume();
		}
	}

	// 返回还剩多久下一次更新
	return mFiexedInterval - (Time::Time_s() - mLastFixedTime);
}

double WaitForEndOfFrame::CheckResume()
{
	double nowTime = Time::Time_s();

	// 执行所有FrameUpdate()回调
	if (nowTime - mLastFixedTime >= mFiexedInterval)
	{
		mLastFixedTime = nowTime;

		// 取出所有挂起的协程句柄到resumeVec，清空mCoVec用于储存新的挂起
		// 这样将 遍历容器 和 储存挂起的容器 分开来

		decltype(mCoVec) resumeVec;
		std::swap(mCoVec, resumeVec);
		mCoVec.clear();

		// 销毁已经标记终止的协程（遍历整个容器开销会不会大了些）
		for (auto it = resumeVec.begin(); it != resumeVec.end(); )
		{
			if ((*it).promise().mStopFlag == true)
			{
				(*it).destroy();
				it = resumeVec.erase(it);
			}
			else ++it;
		}

		//std::cout << "固定更新, 现在时间：" << nowTime <<
		//	"\t间隔:" << nowTime - mLastFixedTime << "\n";
		for (auto& coRect : resumeVec)
		{
			coRect.resume();
		}
	}

	// 返回还剩多久下一次更新
	return mFiexedInterval - (Time::Time_s() - mLastFixedTime);
}
