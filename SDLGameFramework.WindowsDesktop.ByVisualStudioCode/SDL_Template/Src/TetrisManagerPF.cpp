#include "TetrisManagerPF.h"
#include "TetrisManager.h"

void TetrisManagerPF::Load()
{
	mName = "TetrisManager";

	// ��Ӷ���˹����������ű�
	auto com1 = std::make_shared<TetrisManager>();
	AddComponent(com1);
}
