#pragma once
#include "Component.h"
#include "RenderInterface.h"
#include "RectTransform.h"

class CanvasRenderer :
    public Component
{
private:
    struct CanvasRenIF : RenderInterface
    {
        CanvasRenderer* mTexRenCom = nullptr;
        CanvasRenIF(CanvasRenderer* owner) :mTexRenCom(owner) {};
        virtual int GetOrder() override { return mTexRenCom->mOrder; }
        virtual void Draw(Camera& cam) override;
    };

    int mOrder = 99;
    Texture mTexture;
    std::shared_ptr<RenderInterface> mRenderIF;

    ComponentPtr<RectTransform> mRectTrans;

public:

    virtual void Awake() override;
    virtual void Update() override;

    void SetOrder(int order);
    void SetTexture(Texture tex) { mTexture = tex; }
};

