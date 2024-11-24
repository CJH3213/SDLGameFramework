#pragma once
#include "Component.h"
#include "RectTransform.h"
#include "CanvasRenderer.h"

class Image :
    public Component
{
private:
    ComponentPtr<RectTransform> mRectTrans;
    ComponentPtr<CanvasRenderer> mRenderer;

    Texture mTexture;
    bool mPreserveAspect = false;   // 保持图像宽高比

public:
    void SetTexture(Texture tex) { mTexture = tex; };
    void SetPreserveAspect(bool b) { mPreserveAspect = b; }
    void SetNativeSize();   // 将图像大小设置到RectTransform的UI矩形大小上

    virtual void Awake() override;
    virtual void Update() override;
};

