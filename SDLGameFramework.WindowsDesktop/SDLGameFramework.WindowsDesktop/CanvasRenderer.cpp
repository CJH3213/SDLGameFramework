#include "CanvasRenderer.h"
#include "RenderManager.h"
#include "WindowsManager.h"
#include "Transform.h"
#include "GameObject.h"

void CanvasRenderer::Awake()
{
	mRectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);

	mRenderIF = std::make_shared<CanvasRenIF>(this);
	RenderManager::AddRenderer(mRenderIF);
}

void CanvasRenderer::SetOrder(int order)
{
	if (order == mOrder)
		return;

	mOrder = order;
	RenderManager::ChangeOrder(*mRenderIF, mOrder);
}

void CanvasRenderer::CanvasRenIF::Draw(Camera& cam)
{
	auto& tex = mTexRenCom->mTexture;
	auto& trans = *mTexRenCom->mRectTrans;
	auto& win = WindowsManager::Get(0);

	win.DrawTexture(tex, Rectangle::Zero,
		{ cam.WorldToWinPoint(trans.GetRectangle().mCenterPos),
		tex.mSliceRect.mSize * trans.GetScale() },
		360.0 - fmod(trans.GetRotation(), 360.0));	// 旋转方向逆时针变顺时针
}
