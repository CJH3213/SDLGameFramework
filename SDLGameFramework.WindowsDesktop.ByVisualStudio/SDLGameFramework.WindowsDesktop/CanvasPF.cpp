#include "CanvasPF.h"
#include "RectTransform.h"
#include "Canvas.h"

void CanvasPrefab::Start()
{
	// cancas��Ϸ�������ʹ��RectTransform������Transform���
	mTransform = std::make_shared<RectTransform>();
	AddComponent(mTransform);
}

void CanvasPF::Load()
{
	mName = "Canvas";

	// ���
	auto com1 = std::make_shared<Canvas>();
	this->AddComponent(com1);
}
