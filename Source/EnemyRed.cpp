#include <imgui.h>

#include "EnemyRed.h"

#include "Graphics/Graphics.h"

#include "player.h"
#include "Mathf.h"
#include "Collision.h"

#include "GameObjectManager.h"
#include "ItemObj.h"

#include "MetaAI.h"



// �R���X�g���N�^
EnemyRed::EnemyRed()
{
    name = CharacterName::ENEMY_RED;
    model = new Model("Data/model/RedEnemy/RedEnemy.mdl");

    // ���f�����傫���̂ŃX�P�[�����O
    scale = { 0.009f, 0.009f, 0.009f };
    param.height = 1.0f;
    param.rayCastRadius = 10.0f;

    dissolveData.maskFlag = 1.0f;
    dissolveData.dissolveThreshold = 0.0f;

    SetTerritory(param.position, 10.0f);
    SetRayCastRadius(5);

    // �ҋ@�X�e�[�g�֑J��
    TransitionIdleState();

    maxHealth = 4.0f;
    health = maxHealth;

    searchRange = 5.0f;
    attackRange = 1.5f;

    attackPower = 1.0f;
}

// �f�X�g���N�^
EnemyRed:: ~EnemyRed()
{
    delete model;
}

// �X�V����
void EnemyRed::Update(float elapsedTime)
{
    switch (state)
    {
    case State::Wander:
        UpdateWanderState(elapsedTime);
        break;

    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;

    case State::Pursuit:
        UpdatePursuitState(elapsedTime);
        break;

    case State::AttackInit:
        UpdateAttackInitState(elapsedTime);
        break;

    case State::Attack1:
        UpdateAttack1State(elapsedTime);
        break; 
    
    case State::Attack2:
        UpdateAttack2State(elapsedTime);
        break;

    case State::IdleBattle:
        UpdateIdleBattleState(elapsedTime);
        break;

    case State::Damage:
        UpdateDamageState(elapsedTime);
        break;

    case State::Death:
        UpdateDeathState(elapsedTime);
        break;
    }

    // ���͏���
    UpdateVelocity(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    CalcPositionMaxMin();

    UpdateOnlyTransform(elapsedTime);
}

void EnemyRed::UpdateOnlyTransform(float elapsedTime)
{
    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���s��X�V
    model->UpdateAnimation(elapsedTime);
    model->UpdateTransform(transform);
}

// �`�揈��
void EnemyRed::Render(const RenderContext& rc, ModelShader* shader)    // ID3D11DeviceContext��dc�ɂȂ��Ă�@�G���[�o����context�ɕύX
{
    shader->Draw(rc, model);
}

// Render�p�l��RenderContext�ɂ܂Ƃ߂�
RenderContext EnemyRed::SetRenderContext(const RenderContext& rc)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_CUSTOMIZE;
    myRc.cubicColorData.rightVec = DirectX::XMFLOAT4{ transform._11, transform._12, transform._13, transform._14 };
    myRc.cubicColorData.topVec = DirectX::XMFLOAT4{ transform._21, transform._22, transform._23, transform._24 };
    myRc.cubicColorData.frontVec = DirectX::XMFLOAT4{ transform._31, transform._32, transform._33, transform._34 };

    myRc.cubicColorData.colorTop1 = colorTop1;
    myRc.cubicColorData.colorBottom1 = colorBottom1;
    myRc.cubicColorData.colorRight1 = colorRight1;
    myRc.cubicColorData.colorLeft1 = colorLeft1;
    myRc.cubicColorData.colorBack1 = colorBack1;
    myRc.cubicColorData.colorFront1 = colorFront1;

    myRc.cubicColorData.colorTop2 = colorTop2;
    myRc.cubicColorData.colorBottom2 = colorBottom2;
    myRc.cubicColorData.colorRight2 = colorRight2;
    myRc.cubicColorData.colorLeft2 = colorLeft2;
    myRc.cubicColorData.colorBack2 = colorBack2;
    myRc.cubicColorData.colorFront2 = colorFront2;

    myRc.cubicColorData.colorAlpha = colorAlpha;


    myRc.maskData.dissolveThreshold = dissolveData.dissolveThreshold;
    myRc.maskData.edgeThreshold = dissolveData.edgeThreshold;
    myRc.maskData.edgeColor = dissolveData.edgeColor;
    myRc.maskData.maskFlag = dissolveData.maskFlag;

    return myRc;
}



void EnemyRed::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
    Player& player = Player::Instance();
    if (player.GetDodgeFlag())return;

    // �m�[�h�̈ʒu�Ɠ����蔻����s��
    Model::Node* node = model->FindNode(nodeName);

    if (node != nullptr)
    {
        // �m�[�h�̃��[���h���W
        DirectX::XMFLOAT3 nodePosition(node->worldTransform._41, node->worldTransform._42, node->worldTransform._43);

        // �����蔻��\��
        //Graphics::Instance().GetDebugRenderer()->DrawSphere(nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1));

        // �v���C���[�Ɠ����蔻��
        DirectX::XMFLOAT3 outPosition;

        if (Collision::InstersectSphereVsCylinder(nodePosition, nodeRadius,
            player.GetCollisionPosition(), player.GetRadius(), player.GetHeight(),
            outPosition))
        {
            // �_���[�W��^����
            if (!player.IsDodge())
            {
                if (player.ApplyDamage(attackPower, 0.5f))
                {
                    // �G�𐁂���΂��x�N�g�����Z�o
                    DirectX::XMFLOAT3 vec;
                    vec.x = outPosition.x - nodePosition.x;
                    vec.z = outPosition.z - nodePosition.z;

                    float length = sqrtf(vec.x * vec.x + vec.z * vec.z);
                    vec.x /= length;
                    vec.z /= length;

                    // XZ���ʂɐ�����΂��͂�������
                    float power = 10.0f;
                    vec.x *= power;
                    vec.z *= power;

                    // Y�����̗�
                    vec.y = 5.0f;

                    // ������΂�
                    player.AddImpulse(vec);
                }
            }
            else if (DropHeelItem <= 0)
            {
                ObjectManager& objectManager = ObjectManager::Instance();
                ItemHeel* obj = new ItemHeel();
                obj->SetPosition(player.GetPosition());
                objectManager.Register(obj);
                DropHeelItem++;
            }

        }
    }
}



// ���S����
void EnemyRed::OnDead()
{
    // ���S�X�e�[�g��
    TransitionDeathState();

}

// �_���[�W����
void EnemyRed::OnDamaged()
{
    // �_���[�W�X�e�[�g��
    TransitionDamageState();
}


// ���^AI���烁�b�Z�[�W����M
bool EnemyRed::OnMessage(const Telegram& telegram)
{
    switch (telegram.msg)
    {
        // ���̃G�l�~�[����Ă΂ꂽ
    case MESSAGE_TYPE::MsgCallHelp:
        if (!SearchPlayer())
        {
            //// �v���C���[�������Ă��Ȃ��Ƃ���1�w�ڃX�e�[�g��Receive�ɕύX
            //stateMachine->SetState(static_cast<int>(State::Recieve));
        }
        return true;

        // ���^AI���珊�L����^����ꂽ�Ƃ�
    case MESSAGE_TYPE::MsgGiveAttackRight:
        // �U���t���O��true�ɐݒ�
        attackFlg = true;

        return true;
    }

    return false;
}



void EnemyRed::SetRamdomTargetPosition()
{
    float a = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float r = Mathf::RandomRange(0.0f, territoryRange);  // raange

    targetPosition.x = territoryOrigin.x + r * sinf(a);
    targetPosition.y = territoryOrigin.y;
    targetPosition.z = territoryOrigin.z + r * cosf(a);
}

void EnemyRed::MoveToTarget(float elapsedTime, float speedRate)
{
    // �^�[�Q�b�g�����֐i�s�x�N�g�����Z�o
    float vx = targetPosition.x - param.position.x;
    float vz = targetPosition.z - param.position.z;

    float dist = sqrtf(vx * vx + vz * vz);
    vx /= dist;
    vz /= dist;

    // �ړ�����
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed);
}

bool EnemyRed::SearchPlayer()
{
    // �v���C���[�Ƃ̍��፷���l������3D�ł̋������������
    const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();

    float vx = playerPosition.x - param.position.x;
    float vy = playerPosition.y - param.position.y;
    float vz = playerPosition.z - param.position.z;

    float dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < searchRange)
    {
        float distXZ = sqrtf(vx * vx + vz * vz);

        // �P�ʃx�N�g����
        vx /= distXZ;
        vz /= distXZ;

        // �O���x�N�g��
        float frontX = sinf(angle.y);
        float frontZ = cosf(angle.y);

        // 2�̃x�N�g���̓��ϒl�őO�㔻��
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)
        {
            return true;
        }
    }
    return false;
}

Player* EnemyRed::searchPlayer()
{
    // �v���C���[�Ƃ̍��፷���l������3D�ł̋������������
    const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();

    float vx = playerPosition.x - param.position.x;
    float vy = playerPosition.y - param.position.y;
    float vz = playerPosition.z - param.position.z;

    float dist = vx * vx + vy * vy + vz * vz;

    if (dist < searchRange * searchRange)
    {
        float distXZ = sqrtf(vx * vx + vz * vz);

        // �P�ʃx�N�g����
        vx /= distXZ;
        vz /= distXZ;

        // �O���x�N�g��
        float frontX = sinf(angle.y);
        float frontZ = cosf(angle.y);

        // 2�̃x�N�g���̓��ϒl�őO�㔻��
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > fovCos)
        {
            return &Player::Instance();
        }
    }
    return nullptr;
}

//------------------------------------------------
//
//      �X�e�[�g
//
//-----------------------------------------------


// �p�j�X�e�[�g�֑J��
void EnemyRed::TransitionWanderState()
{
    state = State::Wander;

    // �ڕW�n�_�ݒ�
    SetRamdomTargetPosition();

    // �����A�j���[�V�����Đ�
    model->PlayAnimation(Animation::Anim_Walking, true);
}

// �p�j�X�e�[�g�X�V����
void EnemyRed::UpdateWanderState(float elapsedTime)
{
    // �ڕW�n�_�܂�XZ���ʂł̋�������
    float vx = targetPosition.x - param.position.x;
    float vz = targetPosition.z - param.position.z;
    float distSq = vx * +vx + vz * vz;

    if (distSq < param.radius * param.radius)
    {
        TransitionIdleState();
    }
    if (SearchPlayer())
    {
        // �v���C���[��������
        TransitionPursuitState();
    }

    MoveToTarget(elapsedTime, 0.5f);

}


// �ҋ@�X�e�[�g�֑J��
void EnemyRed::TransitionIdleState()
{
    state = State::Idle;

    // �^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Idle, true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyRed::UpdateIdleState(float elapsedTime)
{
    // �^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �p�j�X�e�[�g�֑J��
        TransitionWanderState();
    }

    if (SearchPlayer())
    {
        // �v���C���[��������
        TransitionPursuitState();
    }
}


// �ǐՃX�e�[�g�J��
void EnemyRed::TransitionPursuitState()
{
    state = State::Pursuit;
    moveSpeed = 4.0f;

    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    model->PlayAnimation(Anim_Running, true);
}

//�ǐՃX�e�[�g�X�V����
void EnemyRed::UpdatePursuitState(float elapsedTime)
{
    // �ڕW�n�_���v���C���[�ʒu�ɐݒ�
    targetPosition = Player::Instance().GetPosition();

    // �ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 1.0f);

    // �^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �ҋ@�X�e�[�g�֑J��
        TransitionIdleState();
    }

    // �v���C���[����苗�����߂��ɂ����獂������
    float vx = targetPosition.x - param.position.x;
    float vy = targetPosition.y - param.position.y;
    float vz = targetPosition.z - param.position.z;

    float dist = sqrtf(vx * vx + vy * vy + vz * vz);
    if (dist < attackRange)
    {
        // �U���X�e�[�g��
        TransitionAttackInitState();
    }
}

// �U�������X�e�[�g�ɑJ�ڑJ��
void EnemyRed::TransitionAttackInitState()
{
    state = State::AttackInit;

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Idle, true);

}

// �U�������X�e�[�g�X�V����
void EnemyRed::UpdateAttackInitState(float elapsedTime)
{
    targetPosition = Player::Instance().Instance().GetPosition();
    Meta::Instance().SendMessaging(GetId(), 0, MESSAGE_TYPE::MsgAskAttackRight);

    // �v���C���[���U���͈͂ɂ����ꍇ�͍U���X�e�[�g�֑J��
    float vx = targetPosition.x - param.position.x;
    float vy = targetPosition.y - param.position.y;
    float vz = targetPosition.z - param.position.z;

    float dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist > attackRange)
    {
        TransitionIdleBattleState();
    }
    else
    {
        if (attackFlg)
        {
            int num = Mathf::RandomRange(0, 2);

            switch (num)
            {
            case 0:
                // �U��1�X�e�[�g��
                TransitionAttack1State();
                break;
            case 1:
                // �U��2�X�e�[�g��
                TransitionAttack2State();
                break;
            }
        }
        else
        {
            // �U���������i�ҋ@�j
            TransitionIdleBattleState();
        }
    }

    MoveToTarget(elapsedTime, 0.0f);
}

// �U��1�X�e�[�g�ɑJ��
void EnemyRed::TransitionAttack1State()
{
    state = State::Attack1;
    DropHeelItem = 0;
    model->PlayAnimation(Anim_Attack1, false);
}

// �U��1�X�e�[�g�X�V����
void EnemyRed::UpdateAttack1State(float elapsedTime)
{
    // �C�ӂ̃A�j���[�V�����Đ����ł̂ݏՓ˔��菈��������
    float animationTime = model->GetCurrentAnimetionSeconds();

    if (animationTime >= 0.1f && animationTime <= 0.35f)
    {
        // �U���m�[�h�ƃv���C���[�̏Փˏ���
        CollisionNodeVsPlayer("RigRArmPalmGizmo", 1.0f);
    }

    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

// �U��2�X�e�[�g�ɑJ��
void EnemyRed::TransitionAttack2State()
{
    state = State::Attack2;
    DropHeelItem = 0;
    model->PlayAnimation(Anim_Attack2, false);
}

// �U��2�X�e�[�g�X�V����
void EnemyRed::UpdateAttack2State(float elapsedTime)
{
    // �C�ӂ̃A�j���[�V�����Đ����ł̂ݏՓ˔��菈��������
    float animationTime = model->GetCurrentAnimetionSeconds();

    if (animationTime >= 0.1f && animationTime <= 0.35f)
    {
        // �U���m�[�h�ƃv���C���[�̏Փˏ���
        CollisionNodeVsPlayer("RigRibcageGizmo", 1.0f);
    }

    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

// �U���ҋ@�X�e�[�g�ɑJ��
void EnemyRed::TransitionIdleBattleState()
{
    state = State::IdleBattle;

    if (attackFlg)
    {
        // �U���I��������ɍU�����̕���
        // �U���t���O��false��

        SetAttackFlg(false);
        // �G�l�~�[���烁�^AI�� MsgChangeAttackRight �𑗐M����	
        Meta::Instance().SendMessaging(GetId(), 0, MESSAGE_TYPE::MsgChangeAttackRight);
    }


    // �^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(2.0f, 3.0f);

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Idle, true);

}

// �U���ҋ@�X�e�[�g�X�V����
void EnemyRed::UpdateIdleBattleState(float elapsedTime)
{
    targetPosition = Player::Instance().Instance().GetPosition();

    // �^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �v���C���[���U���͈͂ɂ����ꍇ�͍U���X�e�[�g�֑J��
        float vx = targetPosition.x - param.position.x;
        float vy = targetPosition.y - param.position.y;
        float vz = targetPosition.z - param.position.z;

        float dist = sqrtf(vx * vx + vy * vy + vz * vz);
        if (dist < attackRange)
        {
            // �U��������
            TransitionAttackInitState();
        }
        else
        {
            // �p�j�X�e�[�g��
            TransitionWanderState();
        }
    }
    MoveToTarget(elapsedTime, 0.0f);
}

// �_���[�W�X�e�[�g�ɑJ��
void EnemyRed::TransitionDamageState()
{
    state = State::Damage;
    model->PlayAnimation(Anim_Hit, false);
}

// �_���[�W�X�e�[�g�X�V����
void EnemyRed::UpdateDamageState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

// ���S�X�e�[�g�ɑJ��
void EnemyRed::TransitionDeathState()
{
    state = State::Death;
    model->PlayAnimation(Anim_Die, false);
}

// ���S�X�e�[�g�X�V����
void EnemyRed::UpdateDeathState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        SetMaskFlag(0);
        PlusDissolveThreshold(0.05f);
        if (GetDissolveThreshold() >= 1.0f)
        {
            Destroy();
        }
    }
}



//-----------------------------------------------
//
//      �f�o�b�O�p
//
//-----------------------------------------------
void EnemyRed::DrawDebugImGui()
{

    ImGui::SliderFloat("dissolveThreshold", &dissolveData.dissolveThreshold, 0.0f, 1.0f);

    ImGui::SliderFloat("dissolveFlag", &dissolveData.maskFlag, 0.0f, 1.0f);

}

void EnemyRed::DrawDebugPrimitive()
{
    // ���N���X�̃f�o�b�O�v���~�e�B�u�`��
    Enemy::DrawDebugPrimitive();
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // �꒣��͈͂��f�o�b�O�~���`��
    debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0, 1, 0, 1));

    // ���G�͈͂��f�o�b�O�~���`��
    debugRenderer->DrawCylinder(param.position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

    // �^�[�Q�b�g�ʒu���f�o�b�O���`��
    debugRenderer->DrawSphere(targetPosition, param.radius, DirectX::XMFLOAT4(1, 1, 0, 0));

    DirectX::XMFLOAT3 pos = model->FindNode("RigRArmPalmGizmo")->translate;
    debugRenderer->DrawSphere(pos, 1.0f, DirectX::XMFLOAT4(1, 1, 0, 0));

}