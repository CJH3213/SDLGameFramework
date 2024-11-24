#include "SceneManager.h"
#include "Input.h"
#include "CollisionManager.h"
#include "RenderManager.h"
#include "Debug.h"

SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance;	// ͨ����̬�ֲ�����ȷ��ֻ����һ��ʵ��
	return instance;
}

// �̶�������£���������
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

// ÿ֡����
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

			// ����һ֡Ԥ��Ӷ�����ʽ��������
			for (auto& obj : mAddGameObjects)
			{
				mGameObjects.emplace_back(obj);
				//(*obj).Start();
			}
			mAddGameObjects.clear();

			// ����״̬����
			Input::FrameUpdate();

			// ��ײ����������
			CollisionManager::Update();

			// ִ�нű���Update()
			for (auto& obj : mGameObjects)
				for (auto& com : (*obj).mComponents)
					(*com).Update();

			// ��⡰��ʱ�ָ����񡱵�Э���Ƿ����ִ��
			auto t1 = Time::Time_s();
			WaitForSeconds::CheckResume();
			auto t2 = Time::Time_s();
			auto t3 = t2 - t1;

			// ִ�нű���LateUpdate()
			for (auto& obj : mGameObjects)
				for (auto& com : (*obj).mComponents)
					(*com).LateUpdate();

			// ֡��Ⱦ
			//auto t11 = Time::Time_s();
			RenderManager::Update();
			//auto t21 = Time::Time_s();
			//auto t31 = t21 - t11;
			//SDL_Log("RenderTime : %f", t31);

			// �Ƴ����θ������ٵĶ���
			for (auto& trans : mRemoveGameObjects)
			{
				auto it = std::find_if(mGameObjects.begin(), mGameObjects.end(),
					[&trans](std::shared_ptr<GameObject>& obj)
					{ // ͨ���ȶԱ任����ж��Ƿ�ͬһ����Ϸ����
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

		// ��֡������ͣ���ȴ���һ֡Э�ָ̻�
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

	// �Ƴ����ɾ���ĳ���
	for (auto& sceneName : removeScenes)
	{
		auto it = scenes.find(sceneName);
		(*it).second->Destroy();
		scenes.erase(it);
	}
	removeScenes.clear();

	// ��ʽ����³���
	for (auto& sceneInfo : addScenes)
	{
		scenes.emplace(sceneInfo.name, sceneInfo.mScene);		// ����
	}
	// �����һ���³�����Ϊ��Ծ����
	if (addScenes.size() > 0)
	{
		auto& lastAdd = addScenes.back();
		//auto& newOne = scenes[lastAdd.name];
		if(activeScene != nullptr)
			activeScene->mScene->Destroy();		// �������������ٶ���
		activeScene = std::make_shared<SceneInfo>(lastAdd);
		activeScene->mScene->Start();
	}
	addScenes.clear();

	if (scenes.size() <= 0)
		return;

	if (activeScene == nullptr)
		return;

	// ��Ծ����֡����
	//activeScene->mScene->Update();
	double remainderA = WaitForFixedUpdate::CheckResume();
	double remainderB = WaitForEndOfFrame::CheckResume();
	// ֡���ʣ��佫�߳�˯�ߣ��Խ���CPU���ռ��
	int remainderC = std::min(remainderA, remainderB) * 1000;
	if(remainderC > 0)
		SDL_Delay(remainderC);
}

void SceneManager::LoadScene(std::string name, std::shared_ptr<Scene>& scene)
{
	auto& ins = GetInstance();
	auto& scenes = ins.mAddScenes;

	// Ҫ���ٵ�ǰ����
	if(ins.mActiveScene != nullptr)
		ins.mRemoveScenes.emplace_back(ins.mActiveScene->name);

	// ��ִ�г����ļ���
	scene->Load();
	// ����µĳ�����û�д���scene�Ĺ��캯����
	scenes.emplace_back(name, scene);
}

std::shared_ptr<SceneInfo> SceneManager::CurrentScene()
{
	auto& ins = GetInstance();
	auto& activeScene = ins.mActiveScene;

	return activeScene;
}
