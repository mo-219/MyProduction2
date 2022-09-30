#include <imgui.h>
#include "player.h"
#include "Input/input.h"

#include "Camera.h"

#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "Collision.h"

#include "ProjectileStraight.h"
#include "ProjectileHoming.h"

// �R���X�g���N�^
Player::Player()
{
    model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

    hitEffect = new Effect("Data/Effect/Hit.efk");

    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.01f;
}

// �f�X�g���N�^
Player::~Player()
{
    delete hitEffect;
    delete model;
}

// �X�V����
void Player::Update(float elapsedTime)  //�O��Update���Ă��獡����Ă鎞�܂ł̎���(�o�ߕb��)
{
#if 0
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // �ړ�����
    float moveSpeed = this->moveSpeed * elapsedTime;
    position.x += moveVec.x * moveSpeed;
    position.z += moveVec.z * moveSpeed;
#endif
#if 0
    // ���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX(); // �Q�[���p�b�h��L(��)-Axis(��)�X�e�B�b�N��X����
    float ay = gamePad.GetAxisLY(); // �Q�[���p�b�h��L(��)-Axis(��)�X�e�B�b�N��Y����

    // �ړ�����
    float moveSpeed = 5.0f * elapsedTime;
    {
        // ���X�e�B�b�N�̓��͏������Ƃ�XZ���ʂւ̈ړ�����
        position.x += ax * moveSpeed;
        position.z += ay * moveSpeed;
    }
    
    // ��]����
    float ratateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;
    if (gamePad.GetButton() & GamePad::BTN_A)
    {
        // X����]����
        angle.x += ratateSpeed;

    }
    if (gamePad.GetButton() & GamePad::BTN_B)
    {
        // Y����]����
        angle.y += ratateSpeed;

    }
    if (gamePad.GetButton() & GamePad::BTN_X)
    {
        // Z����]����
        angle.z += ratateSpeed;

    }
#endif

    InputMove(elapsedTime);

    // �W�����v���͏���
    ImputJump();

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // �e�ۓ��͏���
    InputProjectile();

    projectileManager.Update(elapsedTime);

    // �v���C���[�ƓG�Ƃ̏Փˏ���
    CollisionPlayerVsEnemies();

    CollisionProjectilesVsEnemies();

    

    //�I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���s��X�V
    model->UpdateTransform(transform);
}


// �`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    projectileManager.Render(dc, shader);
    shader->Draw(dc, model);
}

void Player::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}

// �f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // �ʒu
            ImGui::InputFloat3("Position", &position.x);

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
void Player::InputMove(float elapsedTime)
{
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // �ړ�����
    Move(moveVec.x, moveVec.z, maxMoveSpeed);
    //Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

    // ���񏈗�
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}


// �f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// ��������p�̃f�o�b�O����`��
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

    // ��������p�̃f�o�b�O�~����`��
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
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

        if (Collision::InstersectCylinderVsCylinder(position, GetRadius(),GetHeight(),
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
            DirectX::XMVECTOR Pvec = DirectX::XMLoadFloat3(&position);
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


// �W�����v���͏���
void Player::ImputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        if (jumpCount <= jumpLimit)
        {
            ++jumpCount;
            Jump(jumpSpeed);
        }

        //if (jumpRest > 0)         //�@�̂���W�����v�񐔓I�ȃv���O����
        //{
        //    --jumpCount;
        //    Jump(jumpSpeed);
        //}
    }
}

// �ڒn�����Ƃ��ɌĂ΂��
void Player::OnLanding()
{
    //jumpRest = jumpLimit;
    jumpCount = 0;
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
        pos.x = position.x;
        pos.y = position.y + height/2;
        pos.z = position.z;

        // ����
        ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
        projectile->Launch(dir, pos);
        /*projectileManager.Registar(projectile);*/
    }

    if (gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        // �O����
        DirectX::XMFLOAT3 dir;
        dir.z = cosf(angle.y);      // ZX,XY,YZ�������Ƃ����߂ɗ���̂�cosf()�A���ɗ���̂�sinf()�Ɗo����
        dir.x = sinf(angle.y);      // �킴�Ƃ������Ԃ��Ⴄ�̂Œ���
        dir.y = 0.0f;

        // ���ˈʒu
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height / 2;
        pos.z = position.z;

        // �^�[�Q�b�g�ݒ�
        DirectX::XMFLOAT3 target;
        target.x = position.x + dir.x * 1000.0f;
        target.y = position.y + dir.y * 1000.0f;
        target.z = position.z + dir.z * 1000.0f;

        // ��ԋ߂��̓G���^�[�Q�b�g�ɂ���
        float dist = FLT_MAX;
        EnemyManager& enemyManager = EnemyManager::Instance();
        int enemyCount = enemyManager.GetEnemyCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            // �G�Ƃ̋�������
            Enemy* enemy = EnemyManager::Instance().GetEnemy(i);

            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
            DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);

            float d;
            DirectX::XMStoreFloat(&d, D);

            // �b��^�[�Q�b�g�����߂��G�Ȃ�
            if (d < dist)
            {
                // �b��^�[�Q�b�g�̍X�V
                dist = d;
                target = enemy->GetPosition();
                target.y += enemy->GetHeight() * 0.5f;
            }
        
        }


        // ����
        ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
        projectile->Launch(dir, pos,target);
        /*projectileManager.Registar(projectile);*/
    }


}