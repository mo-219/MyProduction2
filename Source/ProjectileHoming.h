#pragma once

#include "Graphics/model.h"
#include "Projectile.h"

class ProjectileManager;

// 追尾弾丸
class ProjectileHoming : public Projectile
{
public:
    ProjectileHoming(ProjectileManager* manager);
    ~ProjectileHoming();

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    // 発射
    void Launch(const DirectX::XMFLOAT3& direction, 
                const DirectX::XMFLOAT3& position,
                const DirectX::XMFLOAT3& target);

private:
    Model*              model = nullptr;
    DirectX::XMFLOAT3   target = { 0,0,0 };                                 // 誰を狙うか
    float               moveSpeed = 10.0f;
    float               turnSpeed = DirectX::XMConvertToRadians(180);       // 回転しすぎをなくす
    float               lifeTimer = 3.0f;

};