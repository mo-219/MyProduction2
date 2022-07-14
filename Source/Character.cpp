#include"Character.h"
#include "Stage.h"

// �s��X�V����
void Character::UpdateTransform()
{

    
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);


    // ��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);


    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);


    // �R�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;    //����n


    // ���̏��Ԃɂ���ē����͕ς��  ���ꂾ���Ƃ͌���Ȃ�
    // DirectX::XMMATRIX W = S * T1 * R * T2 * T;
    // 3D�Q�[����8���͂��̂܂܂ő��v�@�@��肽�����Ԃɏ���

    // �E��n�̏ꍇ
    // DirectX::XMMATRIX W = T * R * S;    //����n
    // �|���Z���t




    // �v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}


// �ړ�����
//void Character::Move(float elapsedTime, float vx, float vz, float speed)
void Character::Move(float vx, float vz, float speed)
{
    //speed *= elapsedTime;
    //position.x += vx * speed;
    //position.z += vz * speed;

    // �ړ������x�N�g����ݒ�
    moveVecX = vx;
    moveVecZ = vz;

    // �ő呬�x�ݒ�
    maxMoveSpeed = speed;
}

// ���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;

    DirectX::XMFLOAT3 vec1;
    vec1.x = vx;
    vec1.z = vz;
    float vec = (vec1.x * vec1.x + vec1.z * vec1.z);        //��悵�������ł�0�x�N�g���̔���ł���

    // �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
    if (vec < 0.00001f)     return;

    vec = sqrtf(vec1.x * vec1.x + vec1.z * vec1.z);         // ���肵����ɐ��K��

    // �i�s�x�N�g����P�ʃx�N�g����
    vec1.x /= vec;        // �i�s�x�N�g���̒P�ʃx�N�g��
    vec1.z /= vec;        // �i�s�x�N�g���̒P�ʃx�N�g��

    // ���g�̉�]�l����O���������߂�
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    vec = sqrtf(frontX * frontX + frontZ * frontZ);
    frontX /= vec;
    frontZ /= vec;


    float dot = (vec1.x * frontX) + (vec1.z * frontZ);    // ����
    float rot = 1.0f - dot;                               // �␳�l
    if (rot > speed) rot = speed;                         // rot���p�x���ۂ��Ȃ�    rot�͊p�x����Ȃ���I�I�I�I�I�I�I�I�I�I�I

    // ���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (frontZ * vec1.x) - (frontX * vec1.z);

    // 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
    // ���E������s�����Ƃɂ���č��E��]��I������
    if (cross < 0.0f)
    {
        angle.y -= atanf(rot);
    }
    else
    {
        angle.y += atanf(rot);

    }
}

// �W�����v����
void Character::Jump(float speed)
{
    // ������̗͂�ݒ�
    velocity.y += speed;
}

// ���͏����X�V
void Character::UpdateVelocity(float elapsedTime)      // ��ŐF�X�ς��邱��
{

#if 0
    // �d�͏���
    velocity.y += gravity * elapsedFrame;           // ���t���[���łȂ�����

    // �ړ�����
    position.y += velocity.y * elapsedTime;

    // �n�ʔ���
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        velocity.y = 0.0f;

        if (!IsGround())    // OnLanding�֐������x���Ă΂�Ȃ��悤��
        {
            OnLanding();
        }
        isGround = true;
    }
    else
    {
        isGround = false;
    }
#endif

    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;

    // ���͍X�V
    UpdateVerticalVelocity(elapsedFrame);    // �������͍X�V����
    UpdateHorizontalVelocity(elapsedFrame);  // �������͍X�V����

    // �ړ��X�V
    UpdateVerticalMove(elapsedTime);         // �����ړ��X�V����
    UpdateHorizontalMove(elapsedTime);       // �����ړ��X�V����

}

// �_���[�W��^����
bool Character::ApplyDamage(int damage, float invincibleTime)
{
    if (invincibleTimer > 0.0f) return false;           // ���O�����Ă�̂Ŕ��ʂ��₷���悤��this->invincibleTimer�Ə����Ă��悢
    // �_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
    if (damage <= 0)    return false;

    // ���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
    if (health <= 0)    return false;

    // �_���[�W����
    health -= damage;

    // ���S�ʒm
    if (health <= 0)
    {
        OnDead();
    }
    else
    {
        // �_���[�W�ʒm
        OnDamaged();
        invincibleTimer = invincibleTime;
    }

    // ���N��Ԃ��ύX���ꂽ�ꍇtrue��Ԃ�
    return true;
}

void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
        if (invincibleTimer <= 0.0f)    invincibleTimer = 0.0f;
    }
}

// �������͍X�V����
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
    // �d�͏���
    velocity.y += gravity * elapsedFrame;           // ���t���[���łȂ�����

}

// �����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
#if 0
    // �ړ�����
    position.y += velocity.y * elapsedTime;

    // �n�ʔ���
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        velocity.y = 0.0f;

        if (!IsGround())    // OnLanding�֐������x���Ă΂�Ȃ��悤��
        {
            OnLanding();
        }
        isGround = true;
    }
    else
    {
        isGround = false;
    }
#endif

    // ���������̈ړ���
    float my = velocity.y * elapsedTime;

    slopeRate = 0.0f;

    // ������
    if (my < 0.0f)
    {
        // ���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // ���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

        // ���C�L���X�g�ɂ��n�ʔ���
        HitResult hit;

        if (Stage::Instance().RayCast(start, end, hit))
        {
            // �n�ʂɐڒn���Ă���
            position.y = hit.position.y;

            // �X�Η��̌v�Z
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            // ���n����
            if (!isGround)
            {
                OnLanding();
            }

            velocity.y = 0.0f;
            isGround = true;
        }
        else
        {
            // �󒆂ɕ����Ă���
            position.y += my;
            isGround = false;
        }
    }

    // �㏸��
    else if (my > 0.0f)
    {
        position.y += my;
        isGround = false;
    }
}

// �Ռ���^����
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    // ���͂ɗ͂�������
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;

}


// �������͍X�V����
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
    // XZ���ʂ̑��͂���������
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        // ���C��
        float friction = this->friction * elapsedFrame;
        // �����̍ގ����ς��Ζ��C�͂̑傫�����ω�����

        // �󒆂ɂ���Ƃ��͖��C�͂����炷
        if (!isGround)   friction *= airControl;

        // ���C�ɂ�鉡�����̌�������
        if (length > friction)
        {
            float vx = velocity.x / length;
            float vz = velocity.z / length;

            //length -= friction;
            velocity.x -= vx*friction;
            velocity.z -= vz*friction;
        }
        // �������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
        else
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    // XZ���ʂ̑��͂���������
    if (length <= maxMoveSpeed)
    {
        // �ړ��x�N�g�����[���x�N�g���łȂ��Ȃ�(�X�e�B�b�N�̌X�����[���łȂ��Ȃ�)
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
        if (moveVecLength > 0.0f)
        {
            // ������
            float acceleration = this->acceleration * elapsedFrame;
            // �����̍ގ����ς��Ή����͂̑傫�����ω�����

            // �󒆂ɂ���Ƃ��͉����͂����炷
            if (!isGround)   acceleration *= airControl;        

            // �ړ��x�N�g���ɂ���������
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            // �ő呬�x����
            
            length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                float vx = velocity.x / length;
                float vz = velocity.z / length;

                velocity.x = vx * maxMoveSpeed;
                velocity.z = vz * maxMoveSpeed;
            }

            // �����ŃK�^�K�^���Ȃ��悤�ɂ���
            if (isGround && slopeRate > 0.0f)
            {
                velocity.y -= (length * slopeRate) * elapsedFrame;
            }

        }

    }

    // �ړ��x�N�g�������Z�b�g
    // �Ō��moveVec��0�ɂ���
    moveVecX = 0.0f;
    moveVecZ = 0.0f;
}

// �����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;
}
