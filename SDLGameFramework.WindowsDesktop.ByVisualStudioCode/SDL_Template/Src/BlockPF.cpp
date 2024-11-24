#include "BlockPF.h"
#include "TextureRenderer.h"
#include "ImagesManager.h"

void BlockPF::Load()
{
	mName = "Block";

	// 添加渲染组件，将物体1绘制到屏幕
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
	com1->SetTexture(ImagesManager::ImageToTexture("Resource/Images/Block.png"));
}
