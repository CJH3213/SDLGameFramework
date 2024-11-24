#include "SceneManager.h"
#include "Input.h"
#include "CollisionManager.h"
#include "RenderManager.h"
#include "Debug.h"

SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance;	// 通过静态局部变量确保只创建一次实例
	return instance;
}

// 固定间隔更新，用于物理
CoRet SceneManager::CoFun_FixedUpdate()
{
	while (true)
	{
		if(mActiveScene != nullptr)
		{
			auto& mGameObjects = mActiveScene->mScene->mGameObjects;
			// std::cout << "CoFun_FixedUpdate()" << "\n";
			for (auto& obj : mGameObjects)
				for (auto& com : (*obj).mComponents)
					(*com).FixedUpdate();
		}

		Yield WaitForFixedUpdate();
	}

	co_return 0;
}

// 每帧更新
CoRet SceneManager::CoFun_FrameUpdate()
{
	while (true)
	{
		auto t12 = Time::Time_s();

		if (mActiveScene != nullptr)
		{
			auto& mScenes = mActiveScene->mScene;
			auto& mAddGameObjects = mScenes->mAddGameObjects;
			auto& mGameObjects = mScenes->mGameObjects;
			auto& mRemoveGameObjects = mScenes->mRemoveGameObjects;

			// 将上一帧预添加对象正式加入容器
			for (auto& obj : mAddGameObjects)
			{
				mGameObjects.emplace_back(obj);
				//(*obj).Start();
			}
			mAddGameObjects.clear();

			// 按键状态更新
			Input::FrameUpdate();

			// 碰撞管理器更新
			CollisionManager::Update();

			// 执行脚本的Update()
			for (auto& obj : mGameObjects)
				for (auto& com : (*obj).mComponents)
					(*com).Update();

			// 检测“定时恢复任务”的协程是否继续执行
			auto t1 = Time::Time_s();
			WaitForSeconds::CheckResume();
			auto t2 = Time::Time_s();
			auto t3 = t2 - t1;

			// 执行脚本的LateUpdate()
			for (auto& obj : mGameObjects)
				for (auto& com : (*obj).mComponents)
					(*com).LateUpdate();

			// 帧渲染
			//auto t11 = Time::Time_s();
			RenderManager::Update();
			//auto t21 = Time::Time_s();
			//auto t31 = t21 - t11;
			//SDL_Log("RenderTime : %f", t31);

			// 移除本次更新销毁的对象
			for (auto& trans : mRemoveGameObjects)
			{
				auto it = std::find_if(mGameObjects.begin(), mGameObjects.end(),
					[&trans](std::shared_ptr<GameObject>& obj)
					{ // 通过比对变换组件判断是否同一个游戏对象
						return trans == obj->GetTransform();
					});

				if (it != mGameObjects.end())
				{
					(**it).Destroy();
					mGameObjects.erase(it);
				}
			}
			mRemoveGameObjects.clear();
		}

		// 本帧到此暂停，等待下一帧协程恢复
		Yield WaitForEndOfFrame();
		auto t22 = Time::Time_s();
		auto t32 = t22 - t12;
		//Debug::Log(u8"FPS : %f", 1.0 / t32);
	}

	co_return 0;
}

void SceneManager::Init()
{
	auto& ins = GetInstance();

	ins.CoFun_FixedUpdate();
	ins.CoFun_FrameUpdate();
}

void SceneManager::Update()
{
	auto& ins = GetInstance();
	auto& scenes = ins.mScenes;
	auto& removeScenes = ins.mRemoveScenes;
	auto& addScenes = ins.mAddScenes;
	auto& activeScene = ins.mActiveScene;

	// 移除标记删除的场景
	for (auto& sceneName : removeScenes)
	{
		auto it = scenes.find(sceneName);
		(*it).second->Destroy();
		scenes.erase(it);
	}
	removeScenes.clear();

	// 正式添加新场景
	for (auto& sceneInfo : addScenes)
	{
		scenes.emplace(sceneInfo.name, sceneInfo.mScene);		// 拷贝
	}
	// 将最后一个新场景作为活跃场景
	if (addScenes.size() > 0)
	{
		auto& lastAdd = addScenes.back();
		//auto& newOne = scenes[lastAdd.name];
		if(activeScene != nullptr)
			activeScene->mScene->Destroy();		// 结束场景，销毁对象
		activeScene = std::make_shared<SceneInfo>(lastAdd);
		activeScene->mScene->Start();
	}
	addScenes.clear();

	if (scenes.size() <= 0)
		return;

	if (activeScene == nullptr)
		return;

	// 活跃场景帧更新
	//activeScene->mScene->Update();
	double remainderA = WaitForFixedUpdate::CheckResume();
	double remainderB = WaitForEndOfFrame::CheckResume();
	// 帧间隔剩余间将线程睡眠，以降低CPU虚高占用
	int remainderC = std::min(remainderA, remainderB) * 1000;
	if(remainderC > 0)
		SDL_Delay(remainderC);
}

void SceneManager::LoadScene(std::string name, std::shared_ptr<Scene>& scene)
{
	auto& ins = GetInstance();
	auto& scenes = ins.mAddScenes;

	// 要销毁当前场景
	if(ins.mActiveScene != nullptr)
		ins.mRemoveScenes.emplace_back(ins.mActiveScene->name);

	// 先执行场景的加载
	scene->Load();
	// 添加新的场景（没有触发scene的构造函数）
	scenes.emplace_back(name, scene);
}

std::shared_ptr<SceneInfo> SceneManager::CurrentScene()
{
	auto& ins = GetInstance();
	auto& activeScene = ins.mActiveScene;

	return activeScene;
}
