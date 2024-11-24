#include "Button.h"
#include "GameObject.h"
#include "Debug.h"

void Button::SetNormalTex(Texture tex)
{
	mNormalTex = tex;
	mIsDirty = true;
}

void Button::OnMouseDown()
{
	mRenderTex = mPressedTex;
	mIsDirty = true;
}

void Button::SetmHighLigTex(Texture tex)
{
	mHighLigTex = tex;
	mIsDirty = true;
}

void Button::SetPressedTex(Texture tex)
{
	mPressedTex = tex;
	mIsDirty = true;
}

void Button::OnClick()
{
	mRenderTex = mHighLigTex;
	mIsDirty = true;
}

void Button::Awake()
{
	mRectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	mRenderer = mGameObject->GetComponent<CanvasRenderer>();
	mCollider = mGameObject->GetComponent<BoxCollider>();

	(*mCollider).mMouseDownEvents.Add("Button", std::bind(&Button::OnMouseDown, this));
	(*mCollider).OnClick.Add("Button", std::bind(&Button::OnClick, this));
	(*mCollider).mMouseEnterEvents.Add("Button", std::bind(&Button::OnMouseEnter, this));
	(*mCollider).mMouseExitEvents.Add("Button", std::bind(&Button::OnMouseExit, this));
}

void Button::Update()
{
	(*mRectTrans).SetAnchorMin({ 0.5f, 0.5f });
	(*mRectTrans).SetAnchorMax({ 0.9f, 0.9f });
	(*mRectTrans).SetSizeDelta({ -80.0f, -40.0f });
	(*mRectTrans).SetPivot({ 0.5f, 0.5f });	// 若不是0.5，碰撞器无法对齐

	if (mIsDirty == true)
	{
		mIsDirty = false;

		if (!mRenderTex && mNormalTex)
			mRenderTex = mNormalTex;

		if(mRenderTex)
			(*mRenderer).SetTexture(mRenderTex);
	}

	auto anRect = (*mRectTrans).GetAnchoredRect();
	(*mRectTrans).SetSizeDelta(mRenderTex.mSize - anRect.mSize);
	(*mRectTrans).SetSizeWithCurrentAnchors(RectTransform::Axis::Horizontal, 50.0f);
	(*mRectTrans).SetSizeWithCurrentAnchors(RectTransform::Axis::Vertical, 80.0f);

	auto rect = (*mRectTrans).GetRectangle();
	(*mCollider).mBounds.mSize = mRenderTex.mSliceRect.mSize;
	//(*mCollider).mBounds = { rect.mCenterPos, mRenderTex.mSliceRect.mSize };
}

void Button::OnMouseEnter()
{
	mRenderTex = mHighLigTex;
	mIsDirty = true;
}

void Button::OnMouseExit()
{
	mRenderTex = mNormalTex;
	mIsDirty = true;
}
