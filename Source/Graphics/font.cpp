//******************************************************************************
//
//
//      フォント表示用
//
//
//******************************************************************************

//------< インクルード >----------------------------------------------------------
#include "font.h"
#include "util.h"

#include <d3d11.h>

#include <string>




//------< データ >-----------------------------------------------------------
LoadFont fontTexture[] = {
    { FONTNO::FONT0, L"./Data/Fonts/font0.png" }, // フォント
    { FONTNO::FONT1, L"./Data/Fonts/font1.png" }, // フォント
    { FONTNO::FONT2, L"./Data/Fonts/font2.png" }, // フォント
    { FONTNO::FONT3, L"./Data/Fonts/font3.png" }, // フォント
    { FONTNO::FONT4, L"./Data/Fonts/font4.png" }, // フォント
    { FONTNO::FONT5, L"./Data/Fonts/font5.png" }, // フォント
    { FONTNO::FONT6, L"./Data/Fonts/font6.png" }, // フォント
    { -1,            nullptr                   }  // 終了フラグ
};

//--------------------------------------------------------------
//  コンストラクタ
//--------------------------------------------------------------
FontManager::FontManager() : font(nullptr)
{
    Graphics& graphics = Graphics::Instance();
    Init(graphics.GetDevice());
}

//--------------------------------------------------------------
//  デストラクタ
//--------------------------------------------------------------
FontManager::~FontManager()
{
    Uninit();
}

//--------------------------------------------------------------
//  初期化
//--------------------------------------------------------------
void FontManager::Init(ID3D11Device* device)
{
    font = new SpriteBatch * [FONT_NUM];

    LoadFont* data = fontTexture;
    while ((size_t)data->fontNo < FONT_NUM)
    {
        font[data->fontNo] = new SpriteBatch(
            device, data->fileName, FONT_MAX_INSTANCE);
        data++;
    }
}

//--------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------
void FontManager::Uninit()
{
    ReleaseAll();
    safe_delete_array(font);
}

//--------------------------------------------------------------
//  番号を指定して解放
//--------------------------------------------------------------
void FontManager::Release(int i)
{
    if ((size_t)i < FONT_NUM)
    {
        safe_delete(font[i]);
    }
}

//--------------------------------------------------------------
//  全て解放
//--------------------------------------------------------------
void FontManager::ReleaseAll()
{
    for (size_t i = 0; i < FONT_NUM; i++) Release(i);
}

//--------------------------------------------------------------
//  テキスト表示
//--------------------------------------------------------------
float FontManager::textOut(ID3D11DeviceContext* context,
    int fontNo, std::string str,
    const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT4& color,
    TEXT_ALIGN align, bool world)
{
    if (fontNo >= FONT_NUM) return -1;
    font[fontNo]->begin(context);
    float h = font[fontNo]->textout(str, position, scale, color, align, world);
    font[fontNo]->end(context);
    return h;
}



namespace font
{

    //--------------------------------------------------------------
    //  文字の描画
    //--------------------------------------------------------------
    float textOut(
        int fontNo, std::string str,
        const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
        const DirectX::XMFLOAT4& color,
        TEXT_ALIGN align, bool world)
    {
        return FontManager::Instance().textOut(
            Graphics::Instance().GetDeviceContext(),
            fontNo, str,
            position, scale, color,
            align, world);
    }

    //--------------------------------------------------------------
    //  文字の描画
    //--------------------------------------------------------------
    float textOut(
        int fontNo, std::string str,
        float x, float y, float scaleX, float scaleY,
        float r, float g, float b, float a,
        TEXT_ALIGN align, bool world)
    {
        return textOut(fontNo, str,
            DirectX::XMFLOAT2(x, y), DirectX::XMFLOAT2(scaleX, scaleY),
            DirectX::XMFLOAT4(r, g, b, a),
            align, world);
    }

    //--------------------------------------------------------------
    //  文字列のフォーマット
    //--------------------------------------------------------------
    LPCSTR FormatString(LPCSTR format, ...)
    {
        static CHAR buf[1024];
        va_list args;
        va_start(args, format);
        vsnprintf_s(buf, sizeof(buf), 1024, format, args);
        va_end(args);
        return buf;
    }

}
