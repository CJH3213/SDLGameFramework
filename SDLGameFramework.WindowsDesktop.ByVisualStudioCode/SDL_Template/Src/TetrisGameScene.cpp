#include "TetrisGameScene.h"
#include "CameraPF.h"
#include "AxisXYPF.h"
#include "TetrisManagerPF.h"
#include "CanvasPF.h"


void TetrisGameScene::Load()
{
	// 创建物体1，作为摄像机载体
	auto gobj1 = std::make_shared<CameraPF>();
	AddGObj(gobj1);

	// 创建物体3，作为UI画布
	auto gobj2 = std::make_shared<CanvasPF>();
	AddGObj(gobj2);

	// 得分文本
	auto gobj2_1 = std::make_shared<ScoreTextPF>();
	AddGObj(gobj2_1);
	gobj2_1->GetTransform().SetParent(&gobj2->GetTransform());

	// 下一个形状文本
	auto gobj2_2 = std::make_shared<NextTextPF>();
	AddGObj(gobj2_2);
	gobj2_2->GetTransform().SetParent(&gobj2->GetTransform());

	// 下一个形状的父对象
	auto gobj2_3 = std::make_shared<NextShapePF>();
	AddGObj(gobj2_3);
	gobj2_3->GetTransform().SetParent(&gobj2->GetTransform());

	// 创建物体2，作为坐标轴图片载体
	auto gobj3 = std::make_shared<AxisXYPF>();
	AddGObj(gobj3);

	// 创建物体3，作为俄罗斯方块游戏管理器
	auto gobj4 = std::make_shared<TetrisManagerPF>();
	AddGObj(gobj4);

	// MS编辑界面默认GBK编码，两个字节一汉字；SDL只接受UTF8编码，三字节一汉字
	std::u8string wavPath = u8"Resource/Sounds/周杰伦-稻香.ape";
	mWav1 = AudioManager::LoadAudioFile(std::string(wavPath.cbegin(), wavPath.cend()));
	AudioManager::Play(mWav1);

	//wavPath = u8"Resource/Sounds/林俊杰 金莎-被风吹过的夏天.ape";
	//mWav2 = AudioManager::LoadAudioFile(std::string(wavPath.cbegin(), wavPath.cend()));
	//AudioManager::Play(mWav2);
}

#include "Text.h"
void ScoreTextPF::Load()
{
	mName = "ScoreText";

	auto rectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*rectTrans).SetAnchorMin({ 0.0f, 0.5f });
	(*rectTrans).SetAnchorMax({ 0.0f, 0.5f });
	(*rectTrans).SetAnchoredPosition({ 100.0f, 120.0f });

	// 添加画布渲染组件
	auto com1 = std::make_shared<CanvasRenderer>();
	AddComponent(com1);

	// 添加文本UI组件
	auto com2 = std::make_shared<Text>();
	AddComponent(com2);

	com2->SetContent(u8"得分：0");
	com2->SetFontSize(36);
}

void NextTextPF::Load()
{
	mName = "NextText";

	auto rectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*rectTrans).SetAnchorMin({ 0.0f, 0.5f });
	(*rectTrans).SetAnchorMax({ 0.0f, 0.5f });
	(*rectTrans).SetAnchoredPosition({ 100.0f, 60.0f });

	// 添加画布渲染组件
	auto com1 = std::make_shared<CanvasRenderer>();
	AddComponent(com1);

	// 添加文本UI组件
	auto com2 = std::make_shared<Text>();
	AddComponent(com2);

	com2->SetContent(u8"下一个形状：");
	com2->SetFontSize(36);
}

void NextShapePF::Load()
{
	mName = "NextShape";

	auto rectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*rectTrans).SetAnchorMin({ 0.0f, 0.5f });
	(*rectTrans).SetAnchorMax({ 0.0f, 0.5f });
	//(*rectTrans).SetAnchoredPosition({ 100.0f, 00.0f });
	(*rectTrans).SetInsetAndSizeFromParentEdge(RectTransform::Edge::Left, 50, 100);
}
