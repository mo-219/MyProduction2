#pragma once

#include "Graphics/model.h"
#include "Projectile.h"

class ProjectileManager;

// ’Ç”ö’eŠÛ
class ProjectileHoming : public Projectile
{
public:
    ProjectileHoming(ProjectileManager* manager);
    ~ProjectileHoming();

    // XVˆ—
    void Update(float elapsedTime) override;

    // •`‰æˆ—
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;
    void Render(const RenderContext& dc, ModelShader* shader) override;

    // ”­Ë
    void Launch(const DirectX::XMFLOAT3& direction, 
                const DirectX::XMFLOAT3& position,
                const DirectX::XMFLOAT3& target);

private:
    Model*              model = nullptr;
    DirectX::XMFLOAT3   target = { 0,0,0 };                                 // ’N‚ğ‘_‚¤‚©
    float               moveSpeed = 10.0f;
    float               turnSpeed = DirectX::XMConvertToRadians(180);       // ‰ñ“]‚µ‚·‚¬‚ğ‚È‚­‚·
    float               lifeTimer = 3.0f;

};