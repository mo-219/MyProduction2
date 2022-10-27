#include "ProjectileStraight.h"
#include "ProjectileManager.h"

ProjectileStraight::ProjectileStraight(ProjectileManager* mana) : Projectile(mana)
{
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
    model = new Model("Data/Model/Sword/Sword.mdl");

    // 表示サイズを調整
    //scale = { 0.5f, 0.5f, 0.5f };
    scale = { 3.0f, 3.0f, 3.0f };
}

ProjectileStraight::~ProjectileStraight()
{
    delete model;
}


// 更新処理
void ProjectileStraight::Update(float elapsedTime)
{
    // 寿命処理
    lifeTimer -= elapsedTime;
    if (lifeTimer < 0)
    {
        // 自分を削除
        Destroy();
        return;             // ここから下の処理やる必要がないから    またデストロイ予約してから何かしらの処理がある方が怖い 
    }


    // 移動
    float speed = this->speed * elapsedTime;

    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;

    // オブジェクト行列を更新
    UpdateTransform();

    // モデル行列更新
    model->UpdateTransform(transform);
}


// 描画処理
void ProjectileStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void ProjectileStraight::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}


// 発射
void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction,
                                const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}