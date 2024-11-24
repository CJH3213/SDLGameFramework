#pragma once
#include "Texture.h"
#include "Color.h"
#include <unordered_map>
#include <map>
#include <SDL_ttf.h>

// 字形（可以逻辑或同时实现多种样式）
enum FontGlyph
{
	Normal = 0x00,	//正常
	Bold = 0x01,		//粗体
	Italic = 0x02,		//斜体
	UnderLine = 0x04,	//下划线
	StrikeThrough = 0x08		//删除线
};

// 字体样式
struct FontStyle
{
	std::string fontPath = "Resource/Fonts/simhei.ttf";	//字库名称
	size_t size = 12;	//字体大小
	Color color = Color::Black;	//字体颜色
	FontGlyph glyph = FontGlyph::Normal;
};

class FontManager
{
private:
	// 单例模式下，禁用构造析构赋值
	FontManager() {};
	~FontManager() = default;
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
	// 获取输入类单例
	static FontManager& GetInstance();

private:
	//暂存字体
	std::unordered_map<std::string, std::map<size_t, TTF_Font*>> mFontRegister;

	//通过字库路径和字体大小加载字库
	TTF_Font* LoadFont(std::string path, size_t ptsize);

public:
	static void Init();
	static void Close();
	// 将字符串生成纹理
	static Texture TextToTexture(std::u8string str, FontStyle style);
};

