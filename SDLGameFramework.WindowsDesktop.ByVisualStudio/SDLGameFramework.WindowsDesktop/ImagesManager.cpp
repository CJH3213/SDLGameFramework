#include "ImagesManager.h"
#include "WindowsManager.h"

ImagesManager& ImagesManager::GetInstance()
{
    static ImagesManager instance;
    return instance;
}

Texture ImagesManager::LoadImage(std::string path)
{
    auto it = mTextures.find(path);
    if (it != mTextures.end())
        return it->second;

    Texture tex = WindowsManager::Get(0).LoadImage(path);
    if (tex)
        mTextures[path] = tex;

    return tex;
}

void ImagesManager::Init()
{
}

void ImagesManager::Close()
{
    GetInstance().mTextures.clear();
}

Texture ImagesManager::ImageToTexture(std::string path)
{
    return GetInstance().LoadImage(path);
}
