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
    bool mPreserveAspect = false;   // ����ͼ���߱�

public:
    void SetTexture(Texture tex) { mTexture = tex; };
    void SetPreserveAspect(bool b) { mPreserveAspect = b; }
    void SetNativeSize();   // ��ͼ���С���õ�RectTransform��UI���δ�С��

    virtual void Awake() override;
    virtual void Update() override;
};

