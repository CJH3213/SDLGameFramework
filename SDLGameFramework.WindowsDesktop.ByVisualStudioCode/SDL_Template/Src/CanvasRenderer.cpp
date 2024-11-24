#include "CanvasRenderer.h"
#include "RenderManager.h"
#include "WindowsManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Debug.h"

void CanvasRenderer::Awake()
{
	mRectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);

	mRenderIF = std::make_shared<CanvasRenIF>(this);
	RenderManager::AddRenderer(mRenderIF);
}

void CanvasRenderer::Update()
{
	auto rect = (*mRectTrans).GetParentRect();
	Debug::DrawRectangle(rect, Color::Pink);

	rect = (*mRectTrans).GetAnchoredRect();
	Debug::DrawRectangle(rect, Color::Blue);

	rect = (*mRectTrans).GetRectangle();
	Debug::DrawRectangle(rect, Color::Green);
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
	auto rect = trans.GetRectangle();

	win.DrawTexture(tex, Rectangle::Zero,
		{ cam.WorldToWinPoint(rect.mCenterPos),
		tex.mSize * trans.GetScale() },
		360.0 - fmod(trans.GetRotation(), 360.0));	// 旋转方向逆时针变顺时针
}
