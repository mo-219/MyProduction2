#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

class Player;

// �X���C��
class EnemySlime : public Enemy
{
public:
    EnemySlime();
    //EnemySlime(EnemyManager* mane) : Enemy(mane);
    ~EnemySlime() override;

    // �X�V����
    void Update(float elapsedTime) override;
    void UpdateOnlyTransform(float elapsedTime)override;

    // �`�揈��
    void Render(const RenderContext& rc, ModelShader* shader) override;
    RenderContext SetRenderContext(const RenderContext& rc) override;

    // �f�o�b�O�pGUI�`��
    void DrawDebugImGui();
    void DrawDebugPrimitive() override;


    // �m�[�h�ƃv���C���[�̏Փ�
    void CollisionNodeVsPlayer(const char* nodeName, float nodeRadius);


protected:
    // ���S�����Ƃ��ɌĂ΂��
    void OnDead()override;

    // �_���[�W���󂯂��Ƃ��ɌĂ΂��
    void OnDamaged()override;

    // �^�[�Q�b�g�ʒu�������_���ݒ�
    void SetRamdomTargetPosition();

    // �ڕW�n�_�ֈړ�
    void MoveToTarget(float elapsedTime, float speedRate);

    // �v���C���[���G
    bool SearchPlayer();

    Player* searchPlayer(); // �v���C���[�������ɂȂ������������v���C���[��Ԃ�


    //---------------------------------------
    //
    //      �X�e�[�g�p
    //
    //---------------------------------------

    // �p�j�X�e�[�g
    void TransitionWanderState();                    // �J��
    void UpdateWanderState(float elapsedTime);       // �X�V����

    // �ҋ@�X�e�[�g
    void TransitionIdleState();                      // �J��
    void UpdateIdleState(float elapsedTime);         // �X�V����

    // �ǐՃX�e�[�g
    void TransitionPursuitState();                   // �J��
    void UpdatePursuitState(float elapsedTime);      // �X�V����

    // �U���X�e�[�g
    void TransitionAttackState();                    // �J��
    void UpdateAttackState(float elapsedTime);       // �X�V����

    // �퓬�ҋ@�X�e�[�g
    void TransitionIdleBattleState();                // �J��
    void UpdateIdleBattleState(float elapsedTime);   // �X�V����

    // �_���[�W�X�e�[�g
    void TransitionDamageState();                // �J��
    void UpdateDamageState(float elapsedTime);   // �X�V����

    // �_���[�W�X�e�[�g
    void TransitionDeathState();                // �J��
    void UpdateDeathState(float elapsedTime);   // �X�V����



private:
    Model* model = nullptr;

    // �X�e�[�g
    enum class State
    {
        Wander,
        Idle,
        Pursuit,
        Attack,
        IdleBattle,
        Damage,
        Death,
        MAX
    };
    // �A�j���[�V����
    enum Animation
    {
        Anim_IdleNormal,
        Anim_IdleBattle,
        Anim_Attack1,
        Anim_Attack2,
        Anim_WalkFWD,
        Anim_WalkBWD,
        Anim_WalkLeft,
        Anim_WalkRight,
        Anim_RunFWD,
        Anim_SenseSomthingST,
        Anim_SenseSomthingPRT,
        Anim_Taunt,
        Anim_Victory,
        Anim_GetHit,
        Anim_Dizzy,
        Anim_Die
    };

    State               state = State::Wander;




    float               moveSpeed = 3.0f;
    float               turnSpeed = DirectX::XMConvertToRadians(360);

    float               stateTimer = 0.0f;
    float               searchRange = 5.0f;

    float               attackRange = 1.5f;

    float               fovCos = cosf(DirectX::XMConvertToRadians(45));
};

