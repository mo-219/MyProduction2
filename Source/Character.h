#pragma once

#include<DirectXMATH.h>

//�L�����N�^�[
class Character
{
public:
    Character() {}
    virtual ~Character() {}

    //  �_���[�W��^����
    bool ApplyDamage(int damage, float invincibleTime);
    //bool ApplyDamage(int damage);

protected:
    DirectX::XMFLOAT3   position = { 0,0,0 };
    DirectX::XMFLOAT3   angle = { 0,0,0 };
    DirectX::XMFLOAT3   scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 transform = {
        1,0,0,0,    // X���x�N�g��(�E�����x�N�g����x,y,z(�E�͂ǂ��������킩��))
        0,1,0,0,    // Y���x�N�g��(������̃x�N�g��)
        0,0,1,0,    // Z���x�N�g��(�O�����̃x�N�g��)
        0,0,0,1     // �ʒu
    };
    // �J�����̃����o��view�Ƃ͂܂�������Ƃ��������璍��
    //XM��directX��Math�̈Ӗ������˂�

public:
    //�s��X�V����
    void UpdateTransform();

    // �ʒu�K��
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    // �ʒu�ݒ�
    void setPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // ��]�擾
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }

    // ��]�ݒ�
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    // �X�P�[���擾
    const DirectX::XMFLOAT3& SetScale() const { return scale; }

    // �X�P�[���擾
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // ���a�擾
    float GetRadius() const { return radius; }

    // �n�ʂɐڒn���Ă��邩
    bool IsGround() const { return isGround; }

    // �����擾
    float GetHeight() const { return height; }



    // 09_ ���͏���
    // �������͍X�V����
    void UpdateVerticalVelocity(float elapsedTime);

    // �����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime);

    // �Ռ���^����
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

    int GetHealth() const { return health; }            // ���N��Ԃ��擾
    int GetMaxHealth() const { return maxHealth; }      // �ő匒�N��Ԃ��擾


protected:

    //void Move(float elapsedTime, float vx, float vz, float speed);    // �ړ�����

    void Move(float vx, float vz, float speed);

    void Turn(float elapsedTime, float vx, float vz, float speed);    // ���񏈗�

    void Jump(float speed);    // �W�����v����


    void UpdateVelocity(float elapsedTime);    // ���͏����X�V


    virtual void OnLanding() {}    // �ڒn�����Ƃ��ɂ�΂��

    virtual void OnDamaged() {}    // �_���[�W���󂯂��Ƃ��ɌĂ΂��

    virtual void OnDead() {}    // ���S�����Ƃ��ɌĂ΂��

    void UpdateInvincibleTimer(float elapsedTime);    // ���G���ԍX�V


private:
    void UpdateHorizontalVelocity(float elapsedFrame);  // �������͍X�V����

    void UpdateHorizontalMove(float elapsedTime);       // �����ړ��X�V����

protected:
    float               radius = 0.5f;

    float               gravity = -1.0f;          // 1�t���[���̏d��
    DirectX::XMFLOAT3   velocity = { 0,0,0 };     // ����
    bool                isGround = false;

    float               height = 2.0f;

    int                 health = 5;
    int                 maxHealth = 5;

    float               invincibleTimer = 0.0f;
    float               friction = 0.5f;    

    float               acceleration = 1.0f;
    float               maxMoveSpeed = 5.0f;
    float               moveVecX = 0.0f;
    float               moveVecZ = 0.0f;

    float               airControl = 0.3f;      // -�ɂ���������friction��acceleration��-�ɂȂ����Ƃ��l��
                                                // �v�����ʂ�ɂł��Ȃ���������Ȃ��̂Œ���

    float               stepOffset = 1.0f;
    float               slopeRate = 1.0f;

    // �V�F�[�_�[�p
    // �L���[�r�b�N�J���[
    DirectX::XMFLOAT4 colorTop1     = { 1.0f, 1.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorBottom1  = { 1.0f, 0.8f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorRight1   = { 1.0f, 0.0f, 0.8f, 1.0f };
    DirectX::XMFLOAT4 colorLeft1    = { 0.0f, 1.0f, 0.3f, 1.0f };
    DirectX::XMFLOAT4 colorBack1    = { 0.2f, 0.1f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 colorFront1   = { 1.0f, 0.0f, 0.0f, 1.0f };

    DirectX::XMFLOAT4 colorTop2     = { 1.0f, 1.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorBottom2  = { 1.0f, 0.8f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorRight2   = { 1.0f, 0.0f, 0.8f, 1.0f };
    DirectX::XMFLOAT4 colorLeft2    = { 0.0f, 1.0f, 0.3f, 1.0f };
    DirectX::XMFLOAT4 colorBack2    = { 0.2f, 0.1f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 colorFront2   = { 1.0f, 0.0f, 0.0f, 1.0f };

    DirectX::XMFLOAT4 colorAlpha    = { 1,1,1,0.5f };
};

//�������f���̋�����