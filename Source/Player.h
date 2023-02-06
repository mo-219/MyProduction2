#pragma once

#include "Graphics/shader.h"
#include "Graphics/Model.h"
#include "Character.h"

#include "Sword.h"

#include "Effect.h"

#include "Audio/Audio.h"

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
    void Render(const RenderContext& rc, ModelShader* shader);


    // �f�o�b�O�p
    void DrawDebugGUI();    //GUI�`��
    void DrawDebugPrimitive();  //�v���~�e�B�u�`��


    // ���͏���
    bool ImputJump();                   // �W�����v
    bool InputWeakAttack();             // ��U��
    bool InputStrongAttack();           // ���U��
    bool InputMove(float elapsedTime);  // �ړ�
    bool InputDodge();                  // ����


    // �����X�e�[�g���ǂ���
    bool IsDodge() { return state == State::Dodge; };

    // �����Ă��邩�ǂ���
    bool IsLive() { return isLive; }


    // �Q�b�^�[�A�Z�b�^�[
    void SetStageClearFlag(bool b) { stageClearFlag = b; }
    bool GetStageClearFlag() { return stageClearFlag; }

    bool GetDodgeFlag() { return dodgeFlag; }

    float GetCurrentAP() { return AP; }
    float GetMaxAP() { return MaxAP; }


    // �v�Z����
    bool CalcDodge();           // ����Speed�v�Z
    void CalcAP(float value);   // �A�^�b�N�|�C���g�v�Z
    void AddAP(float value);    // �A�^�b�N�|�C���g��


protected:
    // �ڒn�����Ƃ��ɌĂ΂��
    void OnLanding() override;

    // �_���[�W���󂯂��Ƃ��ɌĂ΂��
    void OnDamaged() override;

    // ���S�����Ƃ��ɌĂ΂��
    void OnDead()override;

    // �U�������Ƃ��ɌĂ΂��
    void OnAttacked();


private:
    // �X�e�B�b�N���͒l����ړ��x�N�g�����擾
    DirectX::XMFLOAT3 GetMoveVec() const;


    // �Փˏ���
    void CollisionPlayerVsEnemies();
    void CollisionPlayerVsStage();
    void CollisionPlayerVsObject();
    void CollisionNodeVsEnemy(const char* nodeName, float nodeRadius);


private:
    // �A�j���[�V����
    enum Animation
    {
        Anim_Error = -1,
        Anim_Combo1 = 0,
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
        Attack1,
        Attack2,
        Attack3,
        Attack4,
        Dodge,
        Falling,
        Landing,
        Damage,
        Dead,

        Max
    };



    //--------------------------------
    // 
    //      �A�j���[�V����
    // 
    //--------------------------------
    
    // �A�j���[�V�����X�e�[�g 
    // �ҋ@�X�e�[�g
    void TransitionIdleState();                     // �J��
    void UpdateIdleState(float elapsedTime);        // �X�V����

    // �ړ��X�e�[�g
    void TransitionMoveState();                     // �J��
    void UpdateMoveState(float elapsedTime);        // �X�V����

    // �W�����v�X�e�[�g
    void TransitionJumpInitState();                 // �J��
    void UpdateJumpInitState(float elapsedTime);    // �X�V����

    // �W�����v�X�e�[�g
    void TransitionJumpState();                     // �J��
    void UpdateJumpState(float elapsedTime);        // �X�V����

    // ���n�X�e�[�g
    void TransitionLandingState();                  // �J��
    void UpdateLandingState(float elapsedTime);     // �X�V����

    // �����X�e�[�g
    void TransitionFallingState();                  // �J��
    void UpdateFallingState(float elapsedTime);     // �X�V����


    //****************
    // 
    //  �U���X�e�[�g
    //
    //****************
    // �R���{�P
    void TransitionAttack1State();               // �J��
    void UpdateAttack1State(float elapsedTime);  // �X�V����

    // �R���{2
    void TransitionAttack2State();               // �J��
    void UpdateAttack2State(float elapsedTime);  // �X�V����

    // �R���{3
    void TransitionAttack3State();               // �J��
    void UpdateAttack3State(float elapsedTime);  // �X�V����

    // �R���{4
    void TransitionAttack4State();               // �J��
    void UpdateAttack4State(float elapsedTime);  // �X�V����
    //****************
    

    // �����X�e�[�g
    void TransitionDodgeState();               // �J��
    void UpdateDodgeState(float elapsedTime);  // �X�V����


    // �_���[�W�X�e�[�g
    void TransitionDamageState();               // �J��
    void UpdateDamageState(float elapsedTime);  // �X�V����

    // ���S�X�e�[�g
    void TransitionDeathState();                // �J��
    void UpdateDeathState(float elapsedTime);   // �X�V����

    //---------------------------------



    // ���f���f�[�^�ۑ�
    Model* model            = nullptr;

    // ����ۑ�
    Sword* sword = nullptr;


    // animation�p�ϐ�
    float animationLimit = 0.0f;
    State   state = State::Idle;


    // �ړ��X�s�[�h�p�ϐ�
    float  moveSpeed        = 5.0f;

    float  dodgeSpeed = 0.0f;
    bool   dodgeFlag = false;

    float  turnSpeed        = DirectX::XMConvertToRadians(720);     // 720��]�܂ł�k


    // �W�����v�ɕK�v�ȕϐ�
    float  jumpSpeed        = 20.0f;    // �W�����v�̑���
    int    jumpCount        = 0;        // �W�����v��
    int    jumpLimit        = 1;        // �W�����v�񐔂̐���
    float  landingFallSpeed = -10.0f;   // �����čs���X�s�[�h

    // �U���p�ϐ�
    float  attackInputTimer = 0.0f;     
    bool   attackCollisionFlag = false;

    // �G�t�F�N�g�̕ۑ�
    Effect* hitEffect = nullptr;

    // �N���A�t���O�ۑ��p
    bool stageClearFlag = false;


    // �A�^�b�N�|�C���g
    float AP = 100;
    float MaxAP = 100;

    // �����t���O
    bool isLive = true;     // true: �����Ă���


    // SE�ۑ��p�ϐ�
    enum SEID
    {
        SE_Sword,
        SE_JUMP,
        SE_DODGE,
        SE_HIT,
        SE_MAX
    };

    std::unique_ptr<AudioSource> se[static_cast<int>(SEID::SE_MAX)];
};