#pragma once
#include <vector>
#include "GameObject.h"
#include "Prefab.h"
#include "WaitInstruction.h"

class Scene
{
	friend class SceneManager;
private:
	std::vector<std::shared_ptr<GameObject>> mGameObjects;
	std::vector<std::shared_ptr<GameObject>> mAddGameObjects;
	std::vector<Transform> mRemoveGameObjects;

public:
	//Scene();
	//~Scene();
	//// �������ƶ����졢�������ƶ���ֵ
	//Scene(const Scene& other);
	//Scene(Scene&& other) noexcept;
	//Scene& operator=(const Scene& other);
	//Scene& operator=(Scene&& other) noexcept;

	virtual void Load() {};
	void Start();
	void Update();
	void Destroy();

	void AddGameObject(std::shared_ptr<GameObject>& obj);
	void AddPrefab(std::shared_ptr <Prefab>& prefab);
	// ��������Ӷ����Ԥ�Ƽ��ķ���
	template<class T>
	void AddGObj(std::shared_ptr<T>& obj);

	void RemoveGameObject(GameObject& gobj);
};

template<class T>
inline void Scene::AddGObj(std::shared_ptr<T>& obj)
{
	// ��Ϸ����ͨ�õļ�������
	mAddGameObjects.emplace_back(obj);
	(*obj).mScene = this;
	(*obj).Start();

	// �����ڼ��ж��Ƿ�ΪԤ�Ƽ�����Ԥ�Ƽ���ִ��һ��Load()
	if constexpr(std::is_same<T, GameObject>::value == false)
		(*obj).Load();
}
