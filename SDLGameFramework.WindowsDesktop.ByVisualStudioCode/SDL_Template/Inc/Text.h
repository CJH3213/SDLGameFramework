#pragma once
#include "Component.h"
#include "TextureRenderer.h"
#include "FontManager.h"
#include "RectTransform.h"
#include "CanvasRenderer.h"

class Text :
    public Component
{
private:
    ComponentPtr<RectTransform> mRectTrans;
    ComponentPtr<CanvasRenderer> mRenderer;
    //ComponentPtr<TextureRenderer> mTextureRenCom;

    bool mNeedToChange = true;
    FontStyle mFontStyle;
    std::u8string mContent;

    void ChangeTexture();

public:
    virtual void Awake() override;
    virtual void Update() override;

    void SetFontAsset(std::string path);
    void SetFontGlyph(FontGlyph glyph);
    void SetFontSize(size_t size);
    void SetFontColor(Color color);
    void SetContent(std::u8string text);
};

