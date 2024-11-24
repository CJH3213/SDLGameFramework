#include "Scene.h"


//Scene::Scene()
//{
//	std::cout << "Scene()" << this << "\n";
//}
//
//Scene::~Scene()
//{
//	std::cout << "~Scene()" << this << "\n";
//}
//
//Scene::Scene(const Scene& other)
//{
//	std::cout << "Scene(const Scene& other)" << this << "\n";
//}
//
//Scene::Scene(Scene&& other) noexcept
//{
//	std::cout << "Scene(Scene&& other)" << this << "\n";
//
//	std::swap(other.mGameObjects, this->mGameObjects);
//}
//
//Scene& Scene::operator=(const Scene& other)
//{
//	std::cout << "operator=(const Scene& other)" << this << "\n";
//
//	this->mGameObjects = other.mGameObjects;
//
//	return *this;
//}
//
//Scene& Scene::operator=(Scene&& other) noexcept
//{
//	std::cout << "operator=(Scene&& other)" << this << "\n";
//
//	std::swap(other.mGameObjects, this->mGameObjects);
//
//	return *this;
//}

void Scene::Start()
{
	
}

void Scene::Update()
{

}

void Scene::Destroy()
{

	for (auto& gobj : mGameObjects)
		gobj->Destroy();
	mGameObjects.clear();

	for (auto& gobj : mAddGameObjects)
		gobj->Destroy();
	mAddGameObjects.clear();
}

void Scene::AddGameObject(std::shared_ptr<GameObject>& obj)
{
	mAddGameObjects.emplace_back(obj);
	(*obj).mScene = this;
	(*obj).Start();
}

void Scene::AddPrefab(std::shared_ptr <Prefab>& prefab)
{
	mAddGameObjects.emplace_back(prefab);
	(*prefab).mScene = this;
	(*prefab).Start();
	(*prefab).Load();
}

void Scene::RemoveGameObject(GameObject& gobj)
{
	mRemoveGameObjects.emplace_back(gobj.GetTransform());
}

std::shared_ptr<GameObject> Scene::Find(std::string name)
{
	auto obj = std::find_if(mGameObjects.begin(), mGameObjects.end(),
		[&name](std::shared_ptr<GameObject>& obj) {return name == obj->mName; });
	return obj != mGameObjects.end() ? *obj : nullptr;
}
