// SDLGameFramework.WindowsDesktop.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "GameManager.h"

int main(int argc, char** argv)
{
    std::cout << "Hello World!\n";

    GameManager::Initialize();
    GameManager::GameLoop();
    GameManager::CloseGame();

    return 0;
}
