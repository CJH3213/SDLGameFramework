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
	using T = int (*)[2];
	int ii[2] = { 1, 2 };
	T a = (int(*)[2])ii;

	a[1];

	(*mRectTrans).mAnchorMin = { 0.5f, 0.5f };
	(*mRectTrans).mAnchorMax = { 0.9f, 0.9f };
	(*mRectTrans).mSizeDelta = { -80.0f, -40.0f };
	(*mRectTrans).mPivot = { 0.0f, 0.5f };

	auto rect = (*mRectTrans).GetParentRect();
	Debug::DrawRectangle(rect, Color::Orange);

	rect = (*mRectTrans).GetAnchoredRect();
	Debug::DrawRectangle(rect, Color::Blue);

	rect = (*mRectTrans).GetRectangle();
	Debug::DrawRectangle(rect, Color::Green);

	if (mIsDirty == true)
	{
		mIsDirty = false;

		if (!mRenderTex && mNormalTex)
			mRenderTex = mNormalTex;

		if(mRenderTex)
		{
			(*mCollider).mBounds.mCenterPos = rect.mCenterPos;
			(*mCollider).mBounds.mSize = mRenderTex.mSliceRect.mSize;
			(*mRenderer).SetTexture(mRenderTex);
		}
	}
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
