#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Scene.h"

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
    std::unique_ptr<Texture>	texture;
    std::unique_ptr<Sprite>		sprite;

    DirectX::XMFLOAT2 renderPosition = {};
};

#endif /* SCENE_TITLE_H */