#include "WindowsManager.h"
#include <algorithm>

WindowsManager& WindowsManager::GetInstance()
{
    static WindowsManager instance;
    return instance;
}

Window& WindowsManager::Get(size_t i)
{
    return *(GetInstance().mWindows.at(i));
}

void WindowsManager::Add(Window* win)
{
    GetInstance().mWindows.emplace_back(win);
}

void WindowsManager::Remove(Window* win)
{
    auto& windows = GetInstance().mWindows;
    auto it = std::find(windows.begin(), windows.end(), win);
    if (it != windows.end())
        windows.erase(it);
}

void WindowsManager::ReRenderTargetsCallback()
{
    auto& windows = GetInstance().mWindows;
    for (auto win : windows)
    {
        win->GetSize(); // 刷新每个窗口的大小
    }
}
