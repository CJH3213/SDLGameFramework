#include "ShapePF.h"
#include "ShapeScript.h"
#include "TextureRenderer.h"
#include "ImagesManager.h"

void ShapePF::Load()
{
	mName = "Shape";

	// �����Ⱦ�����������1���Ƶ���Ļ
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
	com1->SetTexture(ImagesManager::ImageToTexture("Resource/Axis_100p.png"));
	com1->SetOrder(1000);

	auto com2 = std::make_shared<ShapeScript>();
	this->AddComponent(com2);
}
