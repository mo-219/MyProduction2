#pragma once
#include <DirectXMath.h>
#include "Graphics/Sprite.h"

// �\���̑ւ��
class Rect
{
public:
    Rect(const char* filename);
    Rect();
    ~Rect();

    bool isAmong(DirectX::XMFLOAT2 cursol);
    void setSelectColor();
    void setSprite(const char* filename);
    void setSprite();

    void fadeInInit();
    void fadeOutInit();

    bool fadeIn(float timer);          //color��0�ɂȂ�����@true ��������
    bool fadeOut(float timer);         //color��1�ɂȂ�����@true ��������
public:
    Sprite* sprite;

    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 size;         // size.x: ����   size.y: �c��
    DirectX::XMFLOAT4 color;

    bool isAmongFlag = false;
    bool fadeReady = false;

};
