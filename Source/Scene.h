#pragma once

// シーン
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    virtual void Initialize() = 0;                     // 初期化
    virtual void Finalize() = 0;                       // 終了化
    virtual void Update(float elapsedTime) = 0;        // 更新処理
    virtual void Render() = 0;                         // 描画処理

    bool IsReady() const { return ready; }             // 準備完了しているか
    void SetReady() { ready = true; }                  // 準備完了設定

private:
    bool ready = false;
};
