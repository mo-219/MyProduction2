#pragma once

#include<DirectXMATH.h>
#include<Param.h>

//キャラクター
class Character
{
public:
    Character() {}
    virtual ~Character() {}

    //  ダメージを与える
    bool ApplyDamage(int damage, float invincibleTime);
    //bool ApplyDamage(int damage);

protected:
    DirectX::XMFLOAT3   collisionPosition = { 0,0,0 };   
    DirectX::XMFLOAT3   angle = { 0,0,0 };
    DirectX::XMFLOAT3   scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 transform = {
        1,0,0,0,    // X軸ベクトル(右方向ベクトルのx,y,z(右はどっちかがわかる))
        0,1,0,0,    // Y軸ベクトル(上方向のベクトル)
        0,0,1,0,    // Z軸ベクトル(前方向のベクトル)
        0,0,0,1     // 位置
    };
    // カメラのメンバのviewとはまたちょっとちがうから注意
    //XMはdirectXのMathの意味かもねん

public:

    enum CharacterName
    {
        NONE = -1,
        PLAYER = 0,
        ENEMY_RED,
        ENEMY_SLIME,
        ENEMY_GOLEM
    };

    //行列更新処理
    void UpdateTransform();


    // 座標
    const DirectX::XMFLOAT3& GetPosition() const { return param.position; }                    // 習得
    void SetPosition(const DirectX::XMFLOAT3& position) { this->param.position = position; }   // 設定

    // 回転
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }             // 取得
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }  // 設定


    // スケール
    const DirectX::XMFLOAT3& SetScale() const { return scale; }             // 取得
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }  // 設定

    // 半径取得
    float GetRadius() const { return param.radius; }

    // 地面に接地しているか
    bool IsGround() const { return isGround; }


    void SetMaxPos(DirectX::XMFLOAT3 pos) { maxPos = pos; }
    void SetMinPos(DirectX::XMFLOAT3 pos) { minPos = pos; }

    // 衝突判定用=====================================================================================================

    const DirectX::XMFLOAT3& GetCollisionPosition() const { return collisionPosition; }                    // 習得
    void SetCollisionPosition(const DirectX::XMFLOAT3& position) { collisionPosition = position; }   // 設定

    // 高さ取得
    float GetHeight() const { return param.height; }
    float GetWidth() const { return param.width; }
    float GetDepth() const { return param.depth; }

    float GetRayCastRadius() const { return param.rayCastRadius; }
    void  SetRayCastRadius(float rad) { param.rayCastRadius = rad; }


    // =================================================================================================================


    // 速力処理
    // 垂直速力更新処理
    void UpdateVerticalVelocity(float elapsedTime);

    // 垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime);

    void CalcPositionMaxMin();
 

    // 衝撃を与える
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

    // HP関連
    int GetHealth() const { return health; }            // 健康状態を取得
    int GetMaxHealth() const { return maxHealth; }      // 最大健康状態を取得
    void SetHealth(int i) { health = i; }

    // パラメータを取得
    Param GetParam() { return param; }

    CharacterName GetName() { return name; }

    bool GetIsInputMove() { return isInputMove; }
    void SetIsInputMove(bool b) { isInputMove = b; }

protected:

    //void Move(float elapsedTime, float vx, float vz, float speed);    // 移動処理

    void Move(float vx, float vz, float speed);

    void Turn(float elapsedTime, float vx, float vz, float speed);    // 旋回処理

    void Jump(float speed);    // ジャンプ処理


    void UpdateVelocity(float elapsedTime);    // 速力処理更新

    void UpdateVertical(float elapsedTime);    // 横方向移動処理更新

    void UpdateHorizontal(float elapsedTime);  // 縦方向移動処理更新


    virtual void OnLanding() {}     // 接地したときによばれる
    virtual void OnDamaged() {}     // ダメージを受けたときに呼ばれる
    virtual void OnDead() {}        // 死亡したときに呼ばれる


    void UpdateInvincibleTimer(float elapsedTime);    // 無敵時間更新


private:
    void UpdateHorizontalVelocity(float elapsedFrame);  // 水平速力更新処理

    void UpdateHorizontalMove(float elapsedTime);       // 水平移動更新処理

protected:
    CharacterName name = CharacterName::NONE;

    Param param = { DirectX::XMFLOAT3(0,0,0), 0.5f, 0.0f,2.0f,0.0f,0.0f };

    DirectX::XMFLOAT3 maxPos = {};
    DirectX::XMFLOAT3 minPos = {};

    float               gravity     = -1.0f;         // 1フレームの重力
    DirectX::XMFLOAT3   velocity    = { 0,0,0 };     // 速力
    bool                isGround    = true;

    bool                isInputMove = true;         // 移動入力処理を受け付けるかどうかのフラグ

    float               attackPower = 0;            // 攻撃力

    int                 health      = 100;          // ヘルスポイント
    int                 maxHealth   = 10;           // ヘルスポイント最大値


    float               invincibleTimer = 0.0f;     // 無敵時間


    float               friction        = 0.5f;     // 摩擦力
    float               acceleration    = 1.0f;     // 加速度
    float               maxMoveSpeed    = 5.0f;     // 移動スピード最大値
    float               moveVecX        = 0.0f;
    float               moveVecZ        = 0.0f;

    float               airControl  = 0.3f;         
                                                   

    float               stepOffset  = 1.1f;          // レイキャスト位置補正用
    float               slopeRate   = 1.0f;          // 傾斜率 


    // シェーダー用
    // キュービックカラー
    DirectX::XMFLOAT4 colorTop1     = { 1.0f, 1.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorBottom1  = { 1.0f, 0.8f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorRight1   = { 1.0f, 0.0f, 0.8f, 1.0f };
    DirectX::XMFLOAT4 colorLeft1    = { 0.0f, 1.0f, 0.3f, 1.0f };
    DirectX::XMFLOAT4 colorBack1    = { 0.2f, 0.1f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 colorFront1   = { 1.0f, 0.0f, 0.0f, 1.0f };

    DirectX::XMFLOAT4 colorTop2     = { 1.0f, 1.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorBottom2  = { 1.0f, 0.8f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorRight2   = { 1.0f, 0.0f, 0.8f, 1.0f };
    DirectX::XMFLOAT4 colorLeft2    = { 0.0f, 1.0f, 0.3f, 1.0f };
    DirectX::XMFLOAT4 colorBack2    = { 0.2f, 0.1f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 colorFront2   = { 1.0f, 0.0f, 0.0f, 1.0f };

    DirectX::XMFLOAT4 colorAlpha    = { 1,1,1,0.5f };


    // 消滅時マスクするため
    // ディゾルブ
    struct DissolveData
    {
        float				dissolveThreshold = 0.0f;
        float				edgeThreshold = 0.01f;		    // 縁の閾値
        float               maskFlag = 1;			        // 0:マスクしない   1:マスクする
        DirectX::XMFLOAT4	edgeColor = {1,0,0,1};			// 縁の色
    };
    DissolveData    dissolveData;

    float collisionRange = 10;


public:
    // ディゾルブ変化用関数
    float GetDissolveThreshold() { return dissolveData.dissolveThreshold; }
    float GetEdgeThreshold() { return dissolveData.edgeThreshold; }
    float GetMaskFlag() { return dissolveData.maskFlag; }

    void IncDissolveThreshold() { ++dissolveData.dissolveThreshold; }
    void PlusDissolveThreshold(float n) { dissolveData.dissolveThreshold += n; }

    void SetDissolveThreshold(float a) { dissolveData.dissolveThreshold = a; }
    void SetMaskFlag(float a) { dissolveData.maskFlag = a; }

};
