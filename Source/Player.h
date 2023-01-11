#pragma once

#include "Graphics/shader.h"
#include "Graphics/Model.h"
#include "Character.h"

#include "ProjectileManager.h"
#include "Sword.h"

#include "Effect.h"

//プレイヤー
class Player : public Character
{
public:
    // インスタンス取得
    static Player& Instance();

    Player();
    ~Player();

    // 初期化
    void Initialize(DirectX::XMFLOAT3 Respawn);

    // 更新処理
    void Update(float elapsedTime);

    void UpdateOnlyTransform(float elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Render(const RenderContext& rc, ModelShader* shader);

    // デバッグ用GUI描画
    void DrawDebugGUI();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // ジャンプ入力処理
    bool ImputJump();

    // 攻撃入力処理
    bool InputAttack();

    bool JudgeAttack(int Value);

    bool IsDodge() { return state == State::Dodge; };

    // 攻撃入力処理
    bool InputDodge();

    bool CalcDodge();

    void SetStageClearFlag(bool b) { stageClearFlag = b; }
    bool GetStageClearFlag() { return stageClearFlag; }

    bool GetDodgeFlag() { return dodgeFlag; }

    float GetCurrentAP() { return AP; }
    float GetMaxAP() { return MaxAP; }

    void CalcAP(float value);
    void AddAP(float value);

protected:
    // 接地したときに呼ばれる
    void OnLanding() override;

    // ダメージを受けたときに呼ばれる
    void OnDamaged() override;

    // 死亡したときに呼ばれる
    void OnDead()override;

    void Attacked();

private:
    // スティック入力値から移動ベクトルを取得
    DirectX::XMFLOAT3 GetMoveVec() const;


    // 移動入力処理
    bool InputMove(float elapsedTime);


    // プレイヤーとエネミーの衝突処理
    void CollisionPlayerVsEnemies();

    // 弾丸と敵の衝突処理
    void CollisionProjectilesVsEnemies();

    void CollisionPlayerVsStage();

    void CollisionPlayerVsObject();

    // 弾丸入力処理
    void InputProjectile();

private:
    // アニメーション
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
    // ステート
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
    //      アニメーション
    // 
    //--------------------------------
    
    // アニメーションステート 
    // 待機ステート
    void TransitionIdleState();                 // 遷移
    void UpdateIdleState(float elapsedTime);    // 更新処理

    // 移動ステート
    void TransitionMoveState();                 // 遷移
    void UpdateMoveState(float elapsedTime);    // 更新処理

    // ジャンプステート
    void TransitionJumpInitState();                 // 遷移
    void UpdateJumpInitState(float elapsedTime);    // 更新処理

    // ジャンプステート
    void TransitionJumpState();                 // 遷移
    void UpdateJumpState(float elapsedTime);    // 更新処理

    // 着地ステート
    void TransitionLandingState();              // 遷移
    void UpdateLandingState(float elapsedTime); // 更新処理

    // 落ちステート
    void TransitionFallingState();              // 遷移
    void UpdateFallingState(float elapsedTime); // 更新処理

    //*****
    // 
    //  攻撃ステート
    //
    // コンボ１
    void TransitionAttack1State();               // 遷移
    void UpdateAttack1State(float elapsedTime);  // 更新処理

    // コンボ2
    void TransitionAttack2State();               // 遷移
    void UpdateAttack2State(float elapsedTime);  // 更新処理

    // コンボ3
    void TransitionAttack3State();               // 遷移
    void UpdateAttack3State(float elapsedTime);  // 更新処理

    // コンボ4
    void TransitionAttack4State();               // 遷移
    void UpdateAttack4State(float elapsedTime);  // 更新処理

    //*****
    
    // ダメージステート
    void TransitionDamageState();               // 遷移
    void UpdateDamageState(float elapsedTime);  // 更新処理

    // 死亡ステート
    void TransitionDeathState();               // 遷移
    void UpdateDeathState(float elapsedTime);  // 更新処理


    // 避けステート
    void TransitionDodgeState();               // 遷移
    void UpdateDodgeState(float elapsedTime);  // 更新処理

     
    //---------------------------------


    Model* model            = nullptr;
    Sword* sword            = nullptr;

    float AnimationLimit = 0.0f;

    State   state = State::Idle;


    float  moveSpeed        = 5.0f;

    float dodgeSpeed = 0.0f;
    bool dodgeFlag = false;

    float  turnSpeed        = DirectX::XMConvertToRadians(720);     // 720回転までおk

    // ジャンプに必要な変数
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

    // 手のノード
    Model::Node* handNode = nullptr;

    // 一旦
    bool stageClearFlag = false;



    // アタックポイント
    float AP = 100;
    float MaxAP = 100;

};