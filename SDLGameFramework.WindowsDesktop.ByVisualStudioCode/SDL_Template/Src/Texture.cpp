#include "Texture.h"
#include <SDL_image.h>
#include "WindowsManager.h"

Texture::Texture()
{
}

Texture::Texture(SDL_Texture* tex, Rectangle slice, Vector2 anchor) :
	mSliceRect(slice), mAnchorPos(anchor)
{
	mSDLTexture_SP = std::make_shared<_SDL_Texture>(tex);

	// 自动获取纹理大小
	int w, h;
	SDL_QueryTexture(GetSDLTexture(), nullptr, nullptr, &w, &h);
	mSize = { static_cast<float>(w), static_cast<float>(h) };

	// 初始裁切区域
	if (mSliceRect == Rectangle::Zero)
		mSliceRect = { mSize/2, mSize };

	// 初始锚点：中心
	mAnchorPos = mSliceRect.mCenterPos;
}

Texture::~Texture()
{
	int c = mSDLTexture_SP.use_count();
	// int b;
}

SDL_Texture* Texture::GetSDLTexture()
{
	if(mSDLTexture_SP)
		return **mSDLTexture_SP;
	
	return nullptr;
}

Vector2 Texture::GetOriginalSize()
{
	int w, h;
	SDL_QueryTexture(GetSDLTexture(), nullptr, nullptr, &w, &h);
	return { static_cast<float>(w), static_cast<float>(h) };
}

void Texture::SetWidth(float w)
{
	Vector2 size = GetOriginalSize();
	mSize = { w, size.Y * w / size.X };
}

void Texture::SetHeight(float h)
{
	Vector2 size = GetOriginalSize();
	mSize = { size.X * h / size.Y, h };
}

Texture::operator bool() const noexcept
{
	return mSDLTexture_SP && **mSDLTexture_SP;
}
