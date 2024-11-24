#include "TetrisManagerPF.h"
#include "TetrisManager.h"

void TetrisManagerPF::Load()
{
	mName = "TetrisManager";

	// 添加俄罗斯方块管理器脚本
	auto com1 = std::make_shared<TetrisManager>();
	AddComponent(com1);
}
