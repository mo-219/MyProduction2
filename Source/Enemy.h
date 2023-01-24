#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "Telegram.h"


// エネミー
class Enemy : public Character
{
public:
    Enemy() {}
    ~Enemy()override {}

    // 更新処理
    virtual void Update(float elapsedTime) = 0;
    virtual void UpdateOnlyTransform(float elapsedTime) = 0;
    
    // 描画処理
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
    virtual void Render(const RenderContext& rc, ModelShader* shader) = 0;

    // シェーダー用設定関数
    virtual RenderContext SetRenderContext(const RenderContext& rc) = 0;

    // 縄張り設定
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

    // 攻撃フラグ取得
    virtual bool	GetAttackFlg() { return attackFlg; }

    // 攻撃フラグセット
    virtual void	SetAttackFlg(bool flg) { attackFlg = flg; };
    virtual void	SetId(int id) { this->id = id; }
    virtual int		GetId() { return id; }

    // 攻撃範囲取得
    virtual float GetAttackRange() { return attackRange; }

    // メッセージ受信したときの処理
    virtual bool OnMessage(const Telegram& msg);


    // 破棄処理
    void Destroy();


    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive() {};

    virtual void DrawDebugImGui() {};

protected:

    DirectX::XMFLOAT3   targetPosition = { 0,0,0 };
    DirectX::XMFLOAT3   territoryOrigin = { 0,0,0 };
    float               territoryRange = 10.0f;

    int                 DropHeelItem = 0;


    bool attackFlg = false;
    int id = 0;

    float searchRange = 0.0f;
    float attackRange = 0.0f;
};