#include "ProjectileManager.h"
#include "Graphics/Graphics.h"

#include "Collision.h"


// コンストラクタ
ProjectileManager::ProjectileManager()
{

}

// デストラクタ
ProjectileManager::~ProjectileManager()
{
    Clear();
}

// 更新処理
void ProjectileManager::Update(float elapsedTime)
{
    for (Projectile* projectile : projectiles)
    {
        projectile->Update(elapsedTime);
    }

    // 破棄処理
    for (Projectile* projectile : removes)
    {
        // vectorから要素を削除する場合はイテレーターで削除しなければならない
        std::vector<Projectile*>::iterator it =     
                    std::find(projectiles.begin(), projectiles.end(), projectile);  // ここはvector
        
        if (it != projectiles.end())
        {
            projectiles.erase(it);
        }
        // 弾丸の破棄処理
        delete projectile;
    }

    // 破棄リストをクリア
    removes.clear();            //ここでクリアしないとずっと消し続けようとしてしまう
}

// 描画処理
void ProjectileManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Projectile* projectile : projectiles)
    {

        projectile->Render(dc, shader);
        DebugPrimitive();
    }
}

void ProjectileManager::Render(const RenderContext& rc, ModelShader* shader)
{
    for (Projectile* projectile : projectiles)
    {

        projectile->Render(rc, shader);
        DebugPrimitive();
    }
}

//デバッグプリミティブ描画
void ProjectileManager::DebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    for (Projectile* projectile : projectiles)
    {
        debugRenderer->DrawSphere(projectile->GetPosition(), projectile->GetRadius(), DirectX::XMFLOAT4(1, 0, 0, 1));
    }
}

// 弾丸登録
void ProjectileManager::Registar(Projectile* projectile)
{
    projectiles.emplace_back(projectile);
}


// 弾丸全削除
void ProjectileManager::Clear()
{
    for (Projectile* projectile : projectiles)
    {
        if (projectile != nullptr)
        {
            delete projectile;
            projectile = nullptr;
        }
    }
    projectiles.clear();
}


// 弾丸削除
void ProjectileManager::Remove(Projectile* projectile)
{
    // 破棄リストに追加
    removes.insert(projectile);
}