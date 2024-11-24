#include "BlockPF.h"
#include "TextureRenderer.h"
#include "ImagesManager.h"

void BlockPF::Load()
{
	mName = "Block";

	// �����Ⱦ�����������1���Ƶ���Ļ
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
	com1->SetTexture(ImagesManager::ImageToTexture("Resource/Images/Block.png"));
}
