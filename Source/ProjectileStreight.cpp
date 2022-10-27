#include "ProjectileStraight.h"
#include "ProjectileManager.h"

ProjectileStraight::ProjectileStraight(ProjectileManager* mana) : Projectile(mana)
{
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
    model = new Model("Data/Model/Sword/Sword.mdl");

    // �\���T�C�Y�𒲐�
    //scale = { 0.5f, 0.5f, 0.5f };
    scale = { 3.0f, 3.0f, 3.0f };
}

ProjectileStraight::~ProjectileStraight()
{
    delete model;
}


// �X�V����
void ProjectileStraight::Update(float elapsedTime)
{
    // ��������
    lifeTimer -= elapsedTime;
    if (lifeTimer < 0)
    {
        // �������폜
        Destroy();
        return;             // �������牺�̏������K�v���Ȃ�����    �܂��f�X�g���C�\�񂵂Ă��牽������̏�������������|�� 
    }


    // �ړ�
    float speed = this->speed * elapsedTime;

    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;

    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���s��X�V
    model->UpdateTransform(transform);
}


// �`�揈��
void ProjectileStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void ProjectileStraight::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}


// ����
void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction,
                                const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}