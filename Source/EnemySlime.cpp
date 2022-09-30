#include "EnemySlime.h"

// コンストラクタ
EnemySlime::EnemySlime()
{
    model = new Model("Data/model/Slime/Slime.mdl");

    // モデルが大きいのでスケーリング
    //scale.x = scale.y = scale.z = 0.001f;
    scale = { 0.01f, 0.01f, 0.01f };
    height = 1.0f;
}

// デストラクタ
EnemySlime:: ~EnemySlime()
{
    delete model;
}

// 更新処理
void EnemySlime::Update(float elapsedTime)
{
    // 速力処理
    UpdateVelocity(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);
    
    // オブジェクト行列を更新
    UpdateTransform();

    // モデル行列更新
    model->UpdateTransform(transform);
}

// 描画処理
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)    // ID3D11DeviceContextがdcになってる　エラー出たらcontextに変更
{
    shader->Draw(dc, model);
}

void EnemySlime::Render(const RenderContext& rc, ModelShader* shader)    // ID3D11DeviceContextがdcになってる　エラー出たらcontextに変更
{
    shader->Draw(rc, model);
}

// 死亡処理
void EnemySlime::OnDead()
{
    Destroy();
}