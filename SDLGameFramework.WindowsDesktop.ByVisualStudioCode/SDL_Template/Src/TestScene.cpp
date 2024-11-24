#include "TestScene.h"
#include "CameraPF.h"
#include "TestScript.h"
#include "CanvasPF.h"
#include "ButtonPF.h"

void TestScene::Load()
{
	// 创建物体1，作为摄像机载体
	auto gobj1 = std::make_shared<CameraPF>();
	AddGObj(gobj1);

	// 空物体，测试用
	auto gobj2 = std::make_shared<GameObject>();
	AddGObj(gobj2);
	gobj2->mName = "TestScene_TestScript_Gobj";
	auto com1 = std::make_shared<TestScript>();
	gobj2->AddComponent(com1);

	// 创建物体3，作为UI画布
	auto gobj3 = std::make_shared<CanvasPF>();
	AddGObj(gobj3);

	// 创建物体4，作为UI按钮
	auto gobj4 = std::make_shared<ButtonPF>();
	AddGObj(gobj4);
	gobj4->GetTransform().SetParent(&gobj3->GetTransform());

	std::u8string wavPath = u8"Resource/Sounds/林俊杰 金莎-被风吹过的夏天.ape";
	mWav1 = AudioManager::LoadAudioFile(std::string(wavPath.cbegin(), wavPath.cend()));
	AudioManager::Play(mWav1);
}
