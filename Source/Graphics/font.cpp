//******************************************************************************
//
//
//      �t�H���g�\���p
//
//
//******************************************************************************

//------< �C���N���[�h >----------------------------------------------------------
#include "font.h"
#include "util.h"

#include <d3d11.h>

#include <string>




//------< �f�[�^ >-----------------------------------------------------------
LoadFont fontTexture[] = {
    { FONTNO::FONT0, L"./Data/Fonts/font0.png" }, // �t�H���g
    { FONTNO::FONT1, L"./Data/Fonts/font1.png" }, // �t�H���g
    { FONTNO::FONT2, L"./Data/Fonts/font2.png" }, // �t�H���g
    { FONTNO::FONT3, L"./Data/Fonts/font3.png" }, // �t�H���g
    { FONTNO::FONT4, L"./Data/Fonts/font4.png" }, // �t�H���g
    { FONTNO::FONT5, L"./Data/Fonts/font5.png" }, // �t�H���g
    { FONTNO::FONT6, L"./Data/Fonts/font6.png" }, // �t�H���g
    { -1,            nullptr                   }  // �I���t���O
};

//--------------------------------------------------------------
//  �R���X�g���N�^
//--------------------------------------------------------------
FontManager::FontManager() : font(nullptr)
{
    Graphics& graphics = Graphics::Instance();
    Init(graphics.GetDevice());
}

//--------------------------------------------------------------
//  �f�X�g���N�^
//--------------------------------------------------------------
FontManager::~FontManager()
{
    Uninit();
}

//--------------------------------------------------------------
//  ������
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
//  �I������
//--------------------------------------------------------------
void FontManager::Uninit()
{
    ReleaseAll();
    safe_delete_array(font);
}

//--------------------------------------------------------------
//  �ԍ����w�肵�ĉ��
//--------------------------------------------------------------
void FontManager::Release(int i)
{
    if ((size_t)i < FONT_NUM)
    {
        safe_delete(font[i]);
    }
}

//--------------------------------------------------------------
//  �S�ĉ��
//--------------------------------------------------------------
void FontManager::ReleaseAll()
{
    for (size_t i = 0; i < FONT_NUM; i++) Release(i);
}

//--------------------------------------------------------------
//  �e�L�X�g�\��
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
    //  �����̕`��
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
    //  �����̕`��
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
    //  ������̃t�H�[�}�b�g
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
