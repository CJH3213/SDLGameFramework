#pragma once
#include <SDL.h>
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
	Vector2 mSize;
	Rectangle mSliceRect;
	Vector2 mAnchorPos;		// Ãªµã

	Texture();
	Texture(SDL_Texture* tex, Rectangle slice = Rectangle::Zero, Vector2 anchor = Vector2::Zero);
	~Texture();

	SDL_Texture* GetSDLTexture();

	// ²»Îª¿ÕÍ¼Æ¬
	operator bool() const noexcept;
};

