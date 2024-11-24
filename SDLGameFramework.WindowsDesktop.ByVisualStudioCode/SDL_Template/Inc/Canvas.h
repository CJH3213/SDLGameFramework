#pragma once
#include "Component.h"
#include "RectTransform.h"

class Canvas :
    public Component
{
private:
    ComponentPtr<RectTransform> mRectTrans;

    void SyncWithCamera();

public:
    virtual void Awake() override;
    virtual void Update() override;
};

