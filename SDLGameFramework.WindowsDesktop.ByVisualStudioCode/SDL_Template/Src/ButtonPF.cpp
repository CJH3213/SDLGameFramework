#include "ButtonPF.h"
#include "Button.h"
#include "ImagesManager.h"

void ButtonPF::Load()
{
	mName = "Button";

	// 添加画布渲染组件
	auto com1 = std::make_shared<CanvasRenderer>();
	AddComponent(com1);

	// 添加盒形碰撞器
	auto com2 = std::make_shared<BoxCollider>();
	AddComponent(com2);

	// 添加按钮脚本组件
	auto com3 = std::make_shared<Button>();
	AddComponent(com3);
	com3->SetNormalTex(ImagesManager::ImageToTexture("Resource/Images/button.png"));
	com3->SetmHighLigTex(ImagesManager::ImageToTexture("Resource/Images/button_hl.png"));
	com3->SetPressedTex(ImagesManager::ImageToTexture("Resource/Images/button_pres.png"));
}
