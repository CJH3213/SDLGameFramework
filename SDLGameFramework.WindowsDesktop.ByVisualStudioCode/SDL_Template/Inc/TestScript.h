#pragma once
#include "Component.h"
#include <array>
#include "Vector3.h"

class TestScript :
    public Component
{
private:
    CoRet CoRet_WaitSec;
    CoRet CoFun_WaitSec();

    Vector3 mCamMove;

public:
    virtual void Awake() override;
    virtual void Update() override;
};
