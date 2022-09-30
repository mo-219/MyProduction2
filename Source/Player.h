#pragma once

#include "Graphics/shader.h"
#include "Graphics/Model.h"
#include "Character.h"

#include "ProjectileManager.h"

#include "Effect.h"

//プレイヤー
class Player : public Character
{
public:
    Player();
    ~Player();

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Render(const RenderContext& rc, ModelShader* shader);

    // デバッグ用GUI描画
    void DrawDebugGUI();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // ジャンプ入力処理
    void ImputJump();

protected:
    // 接地したときに呼ばれる
    void OnLanding() override;

private:
    // スティック入力値から移動ベクトルを取得
    DirectX::XMFLOAT3 GetMoveVec() const;


    // 移動入力処理
    void InputMove(float elapsedTime);


    // プレイヤーとエネミーの衝突処理
    void CollisionPlayerVsEnemies();

    // 弾丸と敵の衝突処理
    void CollisionProjectilesVsEnemies();

    // 弾丸入力処理
    void InputProjectile();

private:
    Model* model            = nullptr;
    float  moveSpeed        = 5.0f;

    float  turnSpeed        = DirectX::XMConvertToRadians(720);     // 720回転までおk

    // ジャンプに必要な変数
    float  jumpSpeed        = 20.0f;
    int    jumpCount        = 0;
    int    jumpLimit        = 2;

    //int    jumpRest         = 0;
    
    ProjectileManager projectileManager;

    Effect* hitEffect = nullptr;

};