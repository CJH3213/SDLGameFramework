#include "ColliderTestPF.h"
#include "BoxCollider.h"
#include "ClickTest.h"

void ColliderTestPF::Load()
{
	mName = "ColliderTest";

	// �����Ⱦ�����������1���Ƶ���Ļ
	auto com1 = std::make_shared<BoxCollider>();
	this->AddComponent(com1);

	auto com2 = std::make_shared<ClickTest>();
	this->AddComponent(com2);
}
