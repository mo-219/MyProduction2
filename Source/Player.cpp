#include <imgui.h>

#include "player.h"

#include "Input/input.h"
#include "Camera.h"


#include "Graphics/Graphics.h"

#include "StageManager.h"
#include "EnemyManager.h"
#include "GameObjectManager.h"
#include "Collision.h"


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
    sword->UpdateTransform(swordStart);

    // 待機ステートへ遷移
    TransitionIdleState();

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.02f;

    maxMoveSpeed = 7.0f;

    health = 1;

    SetRayCastRadius(5);

    // SE設定
    Audio& audio = Audio::Instance();
    se[SEID::SE_Sword]  = audio.LoadAudioSource("Data/Audio/SE/Sword.wav");
    se[SEID::SE_JUMP]   = audio.LoadAudioSource("Data/Audio/SE/jump.wav");
    se[SEID::SE_DODGE]  = audio.LoadAudioSource("Data/Audio/SE/Kaze.wav");
    se[SEID::SE_HIT]    = audio.LoadAudioSource("Data/Audio/SE/Hit.wav");
}

// デストラクタ
Player::~Player()
{
    delete sword;
    delete hitEffect;
    delete model;
}

// 初期化
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
        // 待機
        UpdateIdleState(elapsedTime);
        break;

    case State::Move:
        // 移動
        UpdateMoveState(elapsedTime);
        break;

    case State::Jump:
        // ジャンプ
        UpdateJumpState(elapsedTime);
        break;

    case State::Attack1:
        // コンボ１
        UpdateAttack1State(elapsedTime);
        break;    
    
    case State::Attack2:
        // コンボ2
        UpdateAttack2State(elapsedTime);
        break;    
    
    case State::Attack3:
        // コンボ3
        UpdateAttack3State(elapsedTime);
        break;    
    
    case State::Attack4:
        // コンボ4
        UpdateAttack4State(elapsedTime);
        break;

    case State::Dodge:
        // 避け
        UpdateDodgeState(elapsedTime);
        break;

    case State::Falling:
        // 落ち
        UpdateFallingState(elapsedTime);
        break;

    case State::Landing:
        // 着地
        UpdateLandingState(elapsedTime);
        break;

    case State::Damage:
        // ダメージ
        UpdateDamageState(elapsedTime);
        break;

    case State::Dead:
        // 死亡
        UpdateDeathState(elapsedTime);
        break;

    }

    attackInputTimer -= 0.1f;
    if (attackInputTimer <= 0.0f)
    {
        attackInputTimer = 0.0f;
    }

    UpdateInvincibleTimer(elapsedTime);

    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // トランスフォーム更新処理
    UpdateOnlyTransform(elapsedTime);


    // 衝突処理
    CalcPositionMaxMin();

    CollisionPlayerVsEnemies();

    CollisionPlayerVsStage();

    CollisionPlayerVsObject();


    // AP回復
    AddAP(0.1f);
}

// トランスフォーム更新処理
void Player::UpdateOnlyTransform(float elapsedTime)
{
    //オブジェクト行列を更新
    UpdateTransform();

    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);

    // モデル行列更新
    model->UpdateTransform(transform);


    // 初期化見直す
    sword->Update(elapsedTime);
    sword->UpdateTransform(model->FindNode("LeftHand")->worldTransform);
    DirectX::XMFLOAT4X4 tip = model->FindNode("LTip")->worldTransform;
    sword->SetTipPos(DirectX::XMFLOAT3(tip._41, tip._42, tip._43));
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


    sword->Render(rc,shader);
    shader->Draw(prc, model);
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



//---------------------------------------------------
//
//         衝突処理
//
//---------------------------------------------------

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


        if (Collision::InstersectCylinderVsCylinder(enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
                                                    param.position, GetRadius(),GetHeight(),
                                                    outPosition))
        {

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
                param.position = outPosition;

            }

        }
    }
}

// プレイヤーとステージの衝突処理
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

// プレイヤーとステージオブジェクトの衝突処理
void Player::CollisionPlayerVsObject()
{
    ObjectManager& objManager = ObjectManager::Instance();
    int objCount = objManager.GetObjectCount();

    for (int i = 0; i < objCount; i++)
    {
        DirectX::XMFLOAT3 outPosition{};
        GameObject* obj = objManager.GetObj(i);
        obj->CollisionFlag = false;
        //if (obj->collision == &noneBehavior) continue;
        if (obj->GetInvincibleTimer() > 0.0f)continue;
        if (obj->GetBehavior()->collision(obj->GetParam(), this->GetParam(), outPosition))
        {
            obj->Hit(this, outPosition);
            obj->CollisionFlag = true;
            
        }
    }

}

// 任意のノードと敵の衝突処理
void Player::CollisionNodeVsEnemy(const char* nodeName, float nodeRadius)
{
    Model::Node* node = model->FindNode(nodeName);
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();


    if (node != nullptr)
    {
        // ノードのワールド座標
        DirectX::XMFLOAT3 nodePosition(node->worldTransform._41, node->worldTransform._42, node->worldTransform._43);



        // プレイヤーと当たり判定
        DirectX::XMFLOAT3 outPosition;
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            if (Collision::InstersectSphereVsCylinder(nodePosition, nodeRadius,
                enemy->GetCollisionPosition(), enemy->GetRadius(), enemy->GetHeight(),
                outPosition))
                DirectX::XMFLOAT3 outPosition;
            {
                // ダメージを与える
                if (enemy->ApplyDamage(1, 1.0f))
                {
                    // 吹き飛ばす
                    {
                        DirectX::XMFLOAT3 impulse;
                        const float power = 8.0f;

                        const DirectX::XMFLOAT3& e = enemy->GetPosition();
                        const DirectX::XMFLOAT3& p = nodePosition;

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

                    }
                }
            }
        }
    }
}





//---------------------------------------------------
//
//      入力処理
//
//---------------------------------------------------

// 移動入力処理
bool Player::InputMove(float elapsedTime)
{
    if (!isInputMove)    return false;

    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // 移動処理
    Move(moveVec.x, moveVec.z, maxMoveSpeed);

    // 旋回処理
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

    return !(moveVec.x * moveVec.x + moveVec.y * moveVec.y + moveVec.z * moveVec.z <= 0.0f);

}

// ジャンプ入力処理
bool Player::ImputJump()
{
    if (!isInputMove)    return false;

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

// 弱攻撃入力処理
bool Player::InputWeakAttack()
{
    if (!isInputMove)    return false;

    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_B)
    {
        attackInputTimer = 2.0f;
        return true;
    }
    return false;
}

// 強攻撃入力処理
bool Player::InputStrongAttack()
{
    if (!isInputMove)    return false;

    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_X)
    {
        attackInputTimer = 2.0f;
        return true;
    }
    return false;
}

// Dodge入力処理
bool Player::InputDodge()
{
    if (!isInputMove)    return false;

    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_Y) return true;
    return false;
}

// Dodge時スピード処理
bool Player::CalcDodge()
{
    if (dodgeSpeed < 0.0f)return false;

    dodgeSpeed -= 0.3f;

    DirectX::XMFLOAT3 front = DirectX::XMFLOAT3(transform._31, transform._32, transform._33);
    param.position.z += front.z * dodgeSpeed;
    param.position.x += front.x * dodgeSpeed;
}




//---------------------------------------------------
//
//      パラメータ計算処理
//
//---------------------------------------------------

void Player::CalcAP(float Value)
{
    AP -= Value;
    if (AP <= 0) AP = 0;
}

void Player::AddAP(float Value)
{
    AP += Value;
    if (AP >= MaxAP) AP = MaxAP;
}


//---------------------------------------------------
//
//      行動したとき呼ばれる処理処理
//
//---------------------------------------------------
// 接地したときに呼ばれる
void Player::OnLanding()
{
    jumpCount = 0;


    if (dodgeFlag)   return;
        if (velocity.y < landingFallSpeed)
        {
            TransitionLandingState();
        }
    
}

// ダメージ受けたときに呼ばれる
void Player::OnDamaged()
{
    // ダメージステートへ遷移
    TransitionDamageState();

}

// 死亡したときに呼ばれる
void Player::OnDead()
{
    // 死亡ステートへ遷移
    TransitionDeathState();

}

// 攻撃したときに呼ばれる
void Player::OnAttacked()
{
    sword->collisionWeaponVSEnemies();
}


//---------------------------------------------------
//
//      アニメーション処理処理
//
//---------------------------------------------------

 // 待機アニメーションに遷移
void Player::TransitionIdleState()
{
    state = State::Idle;

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Idle, true, 1.2f);
}

// 待機アニメーション更新処理
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
    if(GetCurrentAP()-5 >= 0)
    {
        if (InputWeakAttack())
        {
            TransitionAttack1State();
        }
    }
    if (GetCurrentAP()-30 >= 0)
    {
        if (InputDodge())
        {
            TransitionDodgeState();
        }
    }

    // 弾丸入力処理
    SetCollisionPosition(GetPosition());
}


// 移動アニメーションに遷移
void Player::TransitionMoveState()
{
    state = State::Move;

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Running, true, 1.8f);

}

// 移動アニメーション更新処理
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
    if (GetCurrentAP() - 5 >= 0)
    {
        if (InputWeakAttack())
        {
            TransitionAttack1State();
        }
    }
    if (GetCurrentAP() - 30 >= 0)
    {
        if (InputDodge())
        {
            TransitionDodgeState();
        }
    }
    SetCollisionPosition(GetPosition());
}


// ジャンプ開始アニメーションに遷移
void Player::TransitionJumpInitState()
{
    CalcAP(10);
    state = State::Jump;

    model->PlayAnimation(Anim_Jump_Up1, true, 1.0f);
    se[SEID::SE_JUMP]->Play(false);

}

// ジャンプ開始アニメーション更新処理
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


    SetCollisionPosition(GetPosition());
}


// ジャンプアニメーションに遷移
void Player::TransitionJumpState()
{
    state = State::Jump;

    model->PlayAnimation(Anim_Jump_Peak, true, 1.0f);

}

// ジャンプアニメーション更新処理
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


    SetCollisionPosition(GetPosition());
}


// 落ちアニメーションに遷移
void Player::TransitionFallingState()
{
    state = State::Falling;
    // 待機アニメーション再生
    model->PlayAnimation(Anim_Jump_Down1, true);
}

// 落ちアニメーション更新処理
void Player::UpdateFallingState(float elapsedTime)
{
    InputMove(elapsedTime);
    if (InputDodge())
    {
        TransitionDodgeState();
    }
    SetCollisionPosition(GetPosition());
}


// 着地アニメーションに遷移
void Player::TransitionLandingState()
{
    state = State::Landing;

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Landing, false, 1.0f);

}

// 着地アニメーション更新処理
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
    SetCollisionPosition(GetPosition());
}

//************************
// 
//  攻撃アニメーション
//
//************************

// 弱攻撃1アニメーションに遷移
void Player::TransitionAttack1State()
{
    CalcAP(5);
    state = State::Attack1;
    animationLimit = 0.3f;
    model->PlayAnimation(Anim_Combo1, false, 0.9f);

    se[SEID::SE_Sword]->Play(false);
}

// 弱攻撃1アニメーション更新処理
void Player::UpdateAttack1State(float elapsedTime)
{
    OnAttacked();

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
    else if (model->GetCurrentAnimetionSeconds() >= animationLimit)
    {
        if (GetCurrentAP() - 10 >= 0)
        {
            if (InputWeakAttack())
            {
                TransitionAttack2State();
            }
        }
    }
    SetCollisionPosition(GetPosition());

}


// 弱攻撃2アニメーションに遷移
void Player::TransitionAttack2State()
{
    CalcAP(10);
    state = State::Attack2;
    animationLimit = 0.5f;
    model->PlayAnimation(Anim_Combo2, false, 0.9f);

    se[SEID::SE_Sword]->Play(false);

}

// 弱攻撃2アニメーション更新処理
void Player::UpdateAttack2State(float elapsedTime)
{
    OnAttacked();

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
    else if (model->GetCurrentAnimetionSeconds() >= animationLimit)
    {
        if (GetCurrentAP() - 10 >= 0)
        {
            if (InputWeakAttack())
            {
                TransitionAttack1State();
            }
        }
        if (GetCurrentAP() - 15 >= 0)
        {
            if (InputStrongAttack())
            {
                TransitionAttack3State();
            }
        }
    }
    SetCollisionPosition(GetPosition());
}


// 強攻撃1アニメーションに遷移
void Player::TransitionAttack3State()
{
    CalcAP(15);
    state = State::Attack3;
    animationLimit = 0.5f;
    model->PlayAnimation(Anim_Combo3, false, 0.9f);

    se[SEID::SE_Sword]->Play(false);
}

// 強攻撃1アニメーション更新処理
void Player::UpdateAttack3State(float elapsedTime)
{
    OnAttacked();


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
    else if (model->GetCurrentAnimetionSeconds() >= animationLimit)
    {
        if (GetCurrentAP() - 40 >= 0)
        {
            if (InputStrongAttack())
            {
                TransitionAttack4State();
            }
        }
    }
    SetCollisionPosition(GetPosition());

    //CollisionNodeVsEnemy("LeftHand", 1.0f);
}


// 強攻撃2アニメーションに遷移
void Player::TransitionAttack4State()
{
    CalcAP(20);
    state = State::Attack4;
    animationLimit = 1.0f;
    model->PlayAnimation(Anim_Combo4, false, 0.9f);

    se[SEID::SE_Sword]->Play(false);
}

// 強攻撃2アニメーション更新処理
void Player::UpdateAttack4State(float elapsedTime)
{
    OnAttacked();

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
    SetCollisionPosition(GetPosition());

    //CollisionNodeVsEnemy("LeftHand", 1.0f);
}


// ダメージアニメーションに遷移
void Player::TransitionDamageState()
{
    state = State::Damage;
    model->PlayAnimation(Anim_HitFront, false, 0.7f);
    se[SEID::SE_HIT]->Play(false);
}

// ダメージアニメーション更新処理
void Player::UpdateDamageState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        TransitionIdleState();
    }
    SetCollisionPosition(GetPosition());
}


// 死亡アニメーションに遷移
void Player::TransitionDeathState()
{
    state = State::Dead;
    model->PlayAnimation(Anim_Death, false);


}

// 死亡アニメーション更新処理
void Player::UpdateDeathState(float elapsedTime)
{
    SetCollisionPosition(GetPosition());
    if (!model->IsPlayAnimation())
    {
        // 死亡
        isLive = false;
    }
}


// 避けアニメーションに遷移
void Player::TransitionDodgeState()
{
    CalcAP(30);
    state = State::Dodge;
    dodgeSpeed = 12.0f;
    dodgeFlag = true;
    model->PlayAnimation(Anim_Dodge, false, 4.0f);
    SetCollisionPosition(GetPosition());
    
    se[SEID::SE_DODGE]->Play(false);

}

// 避けアニメーション更新処理
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
    SetCollisionPosition(GetCollisionPosition());
}




//---------------------------------------------------
//
//      デバッグ用処理
//
//---------------------------------------------------

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
            ImGui::InputFloat3("position", &param.position.x);
            DirectX::XMFLOAT4X4 nodeTransform = model->FindNode("LeftHand")->worldTransform;
            if (ImGui::TreeNode("LeftHand"))
            {
                ImGui::InputFloat4("Transform1", &transform.m[0][0]);
                ImGui::InputFloat4("Transform2", &transform.m[1][0]);
                ImGui::InputFloat4("Transform3", &transform.m[2][0]);
                ImGui::InputFloat4("Transform4", &transform.m[3][0]);
                ImGui::TreePop();
            }
            nodeTransform = model->FindNode("Spine")->worldTransform;
            if (ImGui::TreeNode("Spine"))
            {
                ImGui::InputFloat4("Transform1", &nodeTransform.m[0][0]);
                ImGui::InputFloat4("Transform2", &nodeTransform.m[1][0]);
                ImGui::InputFloat4("Transform3", &nodeTransform.m[2][0]);
                ImGui::InputFloat4("Transform4", &nodeTransform.m[3][0]);
                ImGui::TreePop();
            }
            nodeTransform = model->FindNode("Spine")->worldTransform;
            if (ImGui::TreeNode("LeftShoulder"))
            {
                ImGui::InputFloat4("Transform1", &nodeTransform.m[0][0]);
                ImGui::InputFloat4("Transform2", &nodeTransform.m[1][0]);
                ImGui::InputFloat4("Transform3", &nodeTransform.m[2][0]);
                ImGui::InputFloat4("Transform4", &nodeTransform.m[3][0]);
                ImGui::TreePop();
            }

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




// デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// 小塔判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(param.position, param.radius, DirectX::XMFLOAT4(0, 0, 0, 1));

    // 小塔判定用のデバッグ円柱を描画
    //debugRenderer->DrawCylinder(collisionPosition, param.radius, param.height, DirectX::XMFLOAT4(0, 0, 0, 1));
    //debugRenderer->DrawSphere(param.position, param.rayCastRadius, DirectX::XMFLOAT4{ 0,1,0,1 });


    //sword->DrawDebugPrimitive();
}


