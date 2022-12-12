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


// インタンス
static Player* instance = nullptr;

// インスタンス取得
Player& Player::Instance()
{
    return *instance;
}


// コンストラクタ 
Player::Player()
{
    name = CharacterName::PLAYER;

    // インスタンスポインタ設定
    instance = this;

    model = new Model("Data/Model/Player/Player.mdl");

    hitEffect = new Effect("Data/Effect/Hit.efk");

    sword = new Sword("Data/Model/Sword/Sword2.mdl");
    DirectX::XMFLOAT4X4 swordStart = model->FindNode("LeftHand")->worldTransform;     // 手元

    handNode = model->FindNode("LeftHand");   // 手のノード取得

    // 待機ステートへ遷移
    TransitionIdleState();

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.02f;

    maxMoveSpeed = 7.0f;

    health = 1000;

    SetRayCastRadius(5);
}

// デストラクタ
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

// 更新処理
void Player::Update(float elapsedTime)  //前回Updateしてから今やってる時までの時間(経過秒数)
{
    // ステート毎の処理
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

    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 弾丸入力処理
    InputProjectile();

    projectileManager.Update(elapsedTime);

    // プレイヤーと敵との衝突処理
    CollisionPlayerVsEnemies();

    CollisionProjectilesVsEnemies();

    CollisionPlayerVsStage();

    CollisionPlayerVsObject();

    UpdateOnlyTransform(elapsedTime);




}

void Player::UpdateOnlyTransform(float elapsedTime)
{
    //オブジェクト行列を更新
    UpdateTransform();

    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);

    // モデル行列更新
    model->UpdateTransform(transform);




    sword->UpdateTransform(model->FindNode("LeftHand")->worldTransform);
    DirectX::XMFLOAT4X4 tip = model->FindNode("LTip")->worldTransform;
    sword->SetTipPos(DirectX::XMFLOAT3(tip._41, tip._42, tip._43));
    sword->Update(elapsedTime);
}


// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    projectileManager.Render(dc, shader);
    shader->Draw(dc, model);
}

void Player::Render(const RenderContext& rc, ModelShader* shader)
{
    // 送るデータ挿入
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

// デバッグ用GUI描画
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
        ImGui::Text(u8"State　%s", str.c_str());

        // トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 位置
            
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

            DirectX::XMFLOAT4X4 swordStart = model->FindNode("LeftHand")->worldTransform;     // 手元
            DirectX::XMFLOAT4X4 swordEnd = model->FindNode("LTip")->worldTransform;       // 剣先
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
bool Player::InputMove(float elapsedTime)
{
    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // 移動処理
    Move(moveVec.x, moveVec.z, maxMoveSpeed);
    //Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

    // 旋回処理
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

    return !(moveVec.x * moveVec.x + moveVec.y * moveVec.y + moveVec.z * moveVec.z <= 0.0f);

}


// デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// 小塔判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

    // 小塔判定用のデバッグ円柱を描画
    debugRenderer->DrawCylinder(param.position, param.radius, param.height, DirectX::XMFLOAT4(0, 0, 0, 1));
    debugRenderer->DrawSphere(param.position, param.rayCastRadius, DirectX::XMFLOAT4{ 0,1,0,1 });


    sword->DrawDebugPrimitive();
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


            // 押し出し後の位置設定
            enemyManager.GetEnemy(i)->setPosition(outPosition);
#endif
            // 敵の真上付近に当たったかどうかを確認
            DirectX::XMVECTOR Pvec = DirectX::XMLoadFloat3(&param.position);
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

            // 敵の真上付近に当たったかどうかを確認
            DirectX::XMVECTOR Pvec = DirectX::XMLoadFloat3(&param.position);
            DirectX::XMVECTOR Evec = DirectX::XMLoadFloat3(&stage->GetPosition());
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
                if (stage->GetCollisionFlag())
                {
                    // 押し出し後の位置設定
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


// ジャンプ入力処理
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



// 押されたか判定するため何回も通るようにしておく
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

// アタックさせるかどうかはここで判定
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

// 接地したときに呼ばれる
void Player::OnLanding()
{
    //jumpRest = jumpLimit;
    jumpCount = 0;

    // ダメージ、死亡ステート時は着地したときにステート遷移しないようにする
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
    // ダメージステートへ遷移
    //TransitionDamageState();

}

void Player::OnDead()
{
    // 死亡ステートへ遷移
    //TransitionDeathState();

}

void Player::Attacked()
{
    sword->collisionWeaponVSEnemies();
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
        pos.x = param.position.x;
        pos.y = param.position.y + param.height/2;
        pos.z = param.position.z;

        // 発射
        ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
        projectile->Launch(dir, pos);
        /*projectileManager.Registar(projectile);*/
    }



}

void Player::TransitionIdleState()
{
    state = State::Idle;

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Idle, true, 1.2f);
}

void Player::UpdateIdleState(float elapsedTime)
{
    // 移動入力処理
    if (InputMove(elapsedTime))
    {
        TransitionMoveState();
    }
    // ジャンプ入力処理
    if (ImputJump())
    {
        TransitionJumpState();
    }
    // 攻撃入力処理
    if (InputAttack() && attackCount <= attackLimit)
    {
        TransitionAttack1State();
    }
    if (InputDodge())
    {
        TransitionDodgeState();
    }

    // 弾丸入力処理
    InputProjectile();
}

void Player::TransitionMoveState()
{
    state = State::Move;

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Running, true, 1.8f);

}

void Player::UpdateMoveState(float elapsedTime)
{
    // 移動入力処理
    if (!InputMove(elapsedTime))
    {
        TransitionIdleState();
    }
    // ジャンプ入力処理
    if (ImputJump())
    {
        TransitionJumpState();
    }
    // 攻撃入力処理
    if (InputAttack() && attackCount <= attackLimit)
    {
        TransitionAttack1State();
    }
    if (InputDodge())
    {
        TransitionDodgeState();
    }
    // 弾丸入力処理
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

    // 移動入力処理
    bool move = InputMove(elapsedTime);

    // ジャンプ入力処理
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

    // 移動入力処理
    bool move = InputMove(elapsedTime);

    // ジャンプ入力処理
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


    // 弾丸入力処理
    InputProjectile();


}

void Player::TransitionLandingState()
{
    state = State::Landing;

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Landing, false, 1.0f);

}

void Player::UpdateLandingState(float elapsedTime)
{
    // 移動入力処理
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
    // 待機アニメーション再生
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
