#include "ProjectileHoming.h"

ProjectileHoming::ProjectileHoming(ProjectileManager* mana) : Projectile(mana)
{
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
    model = new Model("Data/Model/Sword/Sword.mdl");

    // 表示サイズを調整
    //scale = { 0.5f, 0.5f, 0.5f };
    scale = { 3.0f, 3.0f, 3.0f };
}

ProjectileHoming::~ProjectileHoming()
{
    delete model;
}


// 更新処理
void ProjectileHoming::Update(float elapsedTime)
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
    {
        float speed = this->moveSpeed * elapsedTime;

        position.x += direction.x * speed;
        position.y += direction.y * speed;
        position.z += direction.z * speed;
    }

    // 旋回
    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        // ターゲットまでのベクトルを算出
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // ゼロベクトルでないなら回転処理
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3Length(Vec);
        float lengthSq = DirectX::XMVectorGetX(LengthSq);

        if (lengthSq > 0.00001f)        // floatは何回も何回も足すと誤差が生じる　その誤差分
        {
            // ターゲットまでのベクトルを単位ベクトル化
            Vec = DirectX::XMVector3Normalize(Vec);

            // 向いている方向ベクトルを算出
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            // 前方向ベクトルとターゲットまでのベクトルの内積(角度)を算出   ！じっさいのただしいかくどではない！！
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            // 2つの単位ベクトルの角度が小さいほど
            // 1.0に近づくという性質を利用して回転速度を調整する
            float rot = 1.0f - dot;
            if (rot > turnSpeed) rot = turnSpeed;


            // 回転角度があるなら回転処理をする
            if (fabs(rot) > 0.00001f)          //fabs()するのは回転角度がマイナスになることもあるから
            {
                // 回転軸を算出
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);      

                // AxisをNormalize(単位ベクトルにするため)
                Axis = DirectX::XMVector3Normalize(Axis);

                // 回転軸と回転量から回転行列を算出
                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                // 現在の行列を回転させる
                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                // 回転後の前方方向を取り出し、単位ベクトル化する
                Direction = DirectX::XMVector3Normalize(Transform.r[2]);
                DirectX::XMStoreFloat3(&direction, Direction);

            }
        }



    }

    // オブジェクト行列を更新
    UpdateTransform();

    // モデル行列更新
    model->UpdateTransform(transform);
}

// 描画処理
void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void ProjectileHoming::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction,
                              const DirectX::XMFLOAT3& position,
                              const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}