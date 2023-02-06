#include "StageMoveFloor.h"

StageMoveFloor::StageMoveFloor()
{
    scale.x = scale.z = 3.0f;
    scale.y = 0.5f;

    // ステージモデル読み込み
    model = new Model("Data/Model/Cube/Cube.mdl");
}

StageMoveFloor::~StageMoveFloor()
{
    // ステージモデルを破棄
    delete model;
}

// 更新処理
void StageMoveFloor::Update(float elapsedTime)
{
    // 前回の情報を保存
    oldTransform = transform;
    oldAngle = angle;

    // スタートからゴールまでの距離算出
    DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR Goal = DirectX::XMLoadFloat3(&goal);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Goal, Start);
    DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

    float length;
    DirectX::XMStoreFloat(&length, Length);

    // スタートからゴールmでの間を一秒間で進む割合(0.0 ~ 1.0)を算出
    float speed = moveSpeed * elapsedTime;
    float SpeedRate = speed / length;
    moveRate += SpeedRate;

    // ゴールに到達またはスタートに戻った場合、移動方向を反転させる
    if (moveRate <= 0.0f || moveRate >= 1.0f)
    {
        moveSpeed = -moveSpeed;
    }

    // 線形補完で位置を算出
    DirectX::XMVECTOR Position = DirectX::XMVectorLerp(Start, Goal, moveRate);
    DirectX::XMStoreFloat3(&position, Position);

    // 回転
    angle.x += torque.x * elapsedTime;
    angle.y += torque.y * elapsedTime;
    angle.z += torque.z * elapsedTime;

    // 行列更新
    UpdateTransform();

    //model->UpdateTransform(transform);
    // レイキャスト用にモデル空間行列にするため単位行列を渡す
    const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0,
                                                    0,1,0,0,
                                                    0,0,1,0,
                                                    0,0,0,1 };
    model->UpdateTransform(transformIdentity);

}

// 描画処理
void StageMoveFloor::Render(const RenderContext& rc, ModelShader* shader)
{
    model->UpdateTransform(transform);
    shader->Draw(rc, model);
}

RenderContext StageMoveFloor::SetRenderContext(const RenderContext& rc)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;

    return myRc;
}

// レイキャスト
bool StageMoveFloor::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    //return Collision::IntersectRayVsModel(start, end, model, hit);

    // 前回のワールド行列の逆行列を求める
    DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&oldTransform);
    DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);


    // 前回のローカル空間でのレイに変換
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);

    DirectX::XMVECTOR LocalStart = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
    DirectX::XMVECTOR LocalEnd = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
    // 行列との変換

    // ローカル空間でのレイとの交点を求める
    DirectX::XMFLOAT3 localStart, localEnd;
    DirectX::XMStoreFloat3(&localStart, LocalStart);
    DirectX::XMStoreFloat3(&localEnd, LocalEnd);

    HitResult localHit;
    if (Collision::IntersectRayVsModel(localStart, localEnd, model, localHit))
    {
        WorldTransform = DirectX::XMLoadFloat4x4(&transform);
        DirectX::XMVECTOR LocalPosition = DirectX::XMLoadFloat3(&localHit.position);
        DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(LocalPosition, WorldTransform);

        DirectX::XMVECTOR LocalNormal = DirectX::XMLoadFloat3(&localHit.normal);
        DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(LocalNormal, WorldTransform);

        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
        DirectX::XMVECTOR Dist = DirectX::XMVector3Length(Vec);     // distanceは値が変わってしまっているので計算しなおす

        DirectX::XMStoreFloat3(&hit.position, WorldPosition);
        DirectX::XMStoreFloat3(&hit.normal, DirectX::XMVector3Normalize(WorldNormal));
        DirectX::XMStoreFloat(&hit.distance, Dist);
        hit.materialIndex = localHit.materialIndex;

        hit.rotation.x = angle.x - oldAngle.x;
        hit.rotation.y = angle.y - oldAngle.y;
        hit.rotation.z = angle.z - oldAngle.z;
        return true;
    }

    return false;
}

// 行列更新処理
void StageMoveFloor::UpdateTransform()
{
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.z, angle.y, angle.z);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX W = S * R * T;

    DirectX::XMStoreFloat4x4(&transform, W);
}
