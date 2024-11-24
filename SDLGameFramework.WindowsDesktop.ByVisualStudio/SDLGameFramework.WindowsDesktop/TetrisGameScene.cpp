#include "TetrisGameScene.h"
#include "CameraPF.h"
#include "AxisXYPF.h"
#include "TetrisManagerPF.h"


void TetrisGameScene::Load()
{
	// ��������1����Ϊ���������
	auto gobj1 = std::make_shared<CameraPF>();
	this->AddGObj(gobj1);

	// ��������2����Ϊ������ͼƬ����
	auto gobj2 = std::make_shared<AxisXYPF>();
	this->AddGObj(gobj2);

	// ��������3����Ϊ����˹������Ϸ������
	auto gobj3 = std::make_shared<TetrisManagerPF>();
	this->AddGObj(gobj3);

	// MS�༭����Ĭ��GBK���룬�����ֽ�һ���֣�SDLֻ����UTF8���룬���ֽ�һ����
	std::u8string wavPath = u8"Resource/Sounds/�ܽ���-����.ape";
	mWav1 = AudioManager::LoadAudioFile(std::string(wavPath.cbegin(), wavPath.cend()));
	AudioManager::Play(mWav1);
}
