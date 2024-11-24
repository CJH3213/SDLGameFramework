#pragma once
#include <coroutine>
#include <vector>
#include "MyTime.h"

// ʹ��defineҲûʲô��ֵģ����ſ��Կ�iso646.hͷ�ļ�
#define Yield co_await

// Э�̺�������ֵ��Ҳ��ΪЭ��״̬
struct CoRet
{
    struct promise_type;
    using CoHandleType = std::coroutine_handle<CoRet::promise_type>;

    struct promise_type
    {
        std::exception_ptr exception_;
        bool mStopFlag = false;

        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void unhandled_exception() { exception_ = std::current_exception(); }

        CoRet get_return_object()
        {
            return
                CoRet{ CoHandleType::from_promise(*this) };
        }

        template<typename T>
        std::suspend_always yield_value(T yieldValue)
        {
            return{};
        }

        void return_value(int i) {}
        // void return_void() {}    // ����ͬʱ����return_value��return_void
    };

    CoHandleType _h;
    //CoHandleType* ptr = &_h;

    inline void Stop() { _h.promise().mStopFlag = true; }
};

class WaitInstruction
{
    virtual bool await_ready() { return false; }
    virtual int await_resume() { return 0; }
    virtual void await_suspend(CoRet::CoHandleType h) {}
};


struct WaitForSeconds
{
    static std::vector<WaitForSeconds> mCoVec;
    CoRet::CoHandleType mCoHandle;
    double mNextTime;

    WaitForSeconds(double s) :
        mNextTime(s + MyTime::Time_s()) {};

    bool await_ready() { return false; }
    int await_resume() { return 0; }
    void await_suspend(CoRet::CoHandleType h) 
    {
        mCoHandle = h;
        mCoVec.emplace_back(*this);
    }

    static void CheckResume();
};

struct WaitForFixedUpdate
{
    static std::vector<CoRet::CoHandleType> mCoVec;
    static double mFiexedInterval;
    static double mLastFixedTime;

    bool await_ready() { return false; }
    int await_resume() { return 0; }
    void await_suspend(CoRet::CoHandleType h) 
    {
        mCoVec.emplace_back(h);
    }

    static double CheckResume();
};

struct WaitForEndOfFrame
{
    static std::vector<CoRet::CoHandleType> mCoVec;
    static double mFiexedInterval;
    static double mLastFixedTime;

    bool await_ready() { return false; }
    int await_resume() { return 0; }
    void await_suspend(CoRet::CoHandleType h)
    {
        mCoVec.emplace_back(h);
    }

    static double CheckResume();
};
