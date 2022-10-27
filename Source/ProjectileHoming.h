#pragma once

#include "Graphics/model.h"
#include "Projectile.h"

class ProjectileManager;

// �ǔ��e��
class ProjectileHoming : public Projectile
{
public:
    ProjectileHoming(ProjectileManager* manager);
    ~ProjectileHoming();

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;
    void Render(const RenderContext& dc, ModelShader* shader) override;

    // ����
    void Launch(const DirectX::XMFLOAT3& direction, 
                const DirectX::XMFLOAT3& position,
                const DirectX::XMFLOAT3& target);

private:
    Model*              model = nullptr;
    DirectX::XMFLOAT3   target = { 0,0,0 };                                 // �N��_����
    float               moveSpeed = 10.0f;
    float               turnSpeed = DirectX::XMConvertToRadians(180);       // ��]���������Ȃ���
    float               lifeTimer = 3.0f;

};