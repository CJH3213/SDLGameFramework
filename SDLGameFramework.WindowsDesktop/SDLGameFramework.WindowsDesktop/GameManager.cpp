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

	// ��ʼ��SDL��
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	// ������Ϸ������
	std::string title = (char*)(u8"SDLGF, SDL��Ϸ���");
	ins.mWindow = std::make_unique<Window>(title, Vector2( 800, 600));

	// ��ʼ��SDL_TTF��
	FontManager::Init();
	// ��ʼ��IMG��
	ImagesManager::Init();
	// ��Ƶ����
	AudioManager::Init();

	// �����رմ����¼�
	Input::AddListener(SDL_QUIT, "GM_QUIT", std::bind(&GameManager::CloseCallback, &ins));
	// ������ȾĿ�����¼�
	Input::AddListener(SDL_RENDER_TARGETS_RESET, "GM_RENDER_TARGETS_RESET",
		std::bind(&WindowsManager::ReRenderTargetsCallback));

	// Ӧ�ò㣺��ʼ����ײ������
	CollisionManager::Init();
	// ������ʼ��
	SceneManager::Init();

	/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

	// ��������1������˹������Ϸ
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

	// �˳�SDL��
	SDL_Quit();
}
