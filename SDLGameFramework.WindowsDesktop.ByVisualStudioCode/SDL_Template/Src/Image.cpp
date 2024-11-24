#include "Image.h"
#include "GameObject.h"

void Image::SetNativeSize()
{
	(*mRectTrans).SetSizeDelta(mTexture.mSize - (*mRectTrans).GetAnchoredRect().mSize);
}

void Image::Awake()
{
	mRectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	mRenderer = mGameObject->GetComponent<CanvasRenderer>();
}

void Image::Update()
{
	(*mRenderer).SetTexture(mTexture);
}
