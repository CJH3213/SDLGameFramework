#pragma once
//#include "Window.h"
#include "Vector2.h"
#include "RenderInterface.h"
#include "WaitInstruction.h"
#include <SDL_log.h>

class Debug
{
private:
	// 单例模式下，禁用构造析构赋值
	Debug() {};
	~Debug() = default;
	Debug(const Debug&) = delete;
	Debug& operator=(const Debug&) = delete;
	
	struct LineRenIF : RenderInterface
	{
		Vector2 mPos1, mPos2;
		Color mColor = Color::Red;
		LineRenIF(Vector2 v1, Vector2 v2, Color c) :mPos1(v1), mPos2(v2), mColor(c) {};
		virtual int GetOrder() override { return 0; };
		virtual void Draw(Camera& cam) override;
	};

	static CoRet CoFun_WaitSec(std::shared_ptr<LineRenIF> sp, float sec);

public:
	// 获取输入类单例
	static Debug& GetInstance();

	static void DrawLine(Vector2 pos1, Vector2 pos2, Color color, float sec = 0.01f);
	static void DrawRectangle(Rectangle rect, Color color, float sec = 0.01f);
	template<typename... Args>
	static void Log(std::u8string str, Args... args);
};

template<typename ...Args>
inline void Debug::Log(std::u8string str, Args... args)
{
	SDL_Log((const char*)str.c_str(), args...);
}
