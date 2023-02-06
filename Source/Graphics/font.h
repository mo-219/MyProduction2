#pragma once
#ifndef INCLUDED_FONT
#define INCLUDED_FONT

//******************************************************************************
//
//
//      フォント表示用
//
//
//******************************************************************************

//------< インクルード >----------------------------------------------------------
#include <Windows.h>
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"




const UINT FONT_NUM = 7;
const UINT FONT_MAX_INSTANCE = 256;

// フォントロードデータ
struct LoadFont
{
    int             fontNo;     // テクスチャ番号
    const wchar_t* fileName;   // ファイル名
};

// ラベル定義
enum FONTNO
{
    // タイトル
    FONT0 = 0,
    FONT1,
    FONT2,
    FONT3,
    FONT4,
    FONT5,
    FONT6,
};

//==========================================================================
//
//      フォントマネージャ
//
//==========================================================================
class FontManager
{
private:
    SpriteBatch** font;

    FontManager();
    ~FontManager();
public:


    static FontManager& Instance()
    {
        static FontManager instance;
        return instance;
    }

    void Init(ID3D11Device*);
    void Uninit();
    void Release(int);
    void ReleaseAll();

    float textOut(ID3D11DeviceContext*, int,
        std::string,
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT4&,
        TEXT_ALIGN align = (TEXT_ALIGN::UPPER_LEFT), bool world = (false));
};

namespace font
{
    //--------------------------------------------------------------
    //  テキスト表示
    //--------------------------------------------------------------
    //  int fontNo                      フォント番号
    //  std::string                     表示する文字列
    //  float x         float y         表示位置 (x, y)
    //  float scaleX    float scaleY    スケール (x, y)
    //  float r                         赤成分 (0.0f ~ 1.0f)
    //  float g                         緑成分 (0.0f ~ 1.0f)
    //  float b                         青成分 (0.0f ~ 1.0f)
    //  float a                         不透明度 (0.0f ~ 1.0f)
    //  TEXT_ALIGN align                文字のアライン（TEXT_ALIGNから選択）
    //  bool world                 true:ワールド座標に描画 false : スクリーン座標に描画
    //--------------------------------------------------------------
    //  戻り値：float    表示した文字の高さを返す
    //--------------------------------------------------------------
    float textOut(int, std::string,
        float, float, float scaleX = (1), float scaleY = (1),
        float r = (1), float g = (1), float b = (1), float a = (1),
        TEXT_ALIGN align = (TEXT_ALIGN::UPPER_LEFT),
        bool world = (false));

    //--------------------------------------------------------------
    //  テキスト表示
    //--------------------------------------------------------------
    //  int fontNo                  フォント番号
    //  std::string                 表示する文字列
    //  const VECTOR2&  position    表示位置 (x, y)
    //  const VECTOR2&  scale       スケール (x, y)
    //  const VECTOR4&  color       色 (r, g, b, a)
    //  TEXT_ALIGN      align       文字のアライン（TEXT_ALIGNから選択）
    //  bool            world       true:ワールド座標に描画 false : スクリーン座標に描画
    //--------------------------------------------------------------
    //  戻り値：float    表示した文字の高さを返す
    //--------------------------------------------------------------
    float textOut(int, std::string,
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2& scale = (DirectX::XMFLOAT2(1, 1)),
        const DirectX::XMFLOAT4& color = (DirectX::XMFLOAT4(1, 1, 1, 1)),
        TEXT_ALIGN align = (TEXT_ALIGN::UPPER_LEFT),
        bool world = (false));

    LPCSTR FormatString(LPCSTR format, ...);

}
#endif // !INCLUDED_FONT