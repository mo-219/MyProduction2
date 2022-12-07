#include "Rect.h"


// マウスが中にいるかどうか
//Rect::Rect(const char* filename)
//{
//    //sprite = new Sprite(filename); 
//    isAmongFlag = false;
//
//}

Rect::Rect()
{
    //sprite = new Sprite(); 
    isAmongFlag = false;
}

Rect::~Rect()
{
    if (sprite != nullptr)
    {
        delete sprite;
        sprite = nullptr;
    }
}


//void Rect::setSprite(const char* filename)
//{
//    sprite = new Sprite();
//}

void Rect::setSprite()
{
    sprite = new Sprite();
}

void Rect::fadeInInit()
{
    if (fadeReady)   return;

    color.w = 0.0f;
    fadeReady = true;
}

void Rect::fadeOutInit()
{
    if (fadeReady)   return;

    color.w = 1.0f;
    fadeReady = true;
}

// 画面を徐々に消していく
bool Rect::fadeIn(float timer)
{
    if (!fadeReady) fadeInInit();
    if (color.w >= 1.0f)
    {
        fadeReady = false;
        color.w = 1.0f;
        return true;
    }
    color.w += timer;
    return false;
}

// 画面を徐々にみせていく
bool Rect::fadeOut(float timer)
{
    if (!fadeReady) fadeOutInit();
    if (color.w <= 0.0f)
    {
        fadeReady = false;
        color.w = 0.0f;

        return true;
    }
    color.w -= timer;
    return false;
}

bool Rect::isAmong(DirectX::XMFLOAT2 cursol)
{
    isAmongFlag = false;

    float left = pos.x;
    float right = pos.x + size.x;
    float top = pos.y;
    float bottom = pos.y + size.y;

    if (cursol.x < left)         return false;
    if (cursol.x > right)        return false;
    if (cursol.y < top)          return false;
    if (cursol.y > bottom)       return false;

    isAmongFlag = true;
    return true;
}

void Rect::setSelectColor()
{
    if (isAmongFlag)
    {
        color = { 1.0f,1.0f,1.0f,1.0f };
    }
    else
    {
        color = { 0.4f,0.4f,0.4f,1.0f };
    }

}
