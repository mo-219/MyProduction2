#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"

// ���i�e��
class ProjectileStraight : public Projectile
{
public:
    ProjectileStraight(ProjectileManager* mana);
    ~ProjectileStraight() override;

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // ����
    void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
    Model* model = nullptr;
    float  speed = 10.0f;

    float lifeTimer = 3.0f;
};