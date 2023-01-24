#pragma once
#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

class Player;

// 赤鬼
class EnemyRed : public Enemy
{
public:
    EnemyRed();
    //EnemySlime(EnemyManager* mane) : Enemy(mane);
    ~EnemyRed() override;

    // 更新処理
    void Update(float elapsedTime) override;
    void UpdateOnlyTransform(float elapsedTime)override;
    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;
    void Render(const RenderContext& rc, ModelShader* shader) override;
    RenderContext SetRenderContext(const RenderContext& rc) override;

    // デバッグ用GUI描画
    void DrawDebugImGui();
    void DrawDebugPrimitive() override;


    // ノードとプレイヤーの衝突
    void CollisionNodeVsPlayer(const char* nodeName, float nodeRadius);


protected:
    // 死亡したときに呼ばれる
    void OnDead()override;

    // ダメージを受けたときに呼ばれる
    void OnDamaged()override;

    // ターゲット位置をランダム設定
    void SetRamdomTargetPosition();

    // 目標地点へ移動
    void MoveToTarget(float elapsedTime, float speedRate);

    // プレイヤー索敵
    bool SearchPlayer();

    Player* searchPlayer(); // プレイヤーが複数になった時見つけたプレイヤーを返す


    //---------------------------------------
    //
    //      ステート用
    //
    //---------------------------------------

    // 徘徊ステート
    void TransitionWanderState();                    // 遷移
    void UpdateWanderState(float elapsedTime);       // 更新処理

    // 待機ステート
    void TransitionIdleState();                      // 遷移
    void UpdateIdleState(float elapsedTime);         // 更新処理

    // 追跡ステート
    void TransitionPursuitState();                   // 遷移
    void UpdatePursuitState(float elapsedTime);      // 更新処理
    
    // 攻撃開始ステート
    void TransitionAttackInitState();                    // 遷移
    void UpdateAttackInitState(float elapsedTime);       // 更新処理

    // 攻撃1ステート
    void TransitionAttack1State();                    // 遷移
    void UpdateAttack1State(float elapsedTime);       // 更新処理

    // 攻撃2ステート
    void TransitionAttack2State();                    // 遷移
    void UpdateAttack2State(float elapsedTime);       // 更新処理

    // 戦闘待機ステート
    void TransitionIdleBattleState();                // 遷移
    void UpdateIdleBattleState(float elapsedTime);   // 更新処理

    // ダメージステート
    void TransitionDamageState();                // 遷移
    void UpdateDamageState(float elapsedTime);   // 更新処理

    // ダメージステート
    void TransitionDeathState();                // 遷移
    void UpdateDeathState(float elapsedTime);   // 更新処理

    bool OnMessage(const Telegram& telegram)override;



private:
    Model* model = nullptr;

    // ステート
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
    // アニメーション
    enum Animation
    {
        Anim_Error = -1,
        Anim_Tpose = 0,
        Anim_Attack1,
        Anim_Attack2,
        Anim_Die,
        Anim_Hit,
        Anim_Idle,
        Anim_Jump,
        Anim_Running,
        Anim_Walking,

    };

    State               state = State::Wander;




    float               moveSpeed = 2.0f;
    float               turnSpeed = DirectX::XMConvertToRadians(360);

    float               stateTimer = 0.0f;
    //float               searchRange = 5.0f;

    //float               attackRange = 5.0f;

    float               fovCos = cosf(DirectX::XMConvertToRadians(45));
};

