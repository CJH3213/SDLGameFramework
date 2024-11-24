#pragma once
#include <coroutine>
#include <vector>
#include "MyTime.h"

// 使用define也没什么奇怪的，不信可以看iso646.h头文件
#define Yield co_await

// 协程函数返回值，也称为协程状态
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
        // void return_void() {}    // 不能同时设置return_value和return_void
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
