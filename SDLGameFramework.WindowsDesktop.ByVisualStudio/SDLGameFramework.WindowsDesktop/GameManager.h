#pragma once
#include <memory>
#include "Window.h"

class GameManager
{
private:
	// ����ģʽ�£����ù���������ֵ
	GameManager() {};
	~GameManager() = default;
	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
	// ��ȡ�����൥��
	static GameManager& GetInstance();

private:
	enum class GameState
	{
		Preparation, Running, Pause, GameOver
	};
	// ��Ϸ����״̬
	GameState mGameState = GameState::Preparation;

	std::unique_ptr<Window> mWindow;

	// ���ڹرջص�
	void CloseCallback();

public:

	// ��Ϸ��ܳ�ʼ��
	static void Initialize();

	// �߼���ѭ��
	static void GameLoop();

	// �ر���Ϸ
	static void CloseGame();
};

