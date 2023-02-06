#pragma once
#include <thread>

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Rect.h"

// ローディングシーン
class SceneLoading : public Scene
{
//　メンバ関数
public:
    SceneLoading(Scene* next) : nextScene(next) {}
    ~SceneLoading() override {}

    void Initialize() override;
    void Finalize() override;
    void Update(float elapsedTime) override;
    void Render() override;

    void DotUpdate(float elapsedTime);

private:
    static void LoadingThread(SceneLoading* scene);     // ローディングスレッド

// メンバ変数
private:
    Sprite*                     sprite = nullptr;

    float                       angle = 0.0f;

    Scene*                      nextScene = nullptr;
    std::thread*                thread = nullptr;

    int                         state = 0;
    int                         dotState = 0;

    RectFade*                   fade = nullptr;

    float                       standertPos = 0;
    DirectX::XMFLOAT2           dotPos[3];
    int                         currentNum = 0;


};
