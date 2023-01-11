#pragma once
#include <DirectXMath.h>
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Graphics/Graphics.h"

// 構造体替わり
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
    void setSize(DirectX::XMFLOAT2 si) { size = si; }             // 描画する大きさ
    void setSize(DirectX::XMFLOAT2 si, bool on);                  // 描画する大きさ onでimageSize = sizeにする
    void setImageSize(DirectX::XMFLOAT2 si) { imageSize = si; }
    void setColor(DirectX::XMFLOAT4 col) { color = col; }

public:
    Sprite* sprite;
    Texture* texture;

    // 描画位置
    DirectX::XMFLOAT2 position = {0,0};

    // 画像の描画開始位置
    DirectX::XMFLOAT2 imagePosition = {0,0};

    // 描画するサイズ
    DirectX::XMFLOAT2 size = {};              // size.x: 横幅   size.y: 縦幅
    
    // 画像自体のサイズ 
    DirectX::XMFLOAT2 imageSize = {};         // size.x: 横幅   size.y: 縦幅

    // 色
    DirectX::XMFLOAT4 color = {1,1,1,1};



};


// Fade用RECT
class RectFade : public Rect
{
public:
    RectFade();

    void fadeInInit();
    void fadeOutInit();

    bool fadeIn(float timer);          //colorが0になったら　true をかえす
    bool fadeOut(float timer);         //colorが1になったら　true をかえす

private:
    bool fadeReady = false;
};


// 選択肢用RECT
class RectSelect : public Rect
{
public:
    RectSelect();
    bool isAmong(DirectX::XMFLOAT2 cursol);
    void setSelectColor();

private:
    bool isAmongFlag = false;
};


// UIBar用Rect
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
    DirectX::XMFLOAT2 basePos = {0,0};      // 下地の幅
    DirectX::XMFLOAT2 baseImageSize = {2,2};      // 下地の幅
    DirectX::XMFLOAT2 baseSize = {2,2};      // 下地の幅
    DirectX::XMFLOAT4 baseColor = {0,0,0,1};     // 下地の色

};