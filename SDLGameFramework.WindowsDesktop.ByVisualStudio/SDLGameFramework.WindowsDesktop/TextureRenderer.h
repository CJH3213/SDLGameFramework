#pragma once
#include "Component.h"
#include "RenderInterface.h"
#include <memory>
#include "Texture.h"

class TextureRenderer :
    public Component
{
private:
    struct TexRenIF : RenderInterface
    {
        TextureRenderer* mTexRenCom = nullptr;
        TexRenIF(TextureRenderer* owner) :mTexRenCom(owner) {};
        virtual int GetOrder() override;
        virtual void Draw(Camera& cam) override;
    };

    int mOrder = 99;
    Texture mTexture;
    std::shared_ptr<RenderInterface> mRenderIF;

public:
    virtual void Awake() override;

    void SetOrder(int order);
    void SetTexture(Texture tex) { mTexture = tex; }
};

