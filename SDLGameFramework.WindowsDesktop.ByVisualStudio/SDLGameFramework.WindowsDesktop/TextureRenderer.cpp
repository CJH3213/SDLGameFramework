#include "TextureRenderer.h"
#include "RenderManager.h"
#include "Transform.h"
#include "WindowsManager.h"
#include "Time.h"

void TextureRenderer::Awake()
{
	mRenderIF = std::make_shared<TexRenIF>(this);
	RenderManager::AddRenderer(mRenderIF);

	//mTexture = ImagesManager::ImageToTexture("Resource/acheesements_icons.png");
}

void TextureRenderer::SetOrder(int order)
{
	if (order == mOrder)
		return;

	mOrder = order;
	RenderManager::ChangeOrder(*mRenderIF, mOrder);
}

int TextureRenderer::TexRenIF::GetOrder()
{
	return mTexRenCom->mOrder;
}

void TextureRenderer::TexRenIF::Draw(Camera& cam)
{
	auto& tex = mTexRenCom->mTexture;
	auto& trans = mTexRenCom->GetTransform();
	auto& win = WindowsManager::Get(0);

	win.DrawTexture(tex, Rectangle::Zero,
		{ cam.WorldToWinPoint(trans.GetPosition()),
		tex.mSliceRect.mSize * trans.GetScale()},
		360.0 - fmod( trans.GetRotation(), 360.0));	// 旋转方向逆时针变顺时针
}
