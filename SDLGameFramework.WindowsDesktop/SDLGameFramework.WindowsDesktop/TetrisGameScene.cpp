#include "TetrisGameScene.h"
#include "CameraPF.h"
#include "AxisXYPF.h"
#include "TetrisManagerPF.h"


void TetrisGameScene::Load()
{
	// 创建物体1，作为摄像机载体
	auto gobj1 = std::make_shared<CameraPF>();
	this->AddGObj(gobj1);

	// 创建物体2，作为坐标轴图片载体
	auto gobj2 = std::make_shared<AxisXYPF>();
	this->AddGObj(gobj2);

	// 创建物体3，作为俄罗斯方块游戏管理器
	auto gobj3 = std::make_shared<TetrisManagerPF>();
	this->AddGObj(gobj3);

	// MS编辑界面默认GBK编码，两个字节一汉字；SDL只接受UTF8编码，三字节一汉字
	std::u8string wavPath = u8"Resource/Sounds/周杰伦-稻香.ape";
	mWav1 = AudioManager::LoadAudioFile(std::string(wavPath.cbegin(), wavPath.cend()));
	AudioManager::Play(mWav1);
}
