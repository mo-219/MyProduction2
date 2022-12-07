#pragma once
#include <DirectXMath.h>
#include "Graphics/Sprite.h"

// \‘¢‘Ì‘Ö‚í‚è
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

    bool fadeIn(float timer);          //color‚ª0‚É‚È‚Á‚½‚ç@true ‚ğ‚©‚¦‚·
    bool fadeOut(float timer);         //color‚ª1‚É‚È‚Á‚½‚ç@true ‚ğ‚©‚¦‚·
public:
    Sprite* sprite;

    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 size;         // size.x: ‰¡•   size.y: c•
    DirectX::XMFLOAT4 color;

    bool isAmongFlag = false;
    bool fadeReady = false;

};
