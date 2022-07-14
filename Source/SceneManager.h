#pragma once
#include "Scene.h"

// シーンマネージャー
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}

public:
    // 唯一のインスタンス取得
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    void Update(float elapsedTime);        // 更新処理
    void Render();                         // 描画処理
    void Clear();                          // シーンクリア
    void ChangeScene(Scene* scene);        // シーン切り替え

private:
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
};
