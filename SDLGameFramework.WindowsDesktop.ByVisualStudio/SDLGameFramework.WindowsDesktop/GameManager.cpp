#include "GameManager.h"
#include "Input.h"
#include "SceneManager.h"
#include "WindowsManager.h"
#include "FontManager.h"
#include "ImagesManager.h"
#include "CollisionManager.h"
#include "TetrisGameScene.h"

GameManager& GameManager::GetInstance()
{
	static GameManager instance;
	return instance;
}

void GameManager::CloseCallback()
{
	mGameState = GameState::GameOver;
}

void GameManager::Initialize()
{
	auto& ins = GetInstance();

	Time::Init();

	// 初始化SDL库
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	// 创建游戏主窗口
	std::string title = (char*)(u8"SDLGF, SDL游戏框架");
	ins.mWindow = std::make_unique<Window>(title, Vector2( 800, 600));

	// 初始化SDL_TTF库
	FontManager::Init();
	// 初始化IMG库
	ImagesManager::Init();
	// 音频管理
	AudioManager::Init();

	// 监听关闭窗口事件
	Input::AddListener(SDL_QUIT, "GM_QUIT", std::bind(&GameManager::CloseCallback, &ins));
	// 监听渲染目标变更事件
	Input::AddListener(SDL_RENDER_TARGETS_RESET, "GM_RENDER_TARGETS_RESET",
		std::bind(&WindowsManager::ReRenderTargetsCallback));

	// 应用层：初始化碰撞管理器
	CollisionManager::Init();
	// 场景初始化
	SceneManager::Init();

	/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

	// 创建场景1，俄罗斯方块游戏
	std::shared_ptr<Scene> scene1 = std::make_shared<TetrisGameScene>();
	SceneManager::LoadScene("scene01", scene1);

}

void GameManager::GameLoop()
{
	auto& ins = GetInstance();
	ins.mGameState = GameState::Running;

	while (ins.mGameState == GameState::Running)
	{
		Input::SystemUpdate();

		SceneManager::Update();
	}
}

void GameManager::CloseGame()
{
	auto& ins = GetInstance();

	SceneManager::Close();
	CollisionManager::Close();

	Input::RemoveListener(SDL_QUIT, "GM_QUIT");
	Input::RemoveListener(SDL_RENDER_TARGETS_RESET, "GM_RENDER_TARGETS_RESET");

	ins.mWindow.release();

	AudioManager::Close();
	ImagesManager::Close();
	FontManager::Close();

	// 退出SDL库
	SDL_Quit();
}
