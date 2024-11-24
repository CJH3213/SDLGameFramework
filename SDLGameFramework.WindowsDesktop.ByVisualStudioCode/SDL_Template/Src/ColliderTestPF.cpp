#include "ColliderTestPF.h"
#include "BoxCollider.h"
#include "ClickTest.h"

void ColliderTestPF::Load()
{
	mName = "ColliderTest";

	// 添加渲染组件，将物体1绘制到屏幕
	auto com1 = std::make_shared<BoxCollider>();
	this->AddComponent(com1);

	auto com2 = std::make_shared<ClickTest>();
	this->AddComponent(com2);
}
