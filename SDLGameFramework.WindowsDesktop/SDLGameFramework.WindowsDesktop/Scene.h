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
	//// 拷贝、移动构造、拷贝、移动赋值
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
	// 整合了添加对象和预制件的方法
	template<class T>
	void AddGObj(std::shared_ptr<T>& obj);

	void RemoveGameObject(GameObject& gobj);
};

template<class T>
inline void Scene::AddGObj(std::shared_ptr<T>& obj)
{
	// 游戏对象通用的加载流程
	mAddGameObjects.emplace_back(obj);
	(*obj).mScene = this;
	(*obj).Start();

	// 编译期间判断是否为预制件，对预制件多执行一个Load()
	if constexpr(std::is_same<T, GameObject>::value == false)
		(*obj).Load();
}
