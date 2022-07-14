#include "EnemyManager.h"
#include "Graphics/Graphics.h"

#include "Collision.h"


// 更新処理
void EnemyManager::Update(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }

    // 破棄処理
    for (Enemy* enemy : removes)
    {
        std::vector<Enemy*>::iterator it =
            std::find(enemies.begin(), enemies.end(), enemy);

        if (it != enemies.end())
        {
            enemies.erase(it);
        }
        // 弾丸の破棄処理
        delete enemy;
    }
    // 破棄リストをクリア
    removes.clear();

    // 敵同士の衝突判定
    CollisionEnemyVsEnemies();
}

// 描画処理
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(context, shader);
    }
}

// エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}

// エネミー全削除
void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)
    {
        if (enemy != nullptr)
        {
            delete enemy;
            enemy = nullptr;
        }
    }
    enemies.clear();
}

void EnemyManager::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// 衝突判定用のデバッグ球を描画
    //for (Enemy* enemy : enemies)
    //{
    //    debugRenderer->DrawSphere(enemy->GetPosition(), enemy->GetRadius(), DirectX::XMFLOAT4(1, 0, 0, 1));
    //}

    // 衝突判定用のデバッグ円柱を描画
    for (Enemy* enemy : enemies)
    {
        debugRenderer->DrawCylinder(enemy->GetPosition(), enemy->GetRadius(),enemy->GetHeight(), DirectX::XMFLOAT4(1, 0, 0, 1));
    }

}

void EnemyManager::CollisionEnemyVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突処理
    int enemyCount = enemyManager.GetEnemyCount();
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy1 = enemyManager.GetEnemy(j);

        for (int i = j+1; i < enemyCount; ++i)
        {
            if (i == j)  continue;

            DirectX::XMFLOAT3 outPosition{};
            Enemy* enemy2 = enemyManager.GetEnemy(i);

            //if (Collision::InstersectSphereVsSphere(enemy1->GetPosition(), enemy1->GetRadius(),
            //                                        enemy2->GetPosition(), enemy2->GetRadius(),
            //                                        outPosition))
            //{
            //    // 押し出し後の位置設定
            //    enemyManager.GetEnemy(i)->setPosition({ enemy2->GetPosition().x - outPosition.x,
            //                                            enemy2->GetPosition().y - outPosition.y,
            //                                            enemy2->GetPosition().z - outPosition.z });

            //}

            if (Collision::InstersectCylinderVsCylinder(enemy1->GetPosition(), enemy1->GetRadius(),enemy1->GetHeight(),
                                                    enemy2->GetPosition(), enemy2->GetRadius(), enemy2->GetHeight(),
                                                    outPosition))
            {
                // 押し出し後の位置設定
                enemyManager.GetEnemy(i)->setPosition({ outPosition.x,
                                                        outPosition.y,
                                                        outPosition.z });

            }
        }
    }
}

void EnemyManager::Remove(Enemy* enemy)
{
    // 破棄リストに追加
    removes.insert(enemy);
}

