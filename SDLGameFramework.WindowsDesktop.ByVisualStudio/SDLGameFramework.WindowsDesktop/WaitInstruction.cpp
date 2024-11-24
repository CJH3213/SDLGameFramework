#include "WaitInstruction.h"
#include <iostream>
#include <SDL.h>

std::vector<WaitForSeconds> WaitForSeconds::mCoVec;

std::vector<CoRet::CoHandleType> WaitForFixedUpdate::mCoVec;
double WaitForFixedUpdate::mFiexedInterval = 0.02;	// �̶����� 50Hz
double WaitForFixedUpdate::mLastFixedTime = 0.0;

std::vector<CoRet::CoHandleType> WaitForEndOfFrame::mCoVec;
double WaitForEndOfFrame::mFiexedInterval = 0.0166;	// ֡���� 60Hz
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

	// ִ������FixedUpdate()�ص�
	for (; nowTime - mLastFixedTime >= mFiexedInterval; )
	{
		mLastFixedTime += mFiexedInterval;

		// ȡ�����й����Э�̾����resumeVec�����mCoVec���ڴ����µĹ���
		// ������ �������� �� ������������ �ֿ���
		
		resumeVec.clear();
		std::swap(mCoVec, resumeVec);
		mCoVec.clear();

		// �����Ѿ������ֹ��Э�̣������������������᲻�����Щ��
		for (auto it = resumeVec.begin(); it != resumeVec.end(); )
		{
			if ((*it).promise().mStopFlag == true)
			{
				(*it).destroy();
				it = resumeVec.erase(it);
			}
			else ++it;
		}

		//std::cout << "�̶�����, ����ʱ�䣺" << nowTime <<
		//	"\t���:" << nowTime - mLastFixedTime << "\n";
		for (auto& coRect : resumeVec)
		{
			coRect.resume();
		}
	}

	// ���ػ�ʣ�����һ�θ���
	return mFiexedInterval - (Time::Time_s() - mLastFixedTime);
}

double WaitForEndOfFrame::CheckResume()
{
	double nowTime = Time::Time_s();

	// ִ������FrameUpdate()�ص�
	if (nowTime - mLastFixedTime >= mFiexedInterval)
	{
		mLastFixedTime = nowTime;

		// ȡ�����й����Э�̾����resumeVec�����mCoVec���ڴ����µĹ���
		// ������ �������� �� ������������ �ֿ���

		decltype(mCoVec) resumeVec;
		std::swap(mCoVec, resumeVec);
		mCoVec.clear();

		// �����Ѿ������ֹ��Э�̣������������������᲻�����Щ��
		for (auto it = resumeVec.begin(); it != resumeVec.end(); )
		{
			if ((*it).promise().mStopFlag == true)
			{
				(*it).destroy();
				it = resumeVec.erase(it);
			}
			else ++it;
		}

		//std::cout << "�̶�����, ����ʱ�䣺" << nowTime <<
		//	"\t���:" << nowTime - mLastFixedTime << "\n";
		for (auto& coRect : resumeVec)
		{
			coRect.resume();
		}
	}

	// ���ػ�ʣ�����һ�θ���
	return mFiexedInterval - (Time::Time_s() - mLastFixedTime);
}
