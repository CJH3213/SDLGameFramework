#pragma once
#include <vector>
#include <unordered_map>
#include "Scene.h"

struct SceneInfo
{
	std::string name;
	std::shared_ptr<Scene> mScene;
};

// 场景管理器（单例）
class SceneManager
{
private:
	// 单例模式下，禁用构造析构赋值
	SceneManager() {};
	~SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	static SceneManager& GetInstance();

private:
	std::unordered_map<std::string, std::shared_ptr<Scene>> mScenes;
	std::vector<SceneInfo> mAddScenes;
	std::vector<std::string> mRemoveScenes;

	std::shared_ptr<SceneInfo> mActiveScene = nullptr;

	CoRet CoFun_FixedUpdate();
	CoRet CoFun_FrameUpdate();

public:
	static void Init();
	static void Close() {};
	static void Update();
	static void LoadScene(std::string name, std::shared_ptr<Scene>& scene);
	static std::shared_ptr<SceneInfo> CurrentScene();
};

