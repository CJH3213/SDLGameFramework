#include "Window.h"
#include "WindowsManager.h"
#include <SDL_image.h>
#include "Input.h"

void Window::CreateWindow()
{
	//创建窗体（显示，可缩放）
	mSDLWindow = SDL_CreateWindow(mTitle.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		(int)mWinSize.X, (int)mWinSize.Y,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	//创建渲染器（硬件加速、垂直同步）
	mSDLRenderer = SDL_CreateRenderer(mSDLWindow, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

Window::Window(std::string title, Vector2 size) :
	mTitle(title), mWinSize(size)
{
	WindowsManager::Add(this);

	CreateWindow();
}

Window::Window(const Window& other):
	mTitle(other.mTitle), mWinSize(other.mWinSize)
{
	WindowsManager::Add(this);

	CreateWindow();
}

Window::Window(Window&& other)  noexcept
{
	WindowsManager::Add(this);

	std::swap(this->mTitle, other.mTitle);
	std::swap(this->mWinSize, other.mWinSize);

	std::swap(this->mSDLWindow, other.mSDLWindow);
	std::swap(this->mSDLRenderer, other.mSDLRenderer);
}

Window& Window::operator=(const Window& other)
{
	this->mTitle = other.mTitle;
	this->mWinSize = other.mWinSize;

	CreateWindow();
	
	return *this;
}

Window& Window::operator=(Window&& other)  noexcept
{
	std::swap(this->mTitle, other.mTitle);
	std::swap(this->mWinSize, other.mWinSize);

	std::swap(this->mSDLWindow, other.mSDLWindow);
	std::swap(this->mSDLRenderer, other.mSDLRenderer);
	
	return *this;
}

Window::~Window()
{
	SDL_DestroyRenderer(mSDLRenderer);
	SDL_DestroyWindow(mSDLWindow);

	WindowsManager::Remove(this);
}

Vector2 Window::GetSize()
{
	int w, h;
	SDL_GetWindowSize(mSDLWindow, &w, &h);
	mWinSize = Vector2(w, h);
	return mWinSize;
}

void Window::SetPenColor(Color color)
{
	SDL_SetRenderDrawColor(mSDLRenderer, color.R, color.G, color.B, color.A);
}

void Window::DrawPoint(Vector2 pos)
{
	SDL_RenderDrawPointF(mSDLRenderer, pos.X, pos.Y);
}

void Window::DrawLine(Vector2 pos1, Vector2 pos2)
{
	SDL_RenderDrawLineF(mSDLRenderer, pos1.X, pos1.Y, pos2.X, pos2.Y);
}

void Window::DrawRect(Vector2 pos, Vector2 size)
{
	const SDL_FRect rect{ pos.X, pos.Y, size.X, size.Y };
	SDL_RenderDrawRectF(mSDLRenderer, &rect);
}

void Window::DrawTexture(Texture& tex, Rectangle srcRect, Rectangle dstRect, double angle, bool flipX, bool flipY)
{
	auto sdlTex = tex.GetSDLTexture();
	if (sdlTex == nullptr) return;

	SDL_Rect srcSDLRect = srcRect != Rectangle::Zero ? srcRect : tex.mSliceRect;
	SDL_Rect dstSDLRect = dstRect != Rectangle::Zero ? dstRect : SDL_Rect{0, 0, srcSDLRect.w, srcSDLRect.h};
	SDL_Point centerPos = tex.mAnchorPos;

	int flip = SDL_FLIP_NONE;
	if (flipX) flip |= SDL_FLIP_HORIZONTAL;
	if (flipY) flip |= SDL_FLIP_VERTICAL;
	
	SDL_RenderCopyEx(mSDLRenderer, sdlTex,
		&srcSDLRect, &dstSDLRect, 
		angle, &centerPos, SDL_RendererFlip(flip));
}

Texture Window::CreateTextureForRender(Vector2 size)
{
	// 作为渲染目标的纹理类型必须是TARGET，其他类型会变黑
	// 但是TARGET类型的纹理如果储存静态图片，锁屏解锁后会被销毁变黑
	SDL_Texture* sdlTexture = SDL_CreateTexture(mSDLRenderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		(int)size.X, (int)size.Y);

	return Texture(sdlTexture);
}

Texture Window::CreateTextureForImage(Vector2 size)
{
	// 图像特供的纹理使用STATIC类型，表示该纹理几乎不会变化
	// 电脑解锁后SDL会恢复该纹理，窗口会闪烁一下
	SDL_Texture* sdlTexture = SDL_CreateTexture(mSDLRenderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
		(int)size.X, (int)size.Y);

	return Texture(sdlTexture);
}

Texture Window::LoadImage(std::string path)
{
	SDL_Texture* sdlTex = IMG_LoadTexture(mSDLRenderer, path.c_str());

	return sdlTex ? Texture(sdlTex) : Texture();
}

void Window::SetRenderTarget(Texture& texture)
{
	SDL_SetRenderTarget(mSDLRenderer, texture.GetSDLTexture());
}

void Window::SetRenderTargetForWin()
{
	SDL_SetRenderTarget(mSDLRenderer, NULL);
}

bool Window::IsMinimized()
{
	return SDL_GetWindowFlags(mSDLWindow) & SDL_WINDOW_MINIMIZED;;
}

void Window::Clean()
{
	SDL_RenderClear(mSDLRenderer);
}

void Window::Present()
{
	SDL_RenderPresent(mSDLRenderer);
}
