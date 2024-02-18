#pragma once
#include "Component.h"
#include "RectTransform.h"

class Canvas :
    public Component
{
private:
    ComponentPtr<RectTransform> mRectTrans;

public:
    virtual void Awake() override;
    virtual void Update() override;
};

