#include "EnemySlime.h"

// �R���X�g���N�^
EnemySlime::EnemySlime()
{
    model = new Model("Data/model/Slime/Slime.mdl");

    // ���f�����傫���̂ŃX�P�[�����O
    //scale.x = scale.y = scale.z = 0.001f;
    scale = { 0.01f, 0.01f, 0.01f };
    height = 1.0f;
}

// �f�X�g���N�^
EnemySlime:: ~EnemySlime()
{
    delete model;
}

// �X�V����
void EnemySlime::Update(float elapsedTime)
{
    // ���͏���
    UpdateVelocity(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);
    
    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���s��X�V
    model->UpdateTransform(transform);
}

// �`�揈��
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)    // ID3D11DeviceContext��dc�ɂȂ��Ă�@�G���[�o����context�ɕύX
{
    shader->Draw(dc, model);
}

// ���S����
void EnemySlime::OnDead()
{
    Destroy();
}