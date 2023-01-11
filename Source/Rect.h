#pragma once
#include <DirectXMath.h>
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Graphics/Graphics.h"

// �\���̑ւ��
class Rect
{
public:
    Rect();
    Rect(const char* filename);
    ~Rect();

    void SetSprite(const char* filename);
    void SetSprite();

    virtual void Render(const RenderContext& rc, SpriteShader* shader);

    void SetAll(float posX, float posY, float sizeX, float sizeY, float imageSizeX, float imageSizeY, float r, float g, float b, float a);

    void Clear();

    virtual void setPosition(DirectX::XMFLOAT2 pos) { position = pos; }
    void setSize(DirectX::XMFLOAT2 si) { size = si; }             // �`�悷��傫��
    void setSize(DirectX::XMFLOAT2 si, bool on);                  // �`�悷��傫�� on��imageSize = size�ɂ���
    void setImageSize(DirectX::XMFLOAT2 si) { imageSize = si; }
    void setColor(DirectX::XMFLOAT4 col) { color = col; }

public:
    Sprite* sprite;
    Texture* texture;

    // �`��ʒu
    DirectX::XMFLOAT2 position = {0,0};

    // �摜�̕`��J�n�ʒu
    DirectX::XMFLOAT2 imagePosition = {0,0};

    // �`�悷��T�C�Y
    DirectX::XMFLOAT2 size = {};              // size.x: ����   size.y: �c��
    
    // �摜���̂̃T�C�Y 
    DirectX::XMFLOAT2 imageSize = {};         // size.x: ����   size.y: �c��

    // �F
    DirectX::XMFLOAT4 color = {1,1,1,1};



};


// Fade�pRECT
class RectFade : public Rect
{
public:
    RectFade();

    void fadeInInit();
    void fadeOutInit();

    bool fadeIn(float timer);          //color��0�ɂȂ�����@true ��������
    bool fadeOut(float timer);         //color��1�ɂȂ�����@true ��������

private:
    bool fadeReady = false;
};


// �I�����pRECT
class RectSelect : public Rect
{
public:
    RectSelect();
    bool isAmong(DirectX::XMFLOAT2 cursol);
    void setSelectColor();

private:
    bool isAmongFlag = false;
};


// UIBar�pRect
class RectBar : public Rect
{
public:
    RectBar();
    RectBar(const char* fileName);
    RectBar(const char* fileName, const char* baseFileName);
    ~RectBar();

    void setPosition(DirectX::XMFLOAT2 pos)override;
    void setbaseSize(DirectX::XMFLOAT2 size) { baseSize = size;  basePos = DirectX::XMFLOAT2(position.x-baseSize.x, position.y-baseSize.y); }
    void setbaseColor(DirectX::XMFLOAT4 col) { baseColor = col; }

    void SetBaseAll(float baseSizeX, float baseSizeY, float r, float g, float b, float a);

    void culcValue(float currentVal, float maxVal);

    void Render(const RenderContext& rc, SpriteShader* shader)override;
private:
    Sprite* baseSprite;
    Texture* baseTexture;

    float sizeValue = 0;
    DirectX::XMFLOAT2 basePos = {0,0};      // ���n�̕�
    DirectX::XMFLOAT2 baseImageSize = {2,2};      // ���n�̕�
    DirectX::XMFLOAT2 baseSize = {2,2};      // ���n�̕�
    DirectX::XMFLOAT4 baseColor = {0,0,0,1};     // ���n�̐F

};