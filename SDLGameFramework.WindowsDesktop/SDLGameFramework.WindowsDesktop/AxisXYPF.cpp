#include "AxisXYPF.h"
#include "TextureRenderer.h"
#include "ImagesManager.h"

void AxisXYPF::Load()
{
	mName = "AxisXY";

	// �����Ⱦ�����������1���Ƶ���Ļ
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
	com1->SetTexture(ImagesManager::ImageToTexture("Resource/Images/Axis_100p.png"));
}
