#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"

Scene* GameObject::GetScene()
{
	return mScene;
}

void GameObject::Start()
{
	mTransform = std::make_shared<Transform>();
	//mComponents.emplace_back(mTransform);
	AddComponent(mTransform);
}

void GameObject::Destroy()
{
	for (auto& com : mComponents)
		com->_OnDestroy();
	mComponents.clear();
}

Transform& GameObject::GetTransform()
{
	return *(mTransform);
}

std::shared_ptr<GameObject> GameObject::Find(std::string name)
{
	auto scene = SceneManager::CurrentScene();
	return scene->mScene->Find(name);
}
