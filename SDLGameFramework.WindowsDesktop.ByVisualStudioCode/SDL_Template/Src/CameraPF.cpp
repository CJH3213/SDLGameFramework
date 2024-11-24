#include "CameraPF.h"
#include "TextureRenderer.h"
#include "Camera.h"
#include "WindowsManager.h"

#include "ClickTest.h"

void CameraPF::Load()
{
	// 添加渲染组件，将物体1绘制到屏幕
	//auto com1 = std::make_shared<TextureRenderer>();
	//this->AddComponent(com1);

	// 添加摄像机组件
	auto com2 = std::make_shared<Camera>();
	this->AddComponent(com2);
	com2->mIsMainCam = true;
	//com2->mScreenPos = { 0, 0 };		
	//com2->ResetSize({600,  400});

	// 添加测试脚本
	//auto com3 = std::make_shared<ClickTest>();
	//this->AddComponent(com3);
}
