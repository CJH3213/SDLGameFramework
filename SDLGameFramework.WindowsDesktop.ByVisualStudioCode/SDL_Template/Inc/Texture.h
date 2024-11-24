#pragma once

extern "C"
{
	#include <SDL.h>
}

#include "Rectangle.h"
#include "Vector2.h"
#include <memory>
#include <string>


class Texture
{
private:
	struct _SDL_Texture
	{
		SDL_Texture* mSDLTexture = nullptr;
		~_SDL_Texture()
		{
			SDL_DestroyTexture(mSDLTexture);
			mSDLTexture = nullptr;
		}
		SDL_Texture* operator*() const
		{
			return mSDLTexture;
		}
	};
	std::shared_ptr<_SDL_Texture> mSDLTexture_SP;

public:
	Vector2 mSize;	// 纹理的显示大小
	Rectangle mSliceRect;	// 纹理显示截取区域，从左下角开始？
	Vector2 mAnchorPos;		// 锚点

	Texture();
	Texture(SDL_Texture* tex, Rectangle slice = Rectangle::Zero, Vector2 anchor = Vector2::Zero);
	~Texture();

	// 返回SDL纹理（内部使用，用户尽量不要调用）
	SDL_Texture* GetSDLTexture();
	// 获取纹理原始大小
	Vector2 GetOriginalSize();
	// 设置图片宽度，高度按比例改变
	void SetWidth(float w);
	// 设置图片高度, 宽度按比例改变
	void SetHeight(float h);

	// 当前对象是否含有纹理图
	operator bool() const noexcept;
};

