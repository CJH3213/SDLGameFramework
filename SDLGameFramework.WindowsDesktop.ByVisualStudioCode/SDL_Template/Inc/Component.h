#pragma once
#include <memory>
#include <vector>
#include "WaitInstruction.h"
#include <typeinfo>

class Component;
class GameObject;

// 封装智能指针和真指针类型，配合智能指针使用
template<class T>
class ComponentPtr
{
public:

	std::weak_ptr<Component> mSelfPtr;
	T* mComponent = nullptr;

	ComponentPtr() {};

	ComponentPtr(const std::weak_ptr<Component>& wp)
		: mSelfPtr(wp)
	{
		// 弱指针有效，取出组件实际指针
		if (mSelfPtr.expired() == false)
		{
			Component* cc = mSelfPtr.lock().get();
			mComponent = dynamic_cast<T*>(cc);
		}
	}

	ComponentPtr(const std::shared_ptr<Component>& sp) :
		ComponentPtr(std::weak_ptr<Component>(sp)) {}

	T& operator*() const
	{
		return *mComponent;
	}

	operator bool() const noexcept
	{
		return mSelfPtr.expired();	// true已失效，false有效
	}

	bool operator==(const ComponentPtr& other) const
	{
		return mComponent == other.mComponent;
	}

	template<class T1>
	ComponentPtr<T1> As() const;
};

class Component
{
	friend class SceneManager;
	friend class GameObject;

private:
	class Scene* GetScene();
	class GameObject* GetGameObject();

	std::vector<CoRet> mCoRets;
	//标记是否已经执行过Start方法
	bool mIsStarted;

	void _Update();
	void _OnDestroy();

protected:
	// 申请销毁游戏对象
	void Destroy(class GameObject& gObj);
	// 销毁所有子对象（不包括当前对象）
	void DestroyAllChildren(class GameObject& gObj);

public:
	class GameObject* mGameObject = nullptr;
	std::weak_ptr<Component> mSelfPtr;

	class Transform& GetTransform();

	virtual void Awake() {};
	virtual void OnEnable() {};
	virtual void Start() {};
	virtual void FixedUpdate() {};
	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void OnPause() {};
	virtual void OnQuit() {};
	virtual void OnDisable() {};
	virtual void OnDestroy() {};

	// virtual void OnTrigger() {};
	// virtual void OnCollision() {};
	virtual void OnRender() {};

	bool operator==(const Component& other) const;

	// 实例化预制件
	template<typename T>
	void Instantiate(std::shared_ptr<T>& obj, class Transform* parent = nullptr);
	// 从容器中获取一个指定类型的组件
	//template<typename T>
	//ComponentPtr<T> GetComponent();

	//获取当前类信息
	inline virtual const std::type_info& GetClassInfo() { return typeid(*this); }

	void StartCoroutine(CoRet coRet);
};

template<typename T>
inline void Component::Instantiate(std::shared_ptr<T>& gobj, Transform* parent)
{
	//GetScene()->AddGObj(gobj);
	//gobj->GetTransform().SetParent(parent);
	GetGameObject()->Instantiate(gobj, parent);
}

//template<typename T>
//inline ComponentPtr<T> Component::GetComponent()
//{
//	return GetGameObject()->GetComponent<T>();
//}

template<class T>
template<class T1>
inline ComponentPtr<T1> ComponentPtr<T>::As() const
{
	return ComponentPtr<T1>(mSelfPtr);
}
