#pragma once
#ifndef INCLUDED_FONT
#define INCLUDED_FONT

//******************************************************************************
//
//
//      �t�H���g�\���p
//
//
//******************************************************************************

//------< �C���N���[�h >----------------------------------------------------------
#include <Windows.h>
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"




const UINT FONT_NUM = 7;
const UINT FONT_MAX_INSTANCE = 256;

// �t�H���g���[�h�f�[�^
struct LoadFont
{
    int             fontNo;     // �e�N�X�`���ԍ�
    const wchar_t* fileName;   // �t�@�C����
};

// ���x����`
enum FONTNO
{
    // �^�C�g��
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
//      �t�H���g�}�l�[�W��
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
    //  �e�L�X�g�\��
    //--------------------------------------------------------------
    //  int fontNo                      �t�H���g�ԍ�
    //  std::string                     �\�����镶����
    //  float x         float y         �\���ʒu (x, y)
    //  float scaleX    float scaleY    �X�P�[�� (x, y)
    //  float r                         �Ԑ��� (0.0f ~ 1.0f)
    //  float g                         �ΐ��� (0.0f ~ 1.0f)
    //  float b                         ���� (0.0f ~ 1.0f)
    //  float a                         �s�����x (0.0f ~ 1.0f)
    //  TEXT_ALIGN align                �����̃A���C���iTEXT_ALIGN����I���j
    //  bool world                 true:���[���h���W�ɕ`�� false : �X�N���[�����W�ɕ`��
    //--------------------------------------------------------------
    //  �߂�l�Ffloat    �\�����������̍�����Ԃ�
    //--------------------------------------------------------------
    float textOut(int, std::string,
        float, float, float scaleX = (1), float scaleY = (1),
        float r = (1), float g = (1), float b = (1), float a = (1),
        TEXT_ALIGN align = (TEXT_ALIGN::UPPER_LEFT),
        bool world = (false));

    //--------------------------------------------------------------
    //  �e�L�X�g�\��
    //--------------------------------------------------------------
    //  int fontNo                  �t�H���g�ԍ�
    //  std::string                 �\�����镶����
    //  const VECTOR2&  position    �\���ʒu (x, y)
    //  const VECTOR2&  scale       �X�P�[�� (x, y)
    //  const VECTOR4&  color       �F (r, g, b, a)
    //  TEXT_ALIGN      align       �����̃A���C���iTEXT_ALIGN����I���j
    //  bool            world       true:���[���h���W�ɕ`�� false : �X�N���[�����W�ɕ`��
    //--------------------------------------------------------------
    //  �߂�l�Ffloat    �\�����������̍�����Ԃ�
    //--------------------------------------------------------------
    float textOut(int, std::string,
        const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2& scale = (DirectX::XMFLOAT2(1, 1)),
        const DirectX::XMFLOAT4& color = (DirectX::XMFLOAT4(1, 1, 1, 1)),
        TEXT_ALIGN align = (TEXT_ALIGN::UPPER_LEFT),
        bool world = (false));

    LPCSTR FormatString(LPCSTR format, ...);

}
#endif // !INCLUDED_FONT