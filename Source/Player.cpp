#include <imgui.h>
#include "player.h"
#include "Input/input.h"

#include "Camera.h"

#include "Graphics/Graphics.h"

#include "StageManager.h"
#include "EnemyManager.h"
#include "GameObjectManager.h"
#include "Collision.h"

#include "ProjectileStraight.h"
#include "ProjectileHoming.h"


// �C���^���X
static Player* instance = nullptr;

// �C���X�^���X�擾
Player& Player::Instance()
{
    return *instance;
}


// �R���X�g���N�^ 
Player::Player()
{
    name = CharacterName::PLAYER;

    // �C���X�^���X�|�C���^�ݒ�
    instance = this;

    model = new Model("Data/Model/Player/Player.mdl");

    hitEffect = new Effect("Data/Effect/Hit.efk");

    sword = new Sword("Data/Model/Sword/Sword2.mdl");
    DirectX::XMFLOAT4X4 swordStart = model->FindNode("LeftHand")->worldTransform;     // �茳

    handNode = model->FindNode("LeftHand");   // ��̃m�[�h�擾

    // �ҋ@�X�e�[�g�֑J��
    TransitionIdleState();

    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.02f;

    maxMoveSpeed = 7.0f;

    health = 1000;

    SetRayCastRadius(5);
}

// �f�X�g���N�^
Player::~Player()
{
    delete sword;
    delete hitEffect;
    delete model;
}

void Player::Initialize(DirectX::XMFLOAT3 Respawn)
{
    health = maxHealth;
    param.position = Respawn;
    stageClearFlag = false;
    TransitionIdleState();

}

// �X�V����
void Player::Update(float elapsedTime)  //�O��Update���Ă��獡����Ă鎞�܂ł̎���(�o�ߕb��)
{
    // �X�e�[�g���̏���
    switch (state)
    {
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;

    case State::Move:
        UpdateMoveState(elapsedTime);
        break;

    case State::Jump:
        UpdateJumpState(elapsedTime);
        break;


    case State::Attack1:
        UpdateAttack1State(elapsedTime);
        break;    
    
    case State::Attack2:
        UpdateAttack2State(elapsedTime);
        break;    
    
    case State::Attack3:
        UpdateAttack3State(elapsedTime);
        break;    
    
    case State::Attack4:
        UpdateAttack4State(elapsedTime);
        break;

    case State::Dodge:
        UpdateDodgeState(elapsedTime);
        break;

    case State::Falling:
        UpdateFallingState(elapsedTime);
        break;

    case State::Landing:
        UpdateLandingState(elapsedTime);
        break;


    }

    attackInputTimer -= 0.1f;
    if (attackInputTimer <= 0.0f)
    {
        attackInputTimer = 0.0f;
        attackCount = 0;
    }

    UpdateInvincibleTimer(elapsedTime);

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // �e�ۓ��͏���
    InputProjectile();

    projectileManager.Update(elapsedTime);

    // �v���C���[�ƓG�Ƃ̏Փˏ���
    CollisionPlayerVsEnemies();

    CollisionProjectilesVsEnemies();

    CollisionPlayerVsStage();

    CollisionPlayerVsObject();

    UpdateOnlyTransform(elapsedTime);




}

void Player::UpdateOnlyTransform(float elapsedTime)
{
    //�I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);

    // ���f���s��X�V
    model->UpdateTransform(transform);




    sword->UpdateTransform(model->FindNode("LeftHand")->worldTransform);
    DirectX::XMFLOAT4X4 tip = model->FindNode("LTip")->worldTransform;
    sword->SetTipPos(DirectX::XMFLOAT3(tip._41, tip._42, tip._43));
    sword->Update(elapsedTime);
}


// �`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    projectileManager.Render(dc, shader);
    shader->Draw(dc, model);
}

void Player::Render(const RenderContext& rc, ModelShader* shader)
{
    // ����f�[�^�}��
    RenderContext prc = rc;
    prc.cubicColorData.shaderFlag    = CUBIC_CUSTOMIZE;
    prc.cubicColorData.rightVec      = DirectX::XMFLOAT4{ transform._11, transform._12, transform._13, transform._14 };
    prc.cubicColorData.topVec        = DirectX::XMFLOAT4{ transform._21, transform._22, transform._23, transform._24 };
    prc.cubicColorData.frontVec      = DirectX::XMFLOAT4{ transform._31, transform._32, transform._33, transform._34 };

    prc.cubicColorData.colorTop1     = colorTop1;
    prc.cubicColorData.colorBottom1  = colorBottom1;
    prc.cubicColorData.colorRight1   = colorRight1;
    prc.cubicColorData.colorLeft1    = colorLeft1;
    prc.cubicColorData.colorBack1    = colorBack1;
    prc.cubicColorData.colorFront1   = colorFront1;

    prc.cubicColorData.colorTop2     = colorTop2;
    prc.cubicColorData.colorBottom2  = colorBottom2;
    prc.cubicColorData.colorRight2   = colorRight2;
    prc.cubicColorData.colorLeft2    = colorLeft2;
    prc.cubicColorData.colorBack2    = colorBack2;
    prc.cubicColorData.colorFront2   = colorFront2;

    prc.cubicColorData.colorAlpha    = colorAlpha;

    projectileManager.Render(rc, shader);
    sword->Render(rc,shader);

    shader->Draw(prc, model);
}

// �f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    std::string str = "";

    switch (state) {
    case State::Attack1:
        str = "Attack1";
        break;  

    case State::Attack2:
        str = "Attack2";
        break;    
    
    case State::Attack3:
        str = "Attack3";
        break;    
    
    case State::Attack4:
        str = "Attack4";
        break;

    case State::Idle:
        str = "Idle";
        break;

    case State::Move:
        str = "Move";
        break;

    case State::Dodge:
        str = "Dodge";
        break;

    case State::Jump:
        str = "Jump";
        break;

    case State::Falling:
        str = "Falling";
        break;
    case State::Landing:
        str = "Landing";
        break;
    }

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::Text(u8"State�@%s", str.c_str());

        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // �ʒu
            
            ImGui::InputFloat("attackInputTimer", &attackInputTimer);

            DirectX::XMFLOAT4X4 nodeTransform = model->FindNode("LeftHand")->worldTransform;
            ImGui::InputFloat("Transform11", &transform._11);
            ImGui::InputFloat("Transform12", &transform._12);
            ImGui::InputFloat("Transform13", &transform._13);
            ImGui::InputFloat("Transform14", &transform._14);
            ImGui::InputFloat("Transform21", &transform._21);
            ImGui::InputFloat("Transform22", &transform._22);
            ImGui::InputFloat("Transform23", &transform._23);
            ImGui::InputFloat("Transform24", &transform._24);
            ImGui::InputFloat("Transform31", &transform._31);
            ImGui::InputFloat("Transform32", &transform._32);
            ImGui::InputFloat("Transform33", &transform._33);
            ImGui::InputFloat("Transform34", &transform._34);
            ImGui::InputFloat("Transform41", &transform._41);
            ImGui::InputFloat("Transform42", &transform._42);
            ImGui::InputFloat("Transform43", &transform._43);
            ImGui::InputFloat("Transform44", &transform._44);
            // ��]
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);

            ImGui::InputFloat3("velocity", &velocity.x);
            // �X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
            ImGui::Separator();
            ImGui::Text("CubicColor");

            if (ImGui::TreeNode("colorTop"))
            {
                ImGui::ColorPicker3("colorTop", &colorTop1.x);  
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("colorBottom"))
            {
                ImGui::ColorPicker3("colorBottom", &colorBottom1.x);
                ImGui::TreePop();

            }
            if (ImGui::TreeNode("colorRight"))
            {
                ImGui::ColorPicker3("colorRight", &colorRight1.x);
                ImGui::TreePop();

            }
            if (ImGui::TreeNode("colorLeft"))
            {
                ImGui::ColorPicker3("colorLeft", &colorLeft1.x);
                ImGui::TreePop();

            }
            if (ImGui::TreeNode("colorBack"))
            {
                ImGui::ColorPicker3("colorBack", &colorBack1.x);
                ImGui::TreePop();

            }
            if (ImGui::TreeNode("colorFront"))
            {
                ImGui::ColorPicker3("colorFront", &colorFront1.x);
                ImGui::TreePop();

            }

            ImGui::SliderFloat("Alpha", &colorAlpha.w, 0.0f, +1.0f);

            DirectX::XMFLOAT4X4 swordStart = model->FindNode("LeftHand")->worldTransform;     // �茳
            DirectX::XMFLOAT4X4 swordEnd = model->FindNode("LTip")->worldTransform;       // ����
            DirectX::XMFLOAT3 aa = DirectX::XMFLOAT3(swordStart._41, swordStart._42, swordStart._42);
            DirectX::XMFLOAT3 bb = DirectX::XMFLOAT3(swordEnd._41, swordEnd._42, swordEnd._42);
            ImGui::InputFloat3("swordStart", &aa.x);
            ImGui::InputFloat3("swordEnd", &bb.x);
            ImGui::Separator();
            sword->DrawDebugGUI();


        }
    }
    ImGui::End();
}

// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    // ���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    // �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

    // �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraRightX      = cameraRight.x;
    float cameraRightZ      = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    
    if (cameraRightLength > 0.0f)
    {
        // �P�ʃx�N�g����
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;

    }

    // �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX      = cameraFront.x;
    float cameraFrontZ      = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

    if (cameraFrontLength > 0.0f)
    {
        // �P�ʃx�N�g����
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    // �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
    // �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
    // �i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;
    vec.x = (ax * (cameraRightX)) + (ay * (cameraRightZ));
    vec.z = (ax * (cameraFrontX)) + (ay * (cameraFrontZ));

    // Y�������ɂ͈ړ����Ȃ�
    vec.y = 0.0f;

    return vec;
}


// �ړ����͏���
bool Player::InputMove(float elapsedTime)
{
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // �ړ�����
    Move(moveVec.x, moveVec.z, maxMoveSpeed);
    //Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

    // ���񏈗�
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

    return !(moveVec.x * moveVec.x + moveVec.y * moveVec.y + moveVec.z * moveVec.z <= 0.0f);

}


// �f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// ��������p�̃f�o�b�O����`��
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

    // ��������p�̃f�o�b�O�~����`��
    debugRenderer->DrawCylinder(param.position, param.radius, param.height, DirectX::XMFLOAT4(0, 0, 0, 1));
    debugRenderer->DrawSphere(param.position, param.rayCastRadius, DirectX::XMFLOAT4{ 0,1,0,1 });


    sword->DrawDebugPrimitive();
}

// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{

    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓˏ���
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        DirectX::XMFLOAT3 outPosition{};
        Enemy* enemy = enemyManager.GetEnemy(i);

        //if (Collision::InstersectSphereVsSphere(position, GetRadius(),
        //    enemy->GetPosition(), enemy->GetRadius(),
        //    outPosition ))
        //{
        //    // �����o����̈ʒu�ݒ�
        //    enemyManager.GetEnemy(i)->setPosition(outPosition);
        //}

        if (Collision::InstersectCylinderVsCylinder(param.position, GetRadius(),GetHeight(),
            enemy->GetPosition(), enemy->GetRadius(),enemy->GetHeight(),
            outPosition))
        {
#if 0
            if (position.y > enemy->GetPosition().y + 0.8f*enemy->GetHeight() )
            {
                velocity.y = 0.0f;
                Jump(10.0f);
                //OnLanding();
            }


            // �����o����̈ʒu�ݒ�
            enemyManager.GetEnemy(i)->setPosition(outPosition);
#endif
            // �G�̐^��t�߂ɓ����������ǂ������m�F
            DirectX::XMVECTOR Pvec = DirectX::XMLoadFloat3(&param.position);
            DirectX::XMVECTOR Evec = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR vec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(Pvec, Evec));

            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, vec);

            // ��𓥂݂��Ă����珬�W�����v(normal��y�����ɒl�������Ă�����)
            if (normal.y > 0.8f)
            {
                Jump(jumpSpeed * 0.3f);
            }
            else
            {
                // �����o����̈ʒu�ݒ�
                enemyManager.GetEnemy(i)->setPosition(outPosition);

            }

        }
    }
}

void Player::CollisionProjectilesVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̒e�ۂƂ��ׂĂ̓G�𑍓�����ŏՓˏ���
    int projectileCount = projectileManager.GetProjectileCount();
    int enemyCount = enemyManager.GetEnemyCount();

    for (int i = 0; i < projectileCount; ++i)
    {
        Projectile* projectile = projectileManager.GetProjectile(i);
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // �Փˏ���
            DirectX::XMFLOAT3 outPosition;
            if (Collision::InstersectSphereVsCylinder(
                projectile->GetPosition(),
                projectile->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPosition))
            {
                // �_���[�W��^����
                if (enemy->ApplyDamage(1, 0.5f))
                {
                    // ������΂�
                    {
                        DirectX::XMFLOAT3 impulse;
                        const float power = 10.0f;

                        const DirectX::XMFLOAT3& e = enemy->GetPosition();
                        const DirectX::XMFLOAT3& p = projectile->GetPosition();

                        float vx = e.x - p.x;
                        float vz = e.z - p.z;

                        float length = sqrtf(vx * vx + vz * vz);
                        vx /= length;
                        vz /= length;

                        impulse.x = vx * power;
                        impulse.y = 0.5f * power;
                        impulse.z = vz * power;


                        enemy->AddImpulse(impulse);
                    }

                    // �q�b�g�G�t�F�N�g�Đ�
                    {
                        DirectX::XMFLOAT3 e = enemy->GetPosition();
                        e.y += enemy->GetHeight() * 0.5f;
                        hitEffect->Play(e);
                    }


                    // �e�۔j��
                    projectile->Destroy();
                }
            }
        }
    }

}

void Player::CollisionPlayerVsStage()
{
    StageManager& stageManager = StageManager::Instance();
    int stageCount = stageManager.GetStageCount();

    for (int i = 0; i < stageCount; i++)
    {
        DirectX::XMFLOAT3 outPosition{};
        Stage* stage = stageManager.GetStage(i);
        if (stage->GetStageNum() != StageNumber::Door) continue;


        if (Collision::InstersectCubeVsCylinder(stage->GetPosition(), stage->GetWidth(), stage->GetHeight(), stage->GetDepth(), param.position, param.radius, param.height, outPosition))
        {

            // �G�̐^��t�߂ɓ����������ǂ������m�F
            DirectX::XMVECTOR Pvec = DirectX::XMLoadFloat3(&param.position);
            DirectX::XMVECTOR Evec = DirectX::XMLoadFloat3(&stage->GetPosition());
            DirectX::XMVECTOR vec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(Pvec, Evec));

            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, vec);

            // ��𓥂݂��Ă����珬�W�����v(normal��y�����ɒl�������Ă�����)
            if (normal.y > 0.8f)
            {
                Jump(jumpSpeed * 0.3f);
            }
            else
            {
                if (stage->GetCollisionFlag())
                {
                    // �����o����̈ʒu�ݒ�
                    param.position = outPosition;
                }
                else
                {
                    SetStageClearFlag(true);
                }


            }

        }

    }

}

void Player::CollisionPlayerVsObject()
{
    ObjectManager& objManager = ObjectManager::Instance();
    int objCount = objManager.GetObjectCount();

    for (int i = 0; i < objCount; i++)
    {
        DirectX::XMFLOAT3 outPosition{};
        GameObject* obj = objManager.GetObj(i);
        obj->CollisionFlag = false;
        if (obj->GetBehavior()->collision(obj->GetParam(), this->GetParam(), outPosition))
        {
            obj->CollisionFlag = true;
            param.position = outPosition;
        }
    }

}


// �W�����v���͏���
bool Player::ImputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        if (jumpCount < jumpLimit)
        {
            ++jumpCount;
            Jump(jumpSpeed);

            return true;
        }
    }
    return false;
}



// �����ꂽ�����肷�邽�߉�����ʂ�悤�ɂ��Ă���
bool Player::InputAttack()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_B)
    {
        attackInputTimer = 2.0f;
        return true;
    }
    return false;
}

// �A�^�b�N�����邩�ǂ����͂����Ŕ���
bool Player::JudgeAttack()
{
    if (InputAttack() || attackInputTimer > 0.0f)
    {
        if (attackCount <= attackLimit)
        {
            return true;
        }
    }
    return false;
}

bool Player::InputDodge()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_Y) return true;
    return false;
}

bool Player::CalcDodge()
{
    if (dodgeSpeed < 0.0f)return false;

    dodgeSpeed -= 0.3f;

    DirectX::XMFLOAT3 front = DirectX::XMFLOAT3(transform._31, transform._32, transform._33);
    param.position.z += front.z * dodgeSpeed;
    param.position.x += front.x * dodgeSpeed;
}

// �ڒn�����Ƃ��ɌĂ΂��
void Player::OnLanding()
{
    //jumpRest = jumpLimit;
    jumpCount = 0;

    // �_���[�W�A���S�X�e�[�g���͒��n�����Ƃ��ɃX�e�[�g�J�ڂ��Ȃ��悤�ɂ���
    //if (state != State::Damage && state != State::Death)
    //{
    if (dodgeFlag)   return;
        if (velocity.y < landingFallSpeed)
        {
            TransitionLandingState();
        }
    //}
}

void Player::OnDamaged()
{
    // �_���[�W�X�e�[�g�֑J��
    //TransitionDamageState();

}

void Player::OnDead()
{
    // ���S�X�e�[�g�֑J��
    //TransitionDeathState();

}

void Player::Attacked()
{
    sword->collisionWeaponVSEnemies();
}


// �e�ۓ��͏���
void Player::InputProjectile()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_X)
    {
        // �O����
        DirectX::XMFLOAT3 dir;
        dir.z = cosf(angle.y);      // ZX,XY,YZ�������Ƃ����߂ɗ���̂�cosf()�A���ɗ���̂�sinf()�Ɗo����
        dir.x = sinf(angle.y);      // �킴�Ƃ������Ԃ��Ⴄ�̂Œ���
        dir.y = 0.0f;

        // ���ˈʒu
        DirectX::XMFLOAT3 pos;
        pos.x = param.position.x;
        pos.y = param.position.y + param.height/2;
        pos.z = param.position.z;

        // ����
        ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
        projectile->Launch(dir, pos);
        /*projectileManager.Registar(projectile);*/
    }



}

void Player::TransitionIdleState()
{
    state = State::Idle;

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Idle, true, 1.2f);
}

void Player::UpdateIdleState(float elapsedTime)
{
    // �ړ����͏���
    if (InputMove(elapsedTime))
    {
        TransitionMoveState();
    }
    // �W�����v���͏���
    if (ImputJump())
    {
        TransitionJumpState();
    }
    // �U�����͏���
    if (InputAttack() && attackCount <= attackLimit)
    {
        TransitionAttack1State();
    }
    if (InputDodge())
    {
        TransitionDodgeState();
    }

    // �e�ۓ��͏���
    InputProjectile();
}

void Player::TransitionMoveState()
{
    state = State::Move;

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Running, true, 1.8f);

}

void Player::UpdateMoveState(float elapsedTime)
{
    // �ړ����͏���
    if (!InputMove(elapsedTime))
    {
        TransitionIdleState();
    }
    // �W�����v���͏���
    if (ImputJump())
    {
        TransitionJumpState();
    }
    // �U�����͏���
    if (InputAttack() && attackCount <= attackLimit)
    {
        TransitionAttack1State();
    }
    if (InputDodge())
    {
        TransitionDodgeState();
    }
    // �e�ۓ��͏���
    InputProjectile();

}
void Player::TransitionJumpInitState()
{
    state = State::Jump;

    model->PlayAnimation(Anim_Jump_Up1, true, 1.0f);

}

void Player::UpdateJumpInitState(float elapsedTime)
{
    float my = velocity.y * elapsedTime;

    // �ړ����͏���
    bool move = InputMove(elapsedTime);

    // �W�����v���͏���
    if (ImputJump())
    {
        TransitionJumpInitState();
    }
    if (InputDodge())
    {
        TransitionDodgeState();
    }

    if (my < 2.0f)
    {
        TransitionJumpState();
    }





}
void Player::TransitionJumpState()
{
    state = State::Jump;

    model->PlayAnimation(Anim_Jump_Peak, true, 1.0f);

}

void Player::UpdateJumpState(float elapsedTime)
{
    float my = velocity.y * elapsedTime;

    // �ړ����͏���
    bool move = InputMove(elapsedTime);

    // �W�����v���͏���
    if (ImputJump())
    {
        TransitionJumpInitState();
    }
    if (InputDodge())
    {
        TransitionDodgeState();
    }

    if (my < -1.0f)
    {
        TransitionFallingState();
    }


    // �e�ۓ��͏���
    InputProjectile();


}

void Player::TransitionLandingState()
{
    state = State::Landing;

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Landing, false, 1.0f);

}

void Player::UpdateLandingState(float elapsedTime)
{
    // �ړ����͏���
    if (!model->IsPlayAnimation())
    {
        if (!InputMove(elapsedTime))
        {
            TransitionIdleState();
        }
        else
        {
            TransitionMoveState();
        }
    }
}

void Player::TransitionFallingState()
{
    state = State::Falling;
    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Jump_Down1, true);
}

void Player::UpdateFallingState(float elapsedTime)
{
    InputMove(elapsedTime);
    if (InputDodge())
    {
        TransitionDodgeState();
    }
}

void Player::TransitionAttack1State()
{
    state = State::Attack1;
    AnimationLimit = 0.3f;
    model->PlayAnimation(Anim_Combo1, false, 0.9f);
}

void Player::UpdateAttack1State(float elapsedTime)
{
    Attacked();


    if (!model->IsPlayAnimation())
    {
        /*attckCollisionFlag = false;*/

        if (!InputMove(elapsedTime))
        {
            TransitionIdleState();
        }
        else
        {
            TransitionMoveState();
        }
    }  
    else if (model->GetCurrentAnimetionSeconds() >= AnimationLimit)
    {
        if (InputAttack())
        {
            TransitionAttack2State();
        }
    }
}

void Player::TransitionAttack2State()
{
    state = State::Attack2;
    AnimationLimit = 0.5f;
    model->PlayAnimation(Anim_Combo2, false, 0.9f);
}

void Player::UpdateAttack2State(float elapsedTime)
{
    Attacked();

    if (!model->IsPlayAnimation())
    {
        /*attckCollisionFlag = false;*/
        if (!InputMove(elapsedTime))
        {
            TransitionIdleState();
        }
        else
        {
            TransitionMoveState();
        }
    }
    else if (model->GetCurrentAnimetionSeconds() >= AnimationLimit)
    {
        if (InputAttack())
        {
            TransitionAttack3State();
        }
    }
}


void Player::TransitionAttack3State()
{
    state = State::Attack3;
    AnimationLimit = 0.5f;
    model->PlayAnimation(Anim_Combo3, false, 0.9f);
}

void Player::UpdateAttack3State(float elapsedTime)
{
    Attacked();


    if (!model->IsPlayAnimation())
    {
        /*attckCollisionFlag = false;*/
        if (!InputMove(elapsedTime))
        {
            TransitionIdleState();
        }
        else
        {
            TransitionMoveState();
        }
    }
    else if (model->GetCurrentAnimetionSeconds() >= AnimationLimit)
    {
        if (InputAttack())
        {
            TransitionAttack4State();
        }
    }
}

void Player::TransitionAttack4State()
{
    state = State::Attack4;
    AnimationLimit = 1.0f;
    model->PlayAnimation(Anim_Combo4, false, 0.9f);
}

void Player::UpdateAttack4State(float elapsedTime)
{
    Attacked();

    if (!model->IsPlayAnimation())
    {
        /*attckCollisionFlag = false;*/
        if (!InputMove(elapsedTime))
        {
            TransitionIdleState();
        }
        else
        {
            TransitionMoveState();
        }
    }
}









void Player::TransitionDodgeState()
{
    state = State::Dodge;
    dodgeSpeed = 12.0f;
    dodgeFlag = true;
    model->PlayAnimation(Anim_Dodge, false, 2.5f);

}

void Player::UpdateDodgeState(float elapsedTime)
{

    if (!model->IsPlayAnimation())
    {
        dodgeFlag = false;
        if (!InputMove(elapsedTime))
        {
            TransitionIdleState();
        }
        else
        {
            TransitionMoveState();
        }
    }
    CalcDodge();
}
