#include "Rect.h"


// マウスが中にいるかどうか
Rect::Rect()
{
    texture = nullptr;
    sprite = new Sprite();   
}

Rect::Rect(const char* filename)
{
    texture = new Texture(filename);
    sprite = new Sprite();

    sprite->SetShaderResourceView(texture->GetShaderResourceView(), texture->GetWidth(), texture->GetHeight());
    imageSize.x = static_cast<float>(texture->GetWidth());
    imageSize.y = static_cast<float>(texture->GetHeight());

}

Rect::~Rect()
{
    Clear();
}


void Rect::SetSprite(const char* filename)
{
    Clear();
    texture = new Texture(filename);
    sprite = new Sprite();

    sprite->SetShaderResourceView(texture->GetShaderResourceView(), texture->GetWidth(), texture->GetHeight());
    imageSize.x = static_cast<float>(texture->GetWidth());
    imageSize.y = static_cast<float>(texture->GetHeight());
}

void Rect::SetSprite()
{
    Clear();
    texture = nullptr;
    sprite = new Sprite();
}

void Rect::Render(const RenderContext& rc, SpriteShader* shader)
{
    sprite->Update(position.x, position.y,
        size.x, size.y,
        0.0f, 0.0f,
        imageSize.x, imageSize.y,
        0.0f,
        color.x, color.y, color.z, color.w);

    RenderContext myRc = rc;
    shader->Begin(myRc);
    shader->Draw(myRc, sprite);
    shader->End(myRc);
}

void Rect::SetAll(float posX, float posY, float sizeX, float sizeY, float imageSizeX, float imageSizeY, float r, float g, float b, float a)
{
    position.x = posX;
    position.y = posY;

    size.x = sizeX;
    size.y = sizeY;

    imageSize.x = imageSizeX;
    imageSize.y = imageSizeY;
    
    color.x = r;
    color.y = g;
    color.z = b;
    color.w = a;
}

void Rect::Clear()
{
    if (sprite != nullptr)
    {
        delete sprite;
        sprite = nullptr;
    }
    if (texture != nullptr)
    {
        delete texture;
        texture = nullptr;
    }
}

void Rect::setSize(DirectX::XMFLOAT2 si, bool on)
{
    size = si;
    if (on)
    {
        imageSize = size;
    }
}


RectFade::RectFade() : Rect()
{
}

void RectFade::fadeInInit()
{
    if (fadeReady)   return;

    color.w = 0.0f;
    fadeReady = true;
}

void RectFade::fadeOutInit()
{
    if (fadeReady)   return;

    color.w = 1.0f;
    fadeReady = true;
}

// 画面を徐々に消していく
bool RectFade::fadeIn(float timer)
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
bool RectFade::fadeOut(float timer)
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

RectSelect::RectSelect() : Rect()
{
    isAmongFlag = false;
}

bool RectSelect::isAmong(DirectX::XMFLOAT2 cursol)
{
    isAmongFlag = false;

    float left = position.x;
    float right = position.x + size.x;
    float top = position.y;
    float bottom = position.y + size.y;

    if (cursol.x < left)         return false;
    if (cursol.x > right)        return false;
    if (cursol.y < top)          return false;
    if (cursol.y > bottom)       return false;

    isAmongFlag = true;
    return true;
}

void RectSelect::setSelectColor()
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

RectBar::RectBar() : Rect()
{
    baseTexture = nullptr;
    baseSprite = new Sprite();
}

RectBar::RectBar(const char* fileName) : Rect(fileName)
{
    baseTexture = nullptr;
    baseSprite = new Sprite();
}

RectBar::RectBar(const char* fileName, const char* baseFileName) : Rect(fileName)
{
    baseTexture = new Texture(baseFileName);
    baseSprite = new Sprite();

    baseSprite->SetShaderResourceView(baseTexture->GetShaderResourceView(), baseTexture->GetWidth(), baseTexture->GetHeight());
    imageSize.x = static_cast<float>(baseTexture->GetWidth());
    imageSize.y = static_cast<float>(baseTexture->GetHeight());
}

RectBar::~RectBar()
{
    Clear();

    if (baseSprite != nullptr)
    {
        delete baseSprite;
        baseSprite = nullptr;
    }
    if (baseTexture != nullptr)
    {
        delete baseTexture;
        baseTexture = nullptr;
    }
}

void RectBar::setPosition(DirectX::XMFLOAT2 pos)
{
    position = pos;

    basePos.x = position.x - baseSize.x;
    basePos.y = position.y - baseSize.y;
}

void RectBar::SetBaseAll(float baseSizeX, float baseSizeY,  float r, float g, float b, float a)
{
    basePos.x = position.x - baseSizeX;
    basePos.y = position.y - baseSizeX;
    baseSize.x = baseSizeX;
    baseSize.y = baseSizeY;

    baseColor = DirectX::XMFLOAT4(r, g, b, a);
}

void RectBar::culcValue(float currentVal, float maxVal)
{
    sizeValue =  static_cast<float>(currentVal) / maxVal;
}

void RectBar::Render(const RenderContext& rc, SpriteShader* shader)
{
    baseSprite->Update(basePos.x, basePos.y,
                       baseSize.x*2 + size.x, baseSize.y*2 + size.y,
                       0.0f, 0.0f,
                       baseImageSize.x, baseImageSize.y,
                       0.0f,
                       baseColor.x, baseColor.y, baseColor.z, baseColor.w);

    sprite->Update(position.x, position.y,
        size.x * sizeValue, size.y,
        0.0f, 0.0f,
        imageSize.x, imageSize.y,
        0.0f,
        color.x, color.y, color.z, color.w);

    RenderContext myRc = rc;
    shader->Begin(myRc);
    shader->Draw(myRc, baseSprite);
    shader->Draw(myRc, sprite);
    shader->End(myRc);

}
