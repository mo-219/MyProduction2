#include "ProjectileManager.h"
#include "Graphics/Graphics.h"

#include "Collision.h"


// �R���X�g���N�^
ProjectileManager::ProjectileManager()
{

}

// �f�X�g���N�^
ProjectileManager::~ProjectileManager()
{
    Clear();
}

// �X�V����
void ProjectileManager::Update(float elapsedTime)
{
    for (Projectile* projectile : projectiles)
    {
        projectile->Update(elapsedTime);
    }

    // �j������
    for (Projectile* projectile : removes)
    {
        // vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
        std::vector<Projectile*>::iterator it =     
                    std::find(projectiles.begin(), projectiles.end(), projectile);  // ������vector
        
        if (it != projectiles.end())
        {
            projectiles.erase(it);
        }
        // �e�ۂ̔j������
        delete projectile;
    }

    // �j�����X�g���N���A
    removes.clear();            //�����ŃN���A���Ȃ��Ƃ����Ə��������悤�Ƃ��Ă��܂�
}

// �`�揈��
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

//�f�o�b�O�v���~�e�B�u�`��
void ProjectileManager::DebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    for (Projectile* projectile : projectiles)
    {
        debugRenderer->DrawSphere(projectile->GetPosition(), projectile->GetRadius(), DirectX::XMFLOAT4(1, 0, 0, 1));
    }
}

// �e�ۓo�^
void ProjectileManager::Registar(Projectile* projectile)
{
    projectiles.emplace_back(projectile);
}


// �e�ۑS�폜
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


// �e�ۍ폜
void ProjectileManager::Remove(Projectile* projectile)
{
    // �j�����X�g�ɒǉ�
    removes.insert(projectile);
}