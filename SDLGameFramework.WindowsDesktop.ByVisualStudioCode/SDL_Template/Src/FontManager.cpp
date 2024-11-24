#include "FontManager.h"
#include "WindowsManager.h"

FontManager& FontManager::GetInstance()
{
    static FontManager instance;
    return instance;
}

TTF_Font* FontManager::LoadFont(std::string path, size_t ptsize)
{
    auto it1 = mFontRegister.find(path);
    if (it1 != mFontRegister.end())
    {
        auto it2 = it1->second.find(ptsize);
        if (it2 != it1->second.end())
            return it2->second;
    }

    // ���������ļ�
    TTF_Font* font = TTF_OpenFont(path.c_str(), ptsize);
    if (font != nullptr)
    {
        mFontRegister[path][ptsize] = font;
        return font;
    }

    return nullptr;
}

void FontManager::Init()
{
    //��ʼ��TTF��
    if (TTF_Init() == -1)
    {
        SDL_Log("TTF_Init, ErrorCode��%s", TTF_GetError());
    }
}

void FontManager::Close()
{
    auto& fonts = GetInstance().mFontRegister;
    // ж���ֿ�
    for (auto& map : fonts)
        for(auto& font : map.second)
        {
            TTF_CloseFont(font.second);
            font.second = nullptr;
        }

    TTF_Quit();
}

Texture FontManager::TextToTexture(std::u8string str, FontStyle style)
{
    auto renderer = WindowsManager::Get(0).GetSDLRenderer();

    // ����TTF����Ч��
    TTF_Font* font = GetInstance().LoadFont(style.fontPath, style.size);
    TTF_SetFontStyle(font, style.glyph);
    // �����ı�����
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font,
        (const char*)str.c_str(), style.color);
    // ת����ͼΪ����
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    // �ͷű�����Դ
    SDL_FreeSurface(textSurface);

    return Texture(textTexture);
}
