#include "AxisXYPF.h"
#include "TextureRenderer.h"
#include "ImagesManager.h"

void AxisXYPF::Load()
{
	mName = "AxisXY";

	// 添加渲染组件，将物体1绘制到屏幕
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
	com1->SetTexture(ImagesManager::ImageToTexture("Resource/Images/Axis_100p.png"));
}
