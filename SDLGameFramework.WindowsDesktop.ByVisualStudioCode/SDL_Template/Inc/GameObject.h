#pragma once
#include "Component.h"
#include <vector>
#include "Transform.h"
#include <string>
#include <memory>

class Scene;

class GameObject
{
	// 虽然很多人反对friend破坏了封装
	// 但是在此恰恰是维护了封装性，只有框架内的类才能访问到私有成员
	// 二次开发者是访问不到这些成员的，维护了封装性
	friend class Scene;
	friend class SceneManager;

protected:
	std::vector<std::shared_ptr<Component>> mComponents;
	std::shared_ptr<Transform> mTransform;

	// 获取场景对象
	class Scene* GetScene();

public:
	class Scene* mScene = nullptr;
	std::string mName = "Unnamed";
	std::string mTag = "Untagged";

	virtual void Start();
	void Destroy();

	Transform& GetTransform();

	template<class T>
	void AddComponent(T&& com_sp);

	// 实例化预制件
	template<typename T>
	void Instantiate(std::shared_ptr<T>& gobj, class Transform* parent);

	// 从容器中获取一个指定类型的组件
	template<typename T>
	ComponentPtr<T> GetComponent();

	// 静态方法，在当前场景中根据名字寻找游戏对象
	static std::shared_ptr<GameObject> Find(std::string name);
};

/*
关于网上这些编程怪象，我还是想说两句，
很多人根本就是不学无术，网上刷刷视频看几个营销号的逆天言论，
就觉得自己学到东西了，到处发表他的哪些编程“规矩”，装得很懂很内行的样子，
实际上，知识是不学的，项目是没几个的，一问是三不知的，满口是茴字四种写法。
*/

template<class T>
inline void GameObject::AddComponent(T&& com_sp)
{
	mComponents.emplace_back(std::forward<T>(com_sp));
	auto& vec_com_sp = mComponents.back();
	//auto vec_com_wp = std::weak_ptr(vec_com_sp);

	(*vec_com_sp).mGameObject = this;
	(*vec_com_sp).mSelfPtr = std::weak_ptr(vec_com_sp);
	(*vec_com_sp).Awake();
}

template<typename T>
inline void GameObject::Instantiate(std::shared_ptr<T>& gobj, Transform* parent)
{
	GetScene()->AddGObj(gobj);
	gobj->GetTransform().SetParent(parent);
}

template<typename T>
inline ComponentPtr<T> GameObject::GetComponent()
{
	// 从活跃容器中找组件
	for (auto it = mComponents.begin(); it != mComponents.end(); it++)
	{
		if (typeid(T) == (*it)->GetClassInfo())
			return ComponentPtr<T>(*it);  //向下转换类型
	}
	return ComponentPtr<T>();
}
