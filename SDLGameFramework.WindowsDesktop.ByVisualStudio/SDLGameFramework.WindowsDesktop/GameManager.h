#pragma once
#include <memory>
#include "Window.h"

class GameManager
{
private:
	// 单例模式下，禁用构造析构赋值
	GameManager() {};
	~GameManager() = default;
	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
	// 获取输入类单例
	static GameManager& GetInstance();

private:
	enum class GameState
	{
		Preparation, Running, Pause, GameOver
	};
	// 游戏运行状态
	GameState mGameState = GameState::Preparation;

	std::unique_ptr<Window> mWindow;

	// 窗口关闭回调
	void CloseCallback();

public:

	// 游戏框架初始化
	static void Initialize();

	// 逻辑大循环
	static void GameLoop();

	// 关闭游戏
	static void CloseGame();
};

