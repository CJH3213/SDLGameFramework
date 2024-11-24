#include "Text.h"
#include "FontManager.h"
#include "GameObject.h"

void Text::ChangeTexture()
{
	auto tex = FontManager::TextToTexture(mContent, mFontStyle);
	(*mRenderer).SetTexture(tex);
}

void Text::Awake()
{
	mRectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	mRenderer = mGameObject->GetComponent<CanvasRenderer>();
}

void Text::Update()
{
	if (mNeedToChange)
	{
		mNeedToChange = false;
		ChangeTexture();
	}

	auto rect0 = (*mRectTrans).GetParentRect();
	auto rect1 = (*mRectTrans).GetAnchoredRect();
	auto rect2 = (*mRectTrans).GetRectangle();
}

void Text::SetFontAsset(std::string path)
{
	mFontStyle.fontPath = path;
	mNeedToChange = true;
}

void Text::SetFontGlyph(FontGlyph glyph)
{
	mFontStyle.glyph = glyph;
	mNeedToChange = true;
}

void Text::SetFontSize(size_t size)
{
	mFontStyle.size = size;
	mNeedToChange = true;
}

void Text::SetFontColor(Color color)
{
	mFontStyle.color = color;
	mNeedToChange = true;
}

void Text::SetContent(std::u8string text)
{
	mContent = text;
	mNeedToChange = true;
}
