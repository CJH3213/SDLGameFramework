#include "TestScene.h"
#include "CameraPF.h"
#include "TestScript.h"
#include "CanvasPF.h"
#include "ButtonPF.h"

void TestScene::Load()
{
	// ��������1����Ϊ���������
	auto gobj1 = std::make_shared<CameraPF>();
	AddGObj(gobj1);

	// �����壬������
	auto gobj2 = std::make_shared<GameObject>();
	AddGObj(gobj2);
	gobj2->mName = "TestScene_TestScript_Gobj";
	auto com1 = std::make_shared<TestScript>();
	gobj2->AddComponent(com1);

	// ��������3����ΪUI����
	auto gobj3 = std::make_shared<CanvasPF>();
	AddGObj(gobj3);

	// ��������4����ΪUI��ť
	auto gobj4 = std::make_shared<ButtonPF>();
	AddGObj(gobj4);
	gobj4->GetTransform().SetParent(&gobj3->GetTransform());
}
