#include "EnemyBoss.h"

#include <imgui.h>

#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "player.h"
#include "Collision.h"

#include "GameObjectManager.h"
#include "ItemObj.h"



// コンストラクタ
EnemyBoss::EnemyBoss()
{
    name = CharacterName::ENEMY_GOLEM;

    model = new Model("Data/model/Spider/Spider.mdl");

    // モデルが大きいのでスケーリング
    //scale.x = scale.y = scale.z = 0.001f;
    scale = { 0.09f,  0.09f, 0.09f };
    param.radius = 1.5f;
    param.height = 4.0f;
    param.rayCastRadius = 10.0f;


    dissolveData.maskFlag = 1.0f;
    dissolveData.dissolveThreshold = 0.0f;

    SetRayCastRadius(5);

    SetTerritory(param.position, 10.0f);

    // 待機ステートへ遷移
    TransitionIdleState();

    health = 1;

    searchRange = 5.0f;
    attackRange = 3.0f;
}

// デストラクタ
EnemyBoss:: ~EnemyBoss()
{
    delete model;
}

// 更新処理
void EnemyBoss::Update(float elapsedTime)
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

    case State::Attack:
        UpdateAttack1State(elapsedTime);
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

    // 速力処理
    UpdateVelocity(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);

    CalcPositionMaxMin();

    UpdateOnlyTransform(elapsedTime);
}

void EnemyBoss::UpdateOnlyTransform(float elapsedTime)
{
    // オブジェクト行列を更新
    UpdateTransform();

    // モデル行列更新
    model->UpdateAnimation(elapsedTime);
    model->UpdateTransform(transform);
}

// 描画処理
void EnemyBoss::Render(ID3D11DeviceContext* dc, Shader* shader)    // ID3D11DeviceContextがdcになってる　エラー出たらcontextに変更
{
    shader->Draw(dc, model);
}

void EnemyBoss::Render(const RenderContext& rc, ModelShader* shader)    // ID3D11DeviceContextがdcになってる　エラー出たらcontextに変更
{
    shader->Draw(rc, model);
}

RenderContext EnemyBoss::SetRenderContext(const RenderContext& rc)
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

void EnemyBoss::DrawDebugImGui()
{

    ImGui::SliderFloat("dissolveThreshold", &dissolveData.dissolveThreshold, 0.0f, 1.0f);

    ImGui::SliderFloat("dissolveFlag", &dissolveData.maskFlag, 0.0f, 1.0f);

}

void EnemyBoss::DrawDebugPrimitive()
{
    // 基底クラスのデバッグプリミティブ描画
    Enemy::DrawDebugPrimitive();
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // 縄張り範囲をデバッグ円柱描画
    debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0, 1, 0, 1));

    // 索敵範囲をデバッグ円柱描画
    debugRenderer->DrawCylinder(param.position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

    // ターゲット位置をデバッグ球描画
    debugRenderer->DrawSphere(targetPosition, param.radius, DirectX::XMFLOAT4(1, 1, 0, 0));

    DirectX::XMFLOAT3 pos = model->FindNode("RigTip")->translate;
    debugRenderer->DrawSphere(pos, 1.0f, DirectX::XMFLOAT4(1, 1, 0, 0));

}

void EnemyBoss::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
    Player& player = Player::Instance();


    // ノードの位置と当たり判定を行う
    Model::Node* node = model->FindNode(nodeName);

    if (node != nullptr)
    {
        // ノードのワールド座標
        DirectX::XMFLOAT3 nodePosition(node->worldTransform._41, node->worldTransform._42, node->worldTransform._43);

        // 当たり判定表示
        Graphics::Instance().GetDebugRenderer()->DrawSphere(nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1));

        // プレイヤーと当たり判定
        DirectX::XMFLOAT3 outPosition;

        if (Collision::InstersectSphereVsCylinder(nodePosition, nodeRadius,
            player.GetCollisionPosition(), player.GetRadius(), player.GetHeight(),
            outPosition))
        {
            // ダメージを与える
            if (!player.IsDodge())
            {
                if (player.ApplyDamage(1, 0.5f))
                {
                    // 敵を吹っ飛ばすベクトルを算出
                    DirectX::XMFLOAT3 vec;
                    vec.x = outPosition.x - nodePosition.x;
                    vec.z = outPosition.z - nodePosition.z;

                    float length = sqrtf(vec.x * vec.x + vec.z * vec.z);
                    vec.x /= length;
                    vec.z /= length;

                    // XZ平面に吹っ飛ばす力をかける
                    float power = 10.0f;
                    vec.x *= power;
                    vec.z *= power;

                    // Y方向の力
                    vec.y = 5.0f;

                    // 吹っ飛ばす
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

bool EnemyBoss::OnMessage(const Telegram& telegram)
{
    switch (telegram.msg)
    {
        // 他のエネミーから呼ばれた
    case MESSAGE_TYPE::MsgCallHelp:
        if (!SearchPlayer())
        {
            //// プレイヤーを見つけていないときに1層目ステートをReceiveに変更
            //stateMachine->SetState(static_cast<int>(State::Recieve));
        }
        return true;

        // メタAIから所有権を与えられたとき
    case MESSAGE_TYPE::MsgGiveAttackRight:
        // 攻撃フラグをtrueに設定
        attackFlg = true;

        return true;
    }

    return false;
}




// 死亡処理
void EnemyBoss::OnDead()
{
    // 死亡ステートへ
    TransitionDeathState();

}

void EnemyBoss::OnDamaged()
{
    // ダメージステートへ
    TransitionDamageState();
}

void EnemyBoss::SetRamdomTargetPosition()
{
    float a = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float r = Mathf::RandomRange(0.0f, territoryRange);  // raange

    targetPosition.x = territoryOrigin.x + r * sinf(a);
    targetPosition.y = territoryOrigin.y;
    targetPosition.z = territoryOrigin.z + r * cosf(a);
}

void EnemyBoss::MoveToTarget(float elapsedTime, float speedRate)
{
    // ターゲット方向へ進行ベクトルを算出
    float vx = targetPosition.x - param.position.x;
    float vz = targetPosition.z - param.position.z;

    float dist = sqrtf(vx * vx + vz * vz);
    vx /= dist;
    vz /= dist;

    // 移動処理
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed);
}

bool EnemyBoss::SearchPlayer()
{
    // プレイヤーとの高低差を考慮して3Dでの距離判定をする
    const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();

    float vx = playerPosition.x - param.position.x;
    float vy = playerPosition.y - param.position.y;
    float vz = playerPosition.z - param.position.z;

    float dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < searchRange)
    {
        float distXZ = sqrtf(vx * vx + vz * vz);

        // 単位ベクトル化
        vx /= distXZ;
        vz /= distXZ;

        // 前方ベクトル
        float frontX = sinf(angle.y);
        float frontZ = cosf(angle.y);

        // 2つのベクトルの内積値で前後判定
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)
        {
            return true;
        }
    }
    return false;
}

Player* EnemyBoss::searchPlayer()
{
    // プレイヤーとの高低差を考慮して3Dでの距離判定をする
    const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();

    float vx = playerPosition.x - param.position.x;
    float vy = playerPosition.y - param.position.y;
    float vz = playerPosition.z - param.position.z;

    float dist = vx * vx + vy * vy + vz * vz;

    if (dist < searchRange * searchRange)
    {
        float distXZ = sqrtf(vx * vx + vz * vz);

        // 単位ベクトル化
        vx /= distXZ;
        vz /= distXZ;

        // 前方ベクトル
        float frontX = sinf(angle.y);
        float frontZ = cosf(angle.y);

        // 2つのベクトルの内積値で前後判定
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
//      ステート
//
//-----------------------------------------------


// 徘徊ステートへ遷移
void EnemyBoss::TransitionWanderState()
{
    state = State::Wander;
    moveSpeed = 1.5f;
    // 目標地点設定
    SetRamdomTargetPosition();

    // 歩きアニメーション再生
    model->PlayAnimation(Animation::Anim_ForwardWalk, true, 0.7f);
}

// 徘徊ステート更新処理
void EnemyBoss::UpdateWanderState(float elapsedTime)
{
    // 目標地点までXZ平面での距離判定
    float vx = targetPosition.x - param.position.x;
    float vz = targetPosition.z - param.position.z;
    float distSq = vx * +vx + vz * vz;

    if (distSq < param.radius * param.radius)
    {
        TransitionIdleState();
    }
    if (SearchPlayer())
    {
        // プレイヤー見つけたら
        TransitionPursuitState();
    }

    MoveToTarget(elapsedTime, 0.5f);

}


// 待機ステートへ遷移
void EnemyBoss::TransitionIdleState()
{
    state = State::Idle;

    // タイマーをランダム設定
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Idle, true);
}

// 待機ステート更新処理
void EnemyBoss::UpdateIdleState(float elapsedTime)
{
    // タイマー処理
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // 徘徊ステートへ遷移
        TransitionWanderState();
    }

    if (SearchPlayer())
    {
        // プレイヤー見つけたら
        TransitionPursuitState();
    }
}


// 追跡ステート遷移
void EnemyBoss::TransitionPursuitState()
{
    state = State::Pursuit;
    moveSpeed = 3.0f;
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    model->PlayAnimation(Anim_ForwardRun, true);


    // エネミーからメタAIにMSG_CALL_HELPを送信
    Meta::Instance().SendMessaging(GetId(), 0, MESSAGE_TYPE::MsgCallHelp);

}

//追跡ステート更新処理
void EnemyBoss::UpdatePursuitState(float elapsedTime)
{
    // 目標地点をプレイヤー位置に設定
    targetPosition = Player::Instance().GetPosition();

    // 目標地点へ移動
    MoveToTarget(elapsedTime, 1.0f);

    // タイマー処理
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // 待機ステートへ遷移
        TransitionIdleState();
    }

    // プレイヤーが一定距離より近くにいたら高げっき
    float vx = targetPosition.x - param.position.x;
    float vy = targetPosition.y - param.position.y;
    float vz = targetPosition.z - param.position.z;

    float dist = sqrtf(vx * vx + vy * vy + vz * vz);
    if (dist < attackRange)
    {
        // 攻撃ステートへ
        TransitionAttackInitState();
    }
}

void EnemyBoss::TransitionAttackInitState()
{
    state = State::AttackInit;

    if (attackFlg)
    {
        // 攻撃終わった時に攻撃権の放棄
        // 攻撃フラグをfalseに

        SetAttackFlg(false);
        // エネミーからメタAIへ MsgChangeAttackRight を送信する	
        Meta::Instance().SendMessaging(GetId(), 0, MESSAGE_TYPE::MsgChangeAttackRight);
    }
    if (!attackFlg)
    {
        // 攻撃権を依頼
        Meta::Instance().SendMessaging(GetId(), 0, MESSAGE_TYPE::MsgAskAttackRight);

        // 待機アニメーション再生
        model->PlayAnimation(Anim_Idle, true);
    }
}

void EnemyBoss::UpdateAttackInitState(float elapsedTime)
{
    targetPosition = Player::Instance().Instance().GetPosition();

    // プレイヤーが攻撃範囲にいた場合は攻撃ステートへ遷移
    float vx = targetPosition.x - param.position.x;
    float vy = targetPosition.y - param.position.y;
    float vz = targetPosition.z - param.position.z;

    float dist = sqrtf(vx * vx + vy * vy + vz * vz);
    if (attackFlg)
    {
        // 攻撃ステートへ
        TransitionAttack1State();
    }
    else
    {
        // 攻撃準備中（待機）
        TransitionIdleBattleState();
    }

    MoveToTarget(elapsedTime, 0.0f);

}

void EnemyBoss::TransitionAttack1State()
{
    state = State::Attack;

    //// 攻撃権がなければ
    //if (!attackFlg)
    //{
    //    // 攻撃権を依頼
    //    Meta::Instance().SendMessaging(GetId(), 0, MESSAGE_TYPE::MsgAskAttackRight);
    //}
    //// 攻撃権があれば
    //else
    //{
    DropHeelItem = 0;
    model->PlayAnimation(Anim_Attack1, false);
    //}
}

void EnemyBoss::UpdateAttack1State(float elapsedTime)
{
    // 攻撃権があるとき
    if (attackFlg)
    {
        // 任意のアニメーション再生区画でのみ衝突判定処理をする
        float animationTime = model->GetCurrentAnimetionSeconds();

        if (animationTime >= 1.0f && animationTime <= 2.0f)
        {
            // 目玉ノードとプレイヤーの衝突処理
            CollisionNodeVsPlayer("RigTip", 1.0f);
        }
        if (!model->IsPlayAnimation())
        {
            TransitionIdleBattleState();
        }
    }
    // 攻撃権がないとき
    else
    {
        TransitionIdleBattleState();
    }
}

void EnemyBoss::TransitionAttack2State()
{
    state = State::Attack;

    // 攻撃権がなければ
    if (!attackFlg)
    {
        // 攻撃権を依頼
        Meta::Instance().SendMessaging(GetId(), 0, MESSAGE_TYPE::MsgAskAttackRight);
    }
    // 攻撃権があれば
    else
    {
        DropHeelItem = 0;
        model->PlayAnimation(Anim_Attack2, false);
    }
}

void EnemyBoss::UpdateAttack2State(float elapsedTime)
{
    // 攻撃権があれば
    if (attackFlg)
    {
        // 任意のアニメーション再生区画でのみ衝突判定処理をする
        float animationTime = model->GetCurrentAnimetionSeconds();

        if (animationTime >= 0.1f && animationTime <= 0.35f)
        {
            // 目玉ノードとプレイヤーの衝突処理
            //CollisionNodeVsPlayer("EyeBall", 0.2f);
        }

        if (!model->IsPlayAnimation())
        {
            TransitionIdleBattleState();
        }
    }
    // 攻撃権がなければ
    else
    {
        TransitionIdleBattleState();
    }
}

void EnemyBoss::TransitionIdleBattleState()
{
    state = State::IdleBattle;

    if (attackFlg)
    {
        // 攻撃終わった時に攻撃権の放棄
        // 攻撃フラグをfalseに

        SetAttackFlg(false);
        // エネミーからメタAIへ MsgChangeAttackRight を送信する	
        Meta::Instance().SendMessaging(GetId(), 0, MESSAGE_TYPE::MsgChangeAttackRight);
    }


    // タイマーをランダム設定
    stateTimer = Mathf::RandomRange(1.0f, 2.0f);

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Idle, true);

}

void EnemyBoss::UpdateIdleBattleState(float elapsedTime)
{
    targetPosition = Player::Instance().Instance().GetPosition();

    // タイマー処理
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // プレイヤーが攻撃範囲にいた場合は攻撃ステートへ遷移
        float vx = targetPosition.x - param.position.x;
        float vy = targetPosition.y - param.position.y;
        float vz = targetPosition.z - param.position.z;

        float dist = sqrtf(vx * vx + vy * vy + vz * vz);
        if (dist < attackRange)
        {
            // 攻撃したい
            TransitionAttackInitState();
        }
        else
        {
            // 徘徊ステートへ
            TransitionWanderState();
        }
    }
    MoveToTarget(elapsedTime, 0.0f);
}

void EnemyBoss::TransitionDamageState()
{
    state = State::Damage;
    model->PlayAnimation(Anim_Damage, false);
}


void EnemyBoss::UpdateDamageState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

void EnemyBoss::TransitionDeathState()
{
    state = State::Death;
    model->PlayAnimation(Anim_Die, false);
}

void EnemyBoss::UpdateDeathState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        SetMaskFlag(0);
        PlusDissolveThreshold(0.01f);
        if (GetDissolveThreshold() >= 1.0f)
        {
            Destroy();
        }
    }
}