#pragma once

#include<DirectXMATH.h>

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
    DirectX::XMFLOAT3   position = { 0,0,0 };
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
    //行列更新処理
    void UpdateTransform();

    // 位置習得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    // 位置設定
    void setPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // 回転取得
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }

    // 回転設定
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    // スケール取得
    const DirectX::XMFLOAT3& SetScale() const { return scale; }

    // スケール取得
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // 半径取得
    float GetRadius() const { return radius; }

    // 地面に接地しているか
    bool IsGround() const { return isGround; }

    // 高さ取得
    float GetHeight() const { return height; }



    // 09_ 速力処理
    // 垂直速力更新処理
    void UpdateVerticalVelocity(float elapsedTime);

    // 垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime);

    // 衝撃を与える
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

    int GetHealth() const { return health; }            // 健康状態を取得
    int GetMaxHealth() const { return maxHealth; }      // 最大健康状態を取得


protected:

    //void Move(float elapsedTime, float vx, float vz, float speed);    // 移動処理

    void Move(float vx, float vz, float speed);

    void Turn(float elapsedTime, float vx, float vz, float speed);    // 旋回処理

    void Jump(float speed);    // ジャンプ処理


    void UpdateVelocity(float elapsedTime);    // 速力処理更新


    virtual void OnLanding() {}    // 接地したときによばれる

    virtual void OnDamaged() {}    // ダメージを受けたときに呼ばれる

    virtual void OnDead() {}    // 死亡したときに呼ばれる

    void UpdateInvincibleTimer(float elapsedTime);    // 無敵時間更新


private:
    void UpdateHorizontalVelocity(float elapsedFrame);  // 水平速力更新処理

    void UpdateHorizontalMove(float elapsedTime);       // 水平移動更新処理

protected:
    float               radius = 0.5f;

    float               gravity = -1.0f;          // 1フレームの重力
    DirectX::XMFLOAT3   velocity = { 0,0,0 };     // 速力
    bool                isGround = false;

    float               height = 2.0f;

    int                 health = 5;
    int                 maxHealth = 5;

    float               invincibleTimer = 0.0f;
    float               friction = 0.5f;    

    float               acceleration = 1.0f;
    float               maxMoveSpeed = 5.0f;
    float               moveVecX = 0.0f;
    float               moveVecZ = 0.0f;

    float               airControl = 0.3f;      // -にしたいけどfrictionとaccelerationが-になったとき値が
                                                // 思った通りにできないかもしれないので注意

    float               stepOffset = 1.0f;
    float               slopeRate = 1.0f;

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
};

//虚数複素数の強い番