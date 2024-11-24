#include "NextText.h"
#include "TextureRenderer.h"
#include "Text.h"

void NextText::Load()
{
	mName = "Next";

	// �����Ⱦ�����������1���Ƶ���Ļ
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
	//com1->SetTexture(ImagesManager::ImageToTexture("Resource/Images/Block.png"));

	// ����ı����
	auto com2 = std::make_shared<Text>();
	this->AddComponent(com2);
	com2->SetContent(u8"��һ��:");
	com2->SetFontSize(36);
}
