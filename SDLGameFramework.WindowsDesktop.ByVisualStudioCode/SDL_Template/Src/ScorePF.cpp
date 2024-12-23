#include "ScorePF.h"
#include "TextureRenderer.h"
#include "Text.h"

void ScorePF::Load()
{
	mName = "Score";

	// 添加渲染组件，将物体1绘制到屏幕
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
	//com1->SetTexture(ImagesManager::ImageToTexture("Resource/Images/Block.png"));

	// 添加文本组件
	auto com2 = std::make_shared<Text>();
	this->AddComponent(com2);
	com2->SetContent(u8"分数: 0");
	com2->SetFontSize(36);
}
