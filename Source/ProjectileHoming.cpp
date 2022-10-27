#include "ProjectileHoming.h"

ProjectileHoming::ProjectileHoming(ProjectileManager* mana) : Projectile(mana)
{
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
    model = new Model("Data/Model/Sword/Sword.mdl");

    // �\���T�C�Y�𒲐�
    //scale = { 0.5f, 0.5f, 0.5f };
    scale = { 3.0f, 3.0f, 3.0f };
}

ProjectileHoming::~ProjectileHoming()
{
    delete model;
}


// �X�V����
void ProjectileHoming::Update(float elapsedTime)
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
    {
        float speed = this->moveSpeed * elapsedTime;

        position.x += direction.x * speed;
        position.y += direction.y * speed;
        position.z += direction.z * speed;
    }

    // ����
    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        // �^�[�Q�b�g�܂ł̃x�N�g�����Z�o
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // �[���x�N�g���łȂ��Ȃ��]����
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3Length(Vec);
        float lengthSq = DirectX::XMVectorGetX(LengthSq);

        if (lengthSq > 0.00001f)        // float�͉��������������ƌ덷��������@���̌덷��
        {
            // �^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����
            Vec = DirectX::XMVector3Normalize(Vec);

            // �����Ă�������x�N�g�����Z�o
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            // �O�����x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓���(�p�x)���Z�o   �I���������̂������������ǂł͂Ȃ��I�I
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            // 2�̒P�ʃx�N�g���̊p�x���������ق�
            // 1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
            float rot = 1.0f - dot;
            if (rot > turnSpeed) rot = turnSpeed;


            // ��]�p�x������Ȃ��]����������
            if (fabs(rot) > 0.00001f)          //fabs()����͉̂�]�p�x���}�C�i�X�ɂȂ邱�Ƃ����邩��
            {
                // ��]�����Z�o
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);      

                // Axis��Normalize(�P�ʃx�N�g���ɂ��邽��)
                Axis = DirectX::XMVector3Normalize(Axis);

                // ��]���Ɖ�]�ʂ����]�s����Z�o
                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                // ���݂̍s�����]������
                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                // ��]��̑O�����������o���A�P�ʃx�N�g��������
                Direction = DirectX::XMVector3Normalize(Transform.r[2]);
                DirectX::XMStoreFloat3(&direction, Direction);

            }
        }



    }

    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���s��X�V
    model->UpdateTransform(transform);
}

// �`�揈��
void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void ProjectileHoming::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction,
                              const DirectX::XMFLOAT3& position,
                              const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}