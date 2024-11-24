#include "RectanglePF.h"
#include "TextureRenderer.h"

void RectanglePF::Load()
{
	// 设定物体1的基本变换参数
	//(*gobj1).GetTransform().SetRotation(10);
	this->GetTransform().SetPosition({ 0, 0 });
	//(*gobj1).GetTransform().SetScale({ 2, 2 });

	// 添加渲染组件，将物体1绘制到屏幕
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
}
