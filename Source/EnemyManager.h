#pragma once

#include <vector>
#include <set>
#include "Enemy.h"
#include <DirectXMath.h>

// エネミーマネージャー
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager();

public:
    // 唯一のインスタンス取得
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);
    void UpdateOnlyTransform(float elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* context, Shader* shader);
    void Render(const RenderContext& rc, ModelShader* shader);

    // エネミー登録
    void Register(Enemy* enemy);

    // エネミー削除
    void Remove(Enemy* enemy);

    // エネミー全削除
    void Clear();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    void DrawDebugGUI();

    // エネミー数取得
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

    // エネミー取得
    Enemy* GetEnemy(int index) { return enemies.at(index); }

private:
    // プレイヤーとエネミーの衝突処理
    void CollisionEnemyVsEnemies();

private:
    std::vector<Enemy*> enemies;
    std::set<Enemy*>    removes;
};



// エネミーアニメーション
//
//
//
//  Blue、Red
// 
//  Tpose,
//  Attack1,
//  Attack2,
//  Die,
//  Hit,
//  Idle,
//  Jump,
//  Running,
//  Walking,
//
//
//
//  Spider
//  Tpose,
//  Attack1,
//  Attack2,
//  Cast_Spell,
//  Defend,
//  Die,
//  Idle,
//  Jump,
//  Run,
//  Hit,
//  Walk
//
//
//
//
//
//
//
