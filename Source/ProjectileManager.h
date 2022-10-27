#pragma once
#include <vector>
#include <set>
#include "Projectile.h"
#include "Graphics/RenderContext.h"

// �e�ۃ}�l�[�W���[
class ProjectileManager
{
public:
    ProjectileManager();
    ~ProjectileManager();       // �e�ۂ����͉̂����C���[�����łȂ���΃G�l�~�[�����łȂ��̂ł��ꂼ��Ƀ}�l�[�W���[������悤��
                                // �V���O���g���ɂ͂��Ȃ�

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Render(const RenderContext& dc, ModelShader* shader);

    // �f�o�b�O�v���~�e�B�u
    void DebugPrimitive();

    // �e�ۓo�^
    void Registar(Projectile* projectile);

    // �e�ۑS�폜
    void Clear();

    // �e�ې��擾
    int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }

    // �e�ێ擾
    Projectile* GetProjectile(int index) { return projectiles.at(index); }

    // �e�ۍ폜
    void Remove(Projectile* projectile);

private:
    std::vector<Projectile*>    projectiles;
    std::set<Projectile*>       removes;
    //std::vector<Projectile*>    removes;

};