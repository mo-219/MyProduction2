#include "Projectile.h"
#include "ProjectileManager.h"

#include "Graphics/Graphics.h"


// コンストラクタ
Projectile::Projectile(ProjectileManager* mana)  : manager(mana)
{
    mana->Registar(this);
}


// デバッグプリミティブ描画
void Projectile::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    debugRenderer->DrawSphere(DirectX::XMFLOAT3(position.x, position.y, position.z), radius, { 0,1,0,1 });

}

// 行列更新処理
void Projectile::UpdateTransform()
{
    // とりあえず回転は無視
    //transform._11 = scale.x;
    //transform._12 = 0.0f;
    //transform._13 = 0.0f;
    //transform._14 = 0.0f;

    //transform._21 = 0.0f;
    //transform._22 = scale.y;
    //transform._23 = 0.0f;
    //transform._24 = 0.0f;

    //transform._31 = 0.0f;
    //transform._32 = 0.0f;
    //transform._33 = scale.z;
    //transform._34 = 0.0f;

    //transform._41 = position.x;
    //transform._42 = position.y;
    //transform._43 = position.z;
    //transform._44 = 1.0f;

    DirectX::XMVECTOR Front, Up, Right;

    // 前ベクトルの算出
    Front = DirectX::XMLoadFloat3(&direction);
    Front = DirectX::XMVector3Normalize(Front);

    // 仮の上ベクトル
    DirectX::XMFLOAT3 vec = { 0.001f,1.0f,0.0f };
    Up = DirectX::XMLoadFloat3(&vec);
    Up = DirectX::XMVector3Normalize(Up);

    // 右ベクトルを算出
    Right = DirectX::XMVector3Cross(Up, Front);
    Right = DirectX::XMVector3Normalize(Right);


    // 上ベクトルを算出
    Up = DirectX::XMVector3Cross(Front, Right);
    Up = DirectX::XMVector3Normalize(Right);


    // 計算結果を取り出し
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, Right);
    DirectX::XMStoreFloat3(&up, Up);
    DirectX::XMStoreFloat3(&front, Front);

    // 注意！！！→必ずしもこの順番が正義であるわけではないa


    // 算出した軸ベクトルから行列を作成     スケーリングわからない！！！！！！！！
    transform._11 = right.x * scale.x;
    transform._12 = right.y * scale.x;
    transform._13 = right.z * scale.x;
    transform._14 = 0.0f;

    transform._21 = up.x * scale.y;
    transform._22 = up.y * scale.y;
    transform._23 = up.z * scale.y;
    transform._24 = 0.0f;

    transform._31 = front.x * scale.z;
    transform._32 = front.y * scale.z;
    transform._33 = front.z * scale.z;
    transform._34 = 0.0f;

    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    // 発射方向
    this->direction = front;
}

// 破棄
void Projectile::Destroy()
{
    manager->Remove(this);
}

