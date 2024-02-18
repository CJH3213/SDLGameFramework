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

	// �Զ���ȡ������С
	int w, h;
	SDL_QueryTexture(GetSDLTexture(), nullptr, nullptr, &w, &h);
	mSize = { static_cast<float>(w), static_cast<float>(h) };

	// ��ʼ��������
	if (mSliceRect == Rectangle::Zero)
		mSliceRect = { mSize/2, mSize };

	// ��ʼê�㣺����
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

Texture::operator bool() const noexcept
{
	return mSDLTexture_SP && **mSDLTexture_SP;
}