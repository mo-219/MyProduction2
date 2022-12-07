#include "Character.h"
#include "StageManager.h"
#include "GameObjectManager.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"


// �s��X�V����
void Character::UpdateTransform()
{

    
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);


    // ��]�s����쐬
    //DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);

    DirectX::XMMATRIX R = Y * X * Z;                    // ���w�x�[�X�ł� Z * X * Y


    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(param.position.x, param.position.y, param.position.z);


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

    // ����炭���[��Y�������ƂȂ�@���x�N�g��
    DirectX::XMFLOAT3 normal = { -0.015,1,0 };

    // ������
    if (my < 0.0f)
    {
        // ���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start = { param.position.x, param.position.y + stepOffset, param.position.z };
        // ���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end = { param.position.x, param.position.y + my, param.position.z };

        // ���C�L���X�g�ɂ��n�ʔ���
        HitResult hit;

        //Hack: �����ŋ��̓����蔻�聨���C�L���X�g���Ă��
        //if (StageManager::Instance().RayCast(start, end, hit))
        if (ObjectManager::Instance().RayCast(param, start, end, hit))
        {
            // �@���x�N�g���擾
            normal = hit.normal;

            // �n�ʂɐڒn���Ă���
            param.position = hit.position;

            // ��]
            angle.y += hit.rotation.y;

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
            param.position.y += my;
            isGround = false;
        }
    }

    // �㏸��
    else if (my > 0.0f)
    {
        param.position.y += my;
        isGround = false;
    }
    // �n�ʂ̌����ɉ����悤��XZ����]
    {
        float angleX, angleZ;
        angleX = atan2f(normal.z, normal.y);
        angleZ = -atan2f(normal.x, normal.y);

        // ���`�ۊǂŊ��炩�ɂ���
        angle.x = Mathf::Lerp(angle.x, angleX, 0.2f);
        angle.z = Mathf::Lerp(angle.z, angleZ, 0.2f);
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

    // �������͌v�Z
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        // �����ړ��l
        float mx = velocity.x * elapsedTime;
        float mz = velocity.z * elapsedTime;

        // ���C�̊J�n�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { param.position.x, param.position.y + stepOffset , param.position.z };
        DirectX::XMFLOAT3 end = { param.position.x + mx , param.position.y + stepOffset, param.position.z + mz };

        // ���C�L���X�g�ɂ��ǔ���
        HitResult hit;
        //if (StageManager::Instance().RayCast(start, end, hit))
        if (ObjectManager::Instance().RayCast(param, start, end, hit))
        {
            // �ǂ���I�_�܂Ńx�N�g��
            //DirectX::XMVECTOR HitPos = DirectX::XMLoadFloat3(&hit.position);

            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // �ǂ̖@��
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);
            //DirectX::XMVECTOR Inverse =DirectX::XMVectorScale(Normal, -1.0f);


            // ���˃x�N�g����@���Ɏˉe
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // �␳�ʒu�̌v�Z
            const float ofs = 0.001f;   // �߂荞�ݖh�~�ŏ����߂�
            Dot = DirectX::XMVectorAdd(Dot, DirectX::XMLoadFloat(&ofs));

            //DirectX::XMVECTOR V = DirectX::XMVectorMultiply(Normal, Dot);
            //DirectX::XMVECTOR CollectPosition = DirectX::XMVectorSubtract(Vec, V);
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);

            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            // ���C���Ⴄs�s�ɂ��ă`�F�b�N
            HitResult hit2;
            //if (!StageManager::Instance().RayCast(start, collectPosition, hit2))
            if (!ObjectManager::Instance().RayCast(param, start, collectPosition, hit2))
            {
                param.position.x = collectPosition.x;
                param.position.z = collectPosition.z;
            }
            else
            {
                param.position.x = hit2.position.x;
                param.position.z = hit2.position.z;
            }
        }
        else
        {
            // �ړ�
            param.position.x += mx;
            param.position.z += mz;
        }
    }
}
