#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"
#include "MetaAI.h"

class Player;

// �S�[����
class EnemyBoss : public Enemy
{
public:
    EnemyBoss();
    //EnemySlime(EnemyManager* mane) : Enemy(mane);
    ~EnemyBoss() override;

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

    // ���^AI���烁�b�Z�[�W����M
    bool OnMessage(const Telegram& msg)override;


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


    // �U���J�n�X�e�[�g
    void TransitionAttackInitState();                // �J��
    void UpdateAttackInitState(float elapsedTime);   // �X�V����

    // �U��1�X�e�[�g
    void TransitionAttack1State();                   // �J��
    void UpdateAttack1State(float elapsedTime);      // �X�V����

    // �U��2�X�e�[�g
    void TransitionAttack2State();                   // �J��
    void UpdateAttack2State(float elapsedTime);      // �X�V����

    // �퓬�ҋ@�X�e�[�g
    void TransitionIdleBattleState();                // �J��
    void UpdateIdleBattleState(float elapsedTime);   // �X�V����

    // �_���[�W�X�e�[�g
    void TransitionDamageState();                    // �J��
    void UpdateDamageState(float elapsedTime);       // �X�V����

    // �_���[�W�X�e�[�g
    void TransitionDeathState();                     // �J��
    void UpdateDeathState(float elapsedTime);        // �X�V����



private:
    Model* model = nullptr;

    // �X�e�[�g
    enum class State
    {
        Wander,
        Idle,
        Pursuit,
        AttackInit,
        Attack,
        IdleBattle,
        Damage,
        Death,
        MAX
    };
    // �A�j���[�V����
    enum Animation
    {
        Anim_TPose,
        Anim_Attack1,
        Anim_Attack2,
        Anim_CastSpell,
        Anim_Defend,
        Anim_Die,
        Anim_Idle,
        Anim_Jump,
        Anim_BackRun,
        Anim_ForwardRun,
        Anim_Damage,
        Anim_BackWalk,
        Anim_ForwardWalk,
        Anim_Max
    };

    State               state = State::Wander;

    float               moveSpeed = 1.0f;
    float               turnSpeed = DirectX::XMConvertToRadians(360);

    float               stateTimer = 0.0f;

    float               fovCos = cosf(DirectX::XMConvertToRadians(45));
};


