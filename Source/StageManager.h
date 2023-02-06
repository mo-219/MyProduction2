#pragma once
#include <vector>
#include <set>
#include "Stage.h"

// ステージマネージャー
class StageManager
{
private:
    StageManager() {}
    ~StageManager() {}

public:
    static StageManager& Instance()
    {
        static StageManager instance;
        return instance;
    }

    //更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(RenderContext& rc, ModelShader* shader);

    // レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    // ステージ登録
    void Register(Stage* stage);

    // ステージ削除
    void Remove(Stage* stage);

    // ステージ全削除
    void Clear();

    // 特定ステージの全削除
    void StageNumDelete(int type);

    // ステージ数ゲッター
    int GetStageCount() const { return static_cast<int>(stages.size()); }

    // エネミー取得
    Stage* GetStage(int index) { return stages.at(index); }

    // デバッグ用GUI描画
    void DrawDebugGUI();

    // 当たり判定の描画
    void DrawDebugPrimitive();



private:
    std::vector<Stage*> stages;
    std::set<Stage*>    removes;



};