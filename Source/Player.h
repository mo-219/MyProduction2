#pragma once

#include "Graphics/shader.h"
#include "Graphics/Model.h"
#include "Character.h"

#include "ProjectileManager.h"
#include "Sword.h"

#include "Effect.h"

//�v���C���[
class Player : public Character
{
public:
    // �C���X�^���X�擾
    static Player& Instance();

    Player();
    ~Player();

    // ������
    void Initialize(DirectX::XMFLOAT3 Respawn);

    // �X�V����
    void Update(float elapsedTime);

    void UpdateOnlyTransform(float elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Render(const RenderContext& rc, ModelShader* shader);

    // �f�o�b�O�pGUI�`��
    void DrawDebugGUI();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �W�����v���͏���
    bool ImputJump();

    // �U�����͏���
    bool InputAttack();

    bool JudgeAttack();

    // �U�����͏���
    bool InputDodge();

    bool CalcDodge();

    void SetStageClearFlag(bool b) { stageClearFlag = b; }
    bool GetStageClearFlag() { return stageClearFlag; }

    bool GetDodgeFlag() { return dodgeFlag; }

protected:
    // �ڒn�����Ƃ��ɌĂ΂��
    void OnLanding() override;

    // �_���[�W���󂯂��Ƃ��ɌĂ΂��
    void OnDamaged() override;

    // ���S�����Ƃ��ɌĂ΂��
    void OnDead()override;

    void Attacked();

private:
    // �X�e�B�b�N���͒l����ړ��x�N�g�����擾
    DirectX::XMFLOAT3 GetMoveVec() const;


    // �ړ����͏���
    bool InputMove(float elapsedTime);


    // �v���C���[�ƃG�l�~�[�̏Փˏ���
    void CollisionPlayerVsEnemies();

    // �e�ۂƓG�̏Փˏ���
    void CollisionProjectilesVsEnemies();

    void CollisionPlayerVsStage();

    void CollisionPlayerVsObject();

    // �e�ۓ��͏���
    void InputProjectile();

private:
    // �A�j���[�V����
    enum Animation
    {
        Anim_Error = -1,
        Anim_Combo1,
        Anim_Combo2,
        Anim_Combo3,
        Anim_Combo4,
        Anim_Death,
        Anim_Dodge,
        Anim_HitBack,
        Anim_HitFront,
        Anim_Idle,
        Anim_Landing_Long,
        Anim_Landing,
        Anim_Jump_Down1,
        Anim_Jump_Down2,
        Anim_Jump_Up1,
        Anim_Jump_Up2,
        Anim_Jump_Peak,
        Anim_Running,


        Anim_Max
    };
    // �X�e�[�g
    enum class State
    {
        Idle,
        Move,
        Jump,
        Attack,
        Dodge,
        Falling,
        Landing,

        Max
    };

    //--------------------------------
    // 
    //      �A�j���[�V����
    // 
    //--------------------------------
    
    // �A�j���[�V�����X�e�[�g 
    // �ҋ@�X�e�[�g
    void TransitionIdleState();                 // �J��
    void UpdateIdleState(float elapsedTime);    // �X�V����

    // �ړ��X�e�[�g
    void TransitionMoveState();                 // �J��
    void UpdateMoveState(float elapsedTime);    // �X�V����

    // �W�����v�X�e�[�g
    void TransitionJumpInitState();                 // �J��
    void UpdateJumpInitState(float elapsedTime);    // �X�V����

    // �W�����v�X�e�[�g
    void TransitionJumpState();                 // �J��
    void UpdateJumpState(float elapsedTime);    // �X�V����

    // ���n�X�e�[�g
    void TransitionLandingState();              // �J��
    void UpdateLandingState(float elapsedTime); // �X�V����

    // �����X�e�[�g
    void TransitionFallingState();              // �J��
    void UpdateFallingState(float elapsedTime); // �X�V����

    // �U���X�e�[�g
    void TransitionAttackState();               // �J��
    void UpdateAttackState(float elapsedTime);  // �X�V����

    // �����X�e�[�g
    void TransitionDodgeState();               // �J��
    void UpdateDodgeState(float elapsedTime);  // �X�V����

     
    //---------------------------------


    Model* model            = nullptr;
    Sword* sword            = nullptr;

    State   state = State::Idle;


    float  moveSpeed        = 5.0f;

    float dodgeSpeed = 0.0f;
    bool dodgeFlag = false;

    float  turnSpeed        = DirectX::XMConvertToRadians(720);     // 720��]�܂ł�k

    // �W�����v�ɕK�v�ȕϐ�
    float  jumpSpeed        = 20.0f;
    int    jumpCount        = 0;
    int    jumpLimit        = 1;
    float  landingFallSpeed = -10.0f;

    int    attackCount = 0;
    int    attackLimit = 1;
    float  attackInputTimer = 0.0f;

    bool attackCollisionFlag = false;

    
    ProjectileManager projectileManager;

    Effect* hitEffect = nullptr;

    // ��̃m�[�h
    Model::Node* handNode = nullptr;

    // ��U
    bool stageClearFlag = false;

};