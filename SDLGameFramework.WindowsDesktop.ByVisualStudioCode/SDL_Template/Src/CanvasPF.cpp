#include "CanvasPF.h"
#include "RectTransform.h"
#include "Canvas.h"

void CanvasPrefab::Start()
{
	// cancas游戏对象必须使用RectTransform组件替代Transform组件
	mTransform = std::make_shared<RectTransform>();
	AddComponent(mTransform);
}

void CanvasPF::Load()
{
	mName = "Canvas";

	// 添加
	auto com1 = std::make_shared<Canvas>();
	this->AddComponent(com1);
}
