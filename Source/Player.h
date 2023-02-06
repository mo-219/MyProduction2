#pragma once

#include "Graphics/shader.h"
#include "Graphics/Model.h"
#include "Character.h"

#include "Sword.h"

#include "Effect.h"

#include "Audio/Audio.h"

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
    void Render(const RenderContext& rc, ModelShader* shader);


    // デバッグ用
    void DrawDebugGUI();    //GUI描画
    void DrawDebugPrimitive();  //プリミティブ描画


    // 入力処理
    bool ImputJump();                   // ジャンプ
    bool InputWeakAttack();             // 弱攻撃
    bool InputStrongAttack();           // 強攻撃
    bool InputMove(float elapsedTime);  // 移動
    bool InputDodge();                  // 避け


    // 避けステートがどうか
    bool IsDodge() { return state == State::Dodge; };

    // 生きているかどうか
    bool IsLive() { return isLive; }


    // ゲッター、セッター
    void SetStageClearFlag(bool b) { stageClearFlag = b; }
    bool GetStageClearFlag() { return stageClearFlag; }

    bool GetDodgeFlag() { return dodgeFlag; }

    float GetCurrentAP() { return AP; }
    float GetMaxAP() { return MaxAP; }


    // 計算処理
    bool CalcDodge();           // 避けSpeed計算
    void CalcAP(float value);   // アタックポイント計算
    void AddAP(float value);    // アタックポイント回復


protected:
    // 接地したときに呼ばれる
    void OnLanding() override;

    // ダメージを受けたときに呼ばれる
    void OnDamaged() override;

    // 死亡したときに呼ばれる
    void OnDead()override;

    // 攻撃したときに呼ばれる
    void OnAttacked();


private:
    // スティック入力値から移動ベクトルを取得
    DirectX::XMFLOAT3 GetMoveVec() const;


    // 衝突処理
    void CollisionPlayerVsEnemies();
    void CollisionPlayerVsStage();
    void CollisionPlayerVsObject();
    void CollisionNodeVsEnemy(const char* nodeName, float nodeRadius);


private:
    // アニメーション
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
    void TransitionIdleState();                     // 遷移
    void UpdateIdleState(float elapsedTime);        // 更新処理

    // 移動ステート
    void TransitionMoveState();                     // 遷移
    void UpdateMoveState(float elapsedTime);        // 更新処理

    // ジャンプステート
    void TransitionJumpInitState();                 // 遷移
    void UpdateJumpInitState(float elapsedTime);    // 更新処理

    // ジャンプステート
    void TransitionJumpState();                     // 遷移
    void UpdateJumpState(float elapsedTime);        // 更新処理

    // 着地ステート
    void TransitionLandingState();                  // 遷移
    void UpdateLandingState(float elapsedTime);     // 更新処理

    // 落ちステート
    void TransitionFallingState();                  // 遷移
    void UpdateFallingState(float elapsedTime);     // 更新処理


    //****************
    // 
    //  攻撃ステート
    //
    //****************
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
    //****************
    

    // 避けステート
    void TransitionDodgeState();               // 遷移
    void UpdateDodgeState(float elapsedTime);  // 更新処理


    // ダメージステート
    void TransitionDamageState();               // 遷移
    void UpdateDamageState(float elapsedTime);  // 更新処理

    // 死亡ステート
    void TransitionDeathState();                // 遷移
    void UpdateDeathState(float elapsedTime);   // 更新処理

    //---------------------------------



    // モデルデータ保存
    Model* model            = nullptr;

    // 武器保存
    Sword* sword = nullptr;


    // animation用変数
    float animationLimit = 0.0f;
    State   state = State::Idle;


    // 移動スピード用変数
    float  moveSpeed        = 5.0f;

    float  dodgeSpeed = 0.0f;
    bool   dodgeFlag = false;

    float  turnSpeed        = DirectX::XMConvertToRadians(720);     // 720回転までおk


    // ジャンプに必要な変数
    float  jumpSpeed        = 20.0f;    // ジャンプの速さ
    int    jumpCount        = 0;        // ジャンプ回数
    int    jumpLimit        = 1;        // ジャンプ回数の制限
    float  landingFallSpeed = -10.0f;   // 落ちて行くスピード

    // 攻撃用変数
    float  attackInputTimer = 0.0f;     
    bool   attackCollisionFlag = false;

    // エフェクトの保存
    Effect* hitEffect = nullptr;

    // クリアフラグ保存用
    bool stageClearFlag = false;


    // アタックポイント
    float AP = 100;
    float MaxAP = 100;

    // 生存フラグ
    bool isLive = true;     // true: 生きている


    // SE保存用変数
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