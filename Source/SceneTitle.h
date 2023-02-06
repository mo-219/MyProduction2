#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Scene.h"

#include "Rect.h" 

#include "Audio/AudioSource.h"
//#include "DXTKAudio.h"

// タイトルシーン
class SceneTitle : public Scene
{
public:
    SceneTitle() {}
    ~SceneTitle() override {}


    void Initialize() override;                  // 初期化
    void Finalize() override;                    // 終了化
    void Update(float elapsedTime) override;     // 更新処理
    void Render() override;                      // 描画処理

private:
    RectFade*                   fade = nullptr;         // 画面のフェード用 
    int                         state = 0;              // Update遷移用
    bool                        changeScene = false;    // 画面遷移フラグ

    std::unique_ptr<Texture>	texture;
    std::unique_ptr<Sprite>		sprite;

    std::unique_ptr<Texture>	pressTexture;
    std::unique_ptr<Sprite>		pressSprite;

    DirectX::XMFLOAT2 renderPosition = {};

    bool                        alphaFlag = true;
    float                       alpha = 1.0f;

    std::unique_ptr<AudioSource> bgm;
    std::unique_ptr<AudioSource> se;

};

#endif /* SCENE_TITLE_H */