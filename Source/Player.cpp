#include <imgui.h>
#include "player.h"
#include "Input/input.h"

#include "Camera.h"

#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "Collision.h"

#include "ProjectileStraight.h"
#include "ProjectileHoming.h"

// コンストラクタ
Player::Player()
{
    model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

    hitEffect = new Effect("Data/Effect/Hit.efk");

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.01f;
}

// デストラクタ
Player::~Player()
{
    delete hitEffect;
    delete model;
}

// 更新処理
void Player::Update(float elapsedTime)  //前回Updateしてから今やってる時までの時間(経過秒数)
{
#if 0
    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // 移動処理
    float moveSpeed = this->moveSpeed * elapsedTime;
    position.x += moveVec.x * moveSpeed;
    position.z += moveVec.z * moveSpeed;
#endif
#if 0
    // 入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX(); // ゲームパッドのL(左)-Axis(軸)スティックのX成分
    float ay = gamePad.GetAxisLY(); // ゲームパッドのL(左)-Axis(軸)スティックのY成分

    // 移動操作
    float moveSpeed = 5.0f * elapsedTime;
    {
        // 左スティックの入力情報をもとにXZ平面への移動処理
        position.x += ax * moveSpeed;
        position.z += ay * moveSpeed;
    }
    
    // 回転操作
    float ratateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;
    if (gamePad.GetButton() & GamePad::BTN_A)
    {
        // X軸回転操作
        angle.x += ratateSpeed;

    }
    if (gamePad.GetButton() & GamePad::BTN_B)
    {
        // Y軸回転操作
        angle.y += ratateSpeed;

    }
    if (gamePad.GetButton() & GamePad::BTN_X)
    {
        // Z軸回転操作
        angle.z += ratateSpeed;

    }
#endif

    InputMove(elapsedTime);

    // ジャンプ入力処理
    ImputJump();

    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 弾丸入力処理
    InputProjectile();

    projectileManager.Update(elapsedTime);

    // プレイヤーと敵との衝突処理
    CollisionPlayerVsEnemies();

    CollisionProjectilesVsEnemies();

    

    //オブジェクト行列を更新
    UpdateTransform();

    // モデル行列更新
    model->UpdateTransform(transform);
}


// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    projectileManager.Render(dc, shader);
    shader->Draw(dc, model);
}

void Player::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}

// デバッグ用GUI描画
void Player::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 位置
            ImGui::InputFloat3("Position", &position.x);

            // 回転
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);

            ImGui::InputFloat3("velocity", &velocity.x);
            // スケール
            ImGui::InputFloat3("Scale", &scale.x);
        }
    }
    ImGui::End();
}

// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    // 入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // カメラ方向とスティックの入力値によって進行方向を計算する
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    // 移動ベクトルはXZ平面に水平なベクトルになるようにする

    // カメラ右方向ベクトルをXZ単位ベクトルに変換
    float cameraRightX      = cameraRight.x;
    float cameraRightZ      = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    
    if (cameraRightLength > 0.0f)
    {
        // 単位ベクトル化
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;

    }

    // カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX      = cameraFront.x;
    float cameraFrontZ      = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

    if (cameraFrontLength > 0.0f)
    {
        // 単位ベクトル化
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    // スティックの水平入力値をカメラ右方向に反映し、
    // スティックの垂直入力値をカメラ前方向に反映し、
    // 進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;
    vec.x = (ax * (cameraRightX)) + (ay * (cameraRightZ));
    vec.z = (ax * (cameraFrontX)) + (ay * (cameraFrontZ));

    // Y軸方向には移動しない
    vec.y = 0.0f;

    return vec;
}


// 移動入力処理
void Player::InputMove(float elapsedTime)
{
    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // 移動処理
    Move(moveVec.x, moveVec.z, maxMoveSpeed);
    //Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

    // 旋回処理
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}


// デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// 小塔判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

    // 小塔判定用のデバッグ円柱を描画
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突処理
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        DirectX::XMFLOAT3 outPosition{};
        Enemy* enemy = enemyManager.GetEnemy(i);

        //if (Collision::InstersectSphereVsSphere(position, GetRadius(),
        //    enemy->GetPosition(), enemy->GetRadius(),
        //    outPosition ))
        //{
        //    // 押し出し後の位置設定
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


            // 押し出し後の位置設定
            enemyManager.GetEnemy(i)->setPosition(outPosition);
#endif
            // 敵の真上付近に当たったかどうかを確認
            DirectX::XMVECTOR Pvec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR Evec = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR vec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(Pvec, Evec));

            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, vec);

            // 上を踏みつけていたら小ジャンプ(normalのy成分に値が入っていたら)
            if (normal.y > 0.8f)
            {
                Jump(jumpSpeed * 0.3f);
            }
            else
            {
                // 押し出し後の位置設定
                enemyManager.GetEnemy(i)->setPosition(outPosition);

            }

        }
    }
}

void Player::CollisionProjectilesVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての弾丸とすべての敵を総当たりで衝突処理
    int projectileCount = projectileManager.GetProjectileCount();
    int enemyCount = enemyManager.GetEnemyCount();

    for (int i = 0; i < projectileCount; ++i)
    {
        Projectile* projectile = projectileManager.GetProjectile(i);
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // 衝突処理
            DirectX::XMFLOAT3 outPosition;
            if (Collision::InstersectSphereVsCylinder(
                projectile->GetPosition(),
                projectile->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPosition))
            {
                // ダメージを与える
                if (enemy->ApplyDamage(1, 0.5f))
                {
                    // 吹き飛ばす
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

                    // ヒットエフェクト再生
                    {
                        DirectX::XMFLOAT3 e = enemy->GetPosition();
                        e.y += enemy->GetHeight() * 0.5f;
                        hitEffect->Play(e);
                    }


                    // 弾丸破棄
                    projectile->Destroy();
                }
            }
        }
    }

}


// ジャンプ入力処理
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

        //if (jumpRest > 0)         //　のこりジャンプ回数的なプログラム
        //{
        //    --jumpCount;
        //    Jump(jumpSpeed);
        //}
    }
}

// 接地したときに呼ばれる
void Player::OnLanding()
{
    //jumpRest = jumpLimit;
    jumpCount = 0;
}


// 弾丸入力処理
void Player::InputProjectile()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_X)
    {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.z = cosf(angle.y);      // ZX,XY,YZをかくとき初めに来るのがcosf()、後ろに来るのがsinf()と覚える
        dir.x = sinf(angle.y);      // わざとだが順番が違うので注意
        dir.y = 0.0f;

        // 発射位置
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height/2;
        pos.z = position.z;

        // 発射
        ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
        projectile->Launch(dir, pos);
        /*projectileManager.Registar(projectile);*/
    }

    if (gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.z = cosf(angle.y);      // ZX,XY,YZをかくとき初めに来るのがcosf()、後ろに来るのがsinf()と覚える
        dir.x = sinf(angle.y);      // わざとだが順番が違うので注意
        dir.y = 0.0f;

        // 発射位置
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height / 2;
        pos.z = position.z;

        // ターゲット設定
        DirectX::XMFLOAT3 target;
        target.x = position.x + dir.x * 1000.0f;
        target.y = position.y + dir.y * 1000.0f;
        target.z = position.z + dir.z * 1000.0f;

        // 一番近くの敵をターゲットにする
        float dist = FLT_MAX;
        EnemyManager& enemyManager = EnemyManager::Instance();
        int enemyCount = enemyManager.GetEnemyCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            // 敵との距離判定
            Enemy* enemy = EnemyManager::Instance().GetEnemy(i);

            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
            DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);

            float d;
            DirectX::XMStoreFloat(&d, D);

            // 暫定ターゲットよりも近い敵なら
            if (d < dist)
            {
                // 暫定ターゲットの更新
                dist = d;
                target = enemy->GetPosition();
                target.y += enemy->GetHeight() * 0.5f;
            }
        
        }


        // 発射
        ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
        projectile->Launch(dir, pos,target);
        /*projectileManager.Registar(projectile);*/
    }


}