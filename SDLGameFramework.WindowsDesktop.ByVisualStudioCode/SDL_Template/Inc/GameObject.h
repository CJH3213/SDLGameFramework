#pragma once
#include "Component.h"
#include <vector>
#include "Transform.h"
#include <string>
#include <memory>

class Scene;

class GameObject
{
	// ��Ȼ�ܶ��˷���friend�ƻ��˷�װ
	// �����ڴ�ǡǡ��ά���˷�װ�ԣ�ֻ�п���ڵ�����ܷ��ʵ�˽�г�Ա
	// ���ο������Ƿ��ʲ�����Щ��Ա�ģ�ά���˷�װ��
	friend class Scene;
	friend class SceneManager;

protected:
	std::vector<std::shared_ptr<Component>> mComponents;
	std::shared_ptr<Transform> mTransform;

	// ��ȡ��������
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

	// ʵ����Ԥ�Ƽ�
	template<typename T>
	void Instantiate(std::shared_ptr<T>& gobj, class Transform* parent);

	// �������л�ȡһ��ָ�����͵����
	template<typename T>
	ComponentPtr<T> GetComponent();

	// ��̬�������ڵ�ǰ�����и�������Ѱ����Ϸ����
	static std::shared_ptr<GameObject> Find(std::string name);
};

/*
����������Щ��̹����һ�����˵���䣬
�ܶ��˸������ǲ�ѧ����������ˢˢ��Ƶ������Ӫ���ŵ��������ۣ�
�;����Լ�ѧ�������ˣ���������������Щ��̡���ء���װ�úܶ������е����ӣ�
ʵ���ϣ�֪ʶ�ǲ�ѧ�ģ���Ŀ��û�����ģ�һ��������֪�ģ���������������д����
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
	// �ӻ�Ծ�����������
	for (auto it = mComponents.begin(); it != mComponents.end(); it++)
	{
		if (typeid(T) == (*it)->GetClassInfo())
			return ComponentPtr<T>(*it);  //����ת������
	}
	return ComponentPtr<T>();
}
