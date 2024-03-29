#pragma once
#include <vector>
#include <set>
#include "Projectile.h"
#include "Graphics/RenderContext.h"

// 弾丸マネージャー
class ProjectileManager
{
public:
    ProjectileManager();
    ~ProjectileManager();       // 弾丸を撃つのは王レイヤーだけでなければエネミーだけでないのでそれぞれにマネージャーを作れるように
                                // シングルトンにはしない

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Render(const RenderContext& dc, ModelShader* shader);

    // デバッグプリミティブ
    void DebugPrimitive();

    // 弾丸登録
    void Registar(Projectile* projectile);

    // 弾丸全削除
    void Clear();

    // 弾丸数取得
    int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }

    // 弾丸取得
    Projectile* GetProjectile(int index) { return projectiles.at(index); }

    // 弾丸削除
    void Remove(Projectile* projectile);

private:
    std::vector<Projectile*>    projectiles;
    std::set<Projectile*>       removes;
    //std::vector<Projectile*>    removes;

};